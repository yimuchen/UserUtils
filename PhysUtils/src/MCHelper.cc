/**
 * @file MCHelper.cc
 * @author Yi-Mu "Enoch" Chen
 * @brief Implementing the functions required for MC topology crawling.
 */
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "UserUtils/Common/interface/STLUtils/VectorUtils.hpp"
#include "UserUtils/PhysUtils/interface/MCHelper.hpp"

#include <queue>
#include <stack>

namespace usr {

namespace mc {


/**
 * @class ParticleParser
 * @brief Simple interface for wrapping a two particle function and thw 1
 * particle function in the same interface
 *
 * This class uses the std::function class to stored instances of functions that
 * stores the function pointer to singe reco::Candidate input function, and two
 * reco::Candidate input functions. Only one of these functions should be
 * non-nullptr, and this is used to run the standard operator() process.
 */
ParticleParser::ParticleParser() :
  _single( nullptr ),
  _double( nullptr )
{}

ParticleParser::ParticleParser( bool( *x )( const reco::Candidate* ) ) :
  _single( x ),
  _double( nullptr ){}

ParticleParser::ParticleParser( bool( *y )( const reco::Candidate*
                                          , const reco::Candidate* ) ) :
  _single( nullptr ),
  _double( y )
{}

bool
ParticleParser::operator()( const reco::Candidate* x, const reco::Candidate* y ) const
{
  if( _single != nullptr ){ return _single( x ); } else {return _double( x, y );}
}

bool
ParticleParser::is_null() const
{
  return _single == nullptr && _double == nullptr;
}

/**
 * @{
 * @brief  Using PDGID to check if the particle is a SM particle of a certain
 * type.
 */
bool
IsSMQuark( const reco::Candidate* x )
{
  return abs( x->pdgId() ) <= 6;
}

bool
IsSMHadron( const reco::Candidate* x )
{
  // Not accurate but close enough for 99.9% of use cases
  return abs( x->pdgId() ) >  100 && abs( x->pdgId() ) < 100000;
}

/** @} */

/**
 * @brief Has daughter particle that matches a certain criteria
 *
 * The input ParticleParser object can take up to two inputs. The first being the
 * daughter particle, the second being the main reference particle.
 */
bool
HasDaughter( const reco::Candidate* x, const ParticleParser& is_target )
{
  for( unsigned i = 0; i < x->numberOfDaughters(); ++i ){
    if( is_target( x->daughter( i ), x ) ){
      return true;
    }
  }

  return false;
}

/**
 * @brief Has mother particle that matches a certain criteria
 *
 * The input ParticleParser object can take up to two inputs. The first being the
 * mother particle, the second being the main reference particle.
 */
bool
HasMother( const reco::Candidate* x, const ParticleParser& is_target )
{
  for( unsigned i = 0; i < x->numberOfMothers(); ++i ){
    if( is_target( x->mother( i ), x ) ){
      return true;
    }
  }

  return false;
}


/**
 * @brief Checking if the particle has radiative correction particle in
 * neighboring topology.
 *
 * Radiative corrections is current defined as:
 * - The neighbor only a single particle with the same PDGID.
 * - That child has pt within 10% of the parent pt.
 *
 * If both are satisfied, the function returns true, false otherwise.
 */
bool
HasCorrectiveParent( const reco::Candidate* x )
{
  bool is_close  = false;
  bool is_unique = false;

  for( unsigned i = 0; i < x->numberOfMothers(); ++i ){
    if( x->mother( i )->pdgId() == x->pdgId() ){
      const double pt  = x->pt();
      const double mpt = x->mother( i )->pt();

      if( ( fabs( mpt - pt ) / mpt ) < 0.1 ){
        is_close = true;
      }

      if( is_unique == false ){
        is_unique = true;
      } else {
        is_unique = false;
        break;
      }
    }
  }

  return is_close && is_unique;
}

bool
HasCorrectiveChildren( const reco::Candidate* x )
{
  bool is_close  = false;
  bool is_unique = false;

  for( unsigned i = 0; i < x->numberOfDaughters(); ++i ){
    if( x->daughter( i )->pdgId() == x->pdgId() ){
      const double pt  = x->daughter( i )->pt();
      const double mpt = x->pt();

      if( ( fabs( mpt - pt ) / mpt ) < 0.1 ){
        is_close = true;
      }

      if( is_unique == false ){
        is_unique = true;
      } else {
        is_unique = false;
        break;
      }
    }
  }

  return is_close && is_unique;
}
/** @} */

/**
 * @brief Function used for getting all topology particle in
 * BFS order.
 */
bool
Any( const reco::Candidate* x )
{
  return true;
}

/**
 * @brief Function required for no-termination condition in topology crawling.
 */
bool
NoTermination( const reco::Candidate* x )
{
  return false;
}


/**
 * @brief Returning a list of gen particles pointer that match a certain
 * selection criteria
 *
 * This function simply loops over all the particles in the given gen particle
 * list and pass all objects the the calculation function. The ordering of the
 * return list would be according to when they appear in the master list.
 */
std::vector<const reco::Candidate*>
FindAll(
  const std::vector<reco::GenParticle>& genlist,
  const ParticleParser&                 is_target )
{
  std::vector<const reco::Candidate*> ans;

  for( const auto& gen : genlist ){
    if( is_target( &gen, nullptr ) ){
      ans.push_back( &gen );
    }
  }

  return ans;
}

/**
 * @brief Static function for running the breath-first search algorithm on the
 * decay topology.
 *
 * Given a staring particle x, a target function is_target, the function returns
 * a list of particle points for particles that matches the targer function. For
 * each particle during the BFS, the sequence is_terminate is parsed, if true,
 * then the BFS queue is no-longer extended, if false, the push function is
 * called to extend the BFS queue at the current BFS number.
 *
 * The is_target and is_terminate function can either be functions that take in
 * one particle: in which case the argument would be the current particle in the
 * BFS interation, or two particles in which case the two arguments would be the
 * current particle in the BFS interation and the starting particle.
 */
static std::vector<const reco::Candidate*>
RunBFS( const reco::Candidate* x,
        const ParticleParser&  is_target,
        const ParticleParser&  is_terminate,
        void (*                push )( std::queue<const reco::Candidate*>&,
          const reco::Candidate* ) )
{
  std::vector<const reco::Candidate*> ans;
  std::queue<const reco::Candidate*> bfs_queue;
  bfs_queue.push( x );

  while( !bfs_queue.empty() ){
    const reco::Candidate* temp = bfs_queue.front();
    bfs_queue.pop();

    if( is_target( temp, x ) && !usr::FindValue( ans, temp ) ){
      ans.push_back( temp );
    }

    if( !is_terminate( temp, x ) ){
      push( bfs_queue, temp );
    }
  }

  return ans;
}

/**
 * @brief Get the decendants of a particle in the decay topology that matches a
 * certain criteria.
 *
 * The user is responsible to passing the function uses to processes a candidate
 * particle in the decay topology. Currently the function must be self-contained
 * (the function argument is only the decandent candidate as the first candidate
 * and the input particle as the center particle). The return results will be
 * ordered in terms of the BFS search sequence, so objects that are "closer" to
 * the given particle will always be added.
 *
 * Notice that since the decay toplogy is not a strict tree structure (example
 * for hadronizations, two quarks can both be the parent of the resulting tow
 * hadrons), the return result will only contain the first instance at which a
 * particle if found in the decay chain.
 *
 * To save on time, the user can also provide a function to indicate that the BFS
 * termination criteria. If the particle visited by the BFS algorithm matches the
 * critera, then the particles daughters will not be added to BFS queue. In case
 * non is provided, the termination function will be the same as the target
 * function. so that as soon a particle if found as the target, the BFS search
 * will be terminated.
 */
std::vector<const reco::Candidate*>
FindDecendants(  const reco::Candidate* x,
                 const ParticleParser&  is_target,
                 const ParticleParser&  is_terminate  )
{
  const ParticleParser& term = is_terminate.is_null() ? is_target : is_terminate;
  return RunBFS( x, is_target, term,
    []( std::queue<const reco::Candidate*>& q
      , const reco::Candidate* m ){
        for( unsigned i = 0; i < m->numberOfDaughters(); ++i ){
          q.push( m->daughter( i ) );
        }
      } );
}

/**
 * @brief Get the ancestors of a particle in the decay topology that matches a
 * certain criteria.
 *
 * The user is responsible to passing the function uses to processes a candidate
 * particle in the decay topology. Currently the function must be self-contained
 * (the function argument is only the decandent candidate). The return results
 * will be ordered in terms of the BFS search sequence, so objects that are
 * "closer" to the given particle will always be added.
 *
 * Notice that since the decay toplogy is not a strict tree structure (example
 * for hadronizations, two quarks can both be the parent of the resulting tow
 * hadrons), the return result will only contain the first instance at which a
 * particle if found in the decay chain.
 *
 * To save on time, the user can also provide a function to indicate that the BFS
 * termination criteria. If the particle visited by the BFS algorithm matches the
 * critera, then the particles mothers will not be added to BFS queue. In case
 * non is provided, the termination function will be the same as the target
 * function. so that as soon a particle if found as the target, the BFS search
 * will be terminated.
 */
std::vector<const reco::Candidate*>
FindAncestors( const reco::Candidate* x,
               const ParticleParser&  is_target,
               const ParticleParser&  is_terminate  )
{
  const ParticleParser& term = is_terminate.is_null() ? is_target : is_terminate;
  return RunBFS( x, is_target, term,
    []( std::queue<const reco::Candidate*>& q,
        const reco::Candidate* m ){
        for( unsigned i = 0; i < m->numberOfMothers(); ++i ){
          q.push( m->mother( i ) );
        }
      } );
}

/**
 * @brief Get the last particle in a radiative correction chain
 *
 * The particle in the radiative correction chain should only have two daughter
 * particles : The corrected particle and the radiated particle if exited. If
 * more daughter particles exists, then it is doing something non-trivial, and we
 * will not consider this part of the chain.
 */
const reco::Candidate*
GetLastInChain( const reco::Candidate* x )
{
  ParticleParser is_last( []( const reco::Candidate* temp,
                              const reco::Candidate* root ){
                          return ( temp->pdgId() == root->pdgId() ) &&
                          !HasCorrectiveChildren( temp );
        } );
  ParticleParser is_end( []( const reco::Candidate* temp,
                             const reco::Candidate* root ){
                         return ( temp->pdgId() !=  root->pdgId() ) ||
                         !HasCorrectiveChildren( temp );
        } );

  const auto ans_list = FindDecendants( x, is_last, is_end );

  if( ans_list.size() == 0 ){
    return x;
  } else {
    return ans_list.back();
  }
}

/**
 * @brief Get the First particle in the radiative correction chain
 *
 * Tracing the parent until it doesn't have a parent having the same particle ID.
 */
const reco::Candidate*
GetFirstInChain( const reco::Candidate* x )
{
  for( unsigned i = 0; i < x->numberOfMothers(); ++i ){
    if( x->mother( i )->pdgId() == x->pdgId() ){
      return GetFirstInChain( x->mother( i ) );
    }
  }

  return x;
}


/**
 * @brief Get the Least Common Ancestor in the decay topology
 *
 * Useful for finding the particle resposible for creating the particle shower
 * vertex. The root indicates a guaranteed ancestor for all the particles in the
 * list.
 *
 * Note that the decay topology in the hadronization phase isn't guaranteed to be
 * a tree topology. There are topologies like: \f$x\rightarrow ddgg\rightarrow
 * pp\pi\pi\f$, where the all \f$ddgg\f$ are technically the parents of the
 * \f$pp\pi\pi\f$ particles in the final state from the hadronization process,
 * but neither is a physically significant common ancestor. In this case, the
 * function should try and return the particle \f$x\f$ when given the
 * \f$pp\pi\pi\f$ as the input, instead of any of the \f$ddgg\f$ particles.
 *
 * Since non-single parents only occurs in the decay topology (TO FACT CHECK),
 * what this function will do is that the function will first check for the
 * non-single parent particles if the exists, the function will rerun the
 * function with the parent particles replacing the non-single parent particle as
 * the input.
 *
 * The Least Common Ancestor Algorithm for the tree-like topology uses the hash
 * table method since parent pointers area available: for each member in the
 * list, trace the ancestry to the root node and track the number of times a
 * parent particles has been traversed. The particle return will be the one with
 * the maximum number of traverse count (not returning an exact results, since
 * the there will be accidental merging in the verticies)
 */
const reco::Candidate*
GetLeastCommonAncestor( const reco::Candidate*                    root,
                        const std::vector<const reco::Candidate*> list )
{
  // Checking for non-singular parent, recalling function if exists
  std::vector<const reco::Candidate*> working_list;

  for( const auto part : list ){
    if( part->numberOfMothers() > 1 ){
      for( unsigned i = 0; i < part->numberOfMothers(); ++i ){
        if( part->mother( i ) == root ){
          return root;
        }
        working_list.push_back( part->mother( i ) );
      }
    } else {
      working_list.push_back( part );
    }
  }

  if( list.size() < working_list.size() ){
    working_list = usr::RemoveDuplicate( working_list );
    return GetLeastCommonAncestor( root, working_list );
  }

  if( list.size() == 1 ){
    return list.at( 0 );
  }

  // Constructing ancestor map using the other elements. Basically counting the
  // number of times a particles is passed when tracing each particles ancestry.
  std::map<const reco::Candidate*, unsigned> ancestor_map;

  for( unsigned i = 0; i < list.size(); ++i ){
    const reco::Candidate* p = list.at( i );

    while( p->numberOfMothers() == 1 && p != root ){// Parent pointer traversal
      if( ancestor_map.count( p ) ){
        ancestor_map[p]++;
      } else {
        ancestor_map[p] = 1;
      }

      p = p->mother( 0 );
    }
  }

  const reco::Candidate* ans = (const reco::Candidate*)( ~0llu );
  unsigned nref              = 0;

  for( const auto p : ancestor_map ){
    if( p.second > nref ){
      ans  = p.first;
      nref = p.second;
    } else if( p.second == nref && p.first > ans ){
      ans  = p.first;
      nref = p.second;
    }
  }

  // Returning root if nothing is found.
  return ans;
}


/**
 * @brief Get the Gen-Level Vertex position from the list of gen particles
 *
 * Notice that the particle assuming the incoming protons are always the first in
 * the list. In the case that these assertions fail, the point return will have
 * all three coordinates set to -100
 */
math::XYZPoint
GetGenVertex( const std::vector<reco::GenParticle>& genlist )
{
  static const math::XYZPoint default_ans( -100, -100, -100 );

  if( fabs( genlist.front().pdgId() ) != 2212 ){ return default_ans; }
  if( genlist.front().status() != 4 ){ return default_ans; }

  const auto part = genlist.front().daughter( 0 );
  // Getting the first daughter particle

  if( !part ){ return default_ans; }
  return part->vertex();
}

/**
 * @brief Get the closest ancestor that matches the given PDG id
 *
 * Using a BFS algorithm to find the closest ancestor
 */
const reco::Candidate*
FindAncestor( const reco::Candidate* x, int pdgid )
{
  std::queue<const reco::Candidate*> bfs_queue;
  bfs_queue.push( x );

  while( !bfs_queue.empty() ){
    const reco::Candidate* temp = bfs_queue.front();
    bfs_queue.pop();
    if( temp->pdgId() == pdgid ){
      return temp;
    }

    for( unsigned i = 0; i < temp->numberOfMothers(); ++i ){
      bfs_queue.push( temp->mother( i ) );
    }
  }

  return nullptr;
}

}/* mc */

}/* usr */
