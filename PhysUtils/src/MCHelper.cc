/**
 * @file MCHelper.cc
 * @author Yi-Mu "Enoch" Chen
 * @brief Implementing the functions required for MC topology crawling.
 */
#include "DataFormats/Candidate/interface/Candidate.h"

#include "UserUtils/Common/interface/STLUtils/VectorUtils.hpp"
#include "UserUtils/PhysUtils/interface/MCHelper.hpp"

#include <queue>
#include <stack>

namespace usr {

/**
 * @brief returning pointer to the particle in the parent topology with a
 *        specified flavour.
 *
 * For an input particle x, it will traverse the topology towards the parent
 * side, until
 * 1. the parent particle has the target flavour F, in which case it returns the
 *    pointer to the parent particle. Note that the absolute value will be taken
 *    for the for the flavour.
 * 2. The parent particle has a flavour different to x, in which case a null
 *    pointer is return.
 *
 * This function help find direct parent of a particle while avoid the
 * possibility of final state radiations affecting the topology parsing. For
 * example, the following topology is actually rather common in CMSSW truth
 * topologies:
 *
 * \f[F\rightarrow(x+g)\rightarrow(x'+gg)\rightarrow(x''+ggg) \f]
 *
 * The three function calls GetDirectMother( x, F ), GetDirectMother( x', F ),
 * and GetDirectMother( x'', F ) should all return the same pointer F.
 *
 *  Topologies in MiniAOD also have some times have the topologies like: \f$x
 *  \rightarrow x' \rightarrow x''\f$, where  the radiation particles are dropped
 *  to save space. This function will also avoid cases like this.
 */
const reco::Candidate*
GetDirectMother( const reco::Candidate* x, int target_ID )
{
  std::queue<const reco::Candidate*> bfs_queue;
  bfs_queue.push( x );

  while( !bfs_queue.empty() ){
    const reco::Candidate* temp = bfs_queue.front();
    bfs_queue.pop();
    if( abs( temp->pdgId() ) == abs( target_ID ) ){
      return temp;
    }

    for( unsigned i = 0; i < temp->numberOfMothers(); ++i ){
      if( temp->mother( i )->pdgId() == temp->pdgId() ){
        bfs_queue.push( temp->mother( i ) );
      } else if( abs( temp->mother( i )->pdgId() ) == abs( target_ID ) ){
        return temp->mother( i );
      }
    }
  }

  return nullptr;
}

/**
 * @brief Essentially the same as the GetDirectMother functions, except this time
 *        the traversal directions is towards the children side.
 *
 * It will follow the same flavour decaying chain until there is a flavour
 * change. A major caveat of this functions would be if the decay topology you
 * are interested in is on the otherside of the hard radiation photon, gluon or Z
 * boson. In these cases, the user should be careful that this function is
 * returning what is desired.
 */
const reco::Candidate*
GetDaughter( const reco::Candidate* x, int target_ID )
{
  std::queue<const reco::Candidate*> bfs_queue;
  bfs_queue.push( x );

  while( !bfs_queue.empty() ){
    const reco::Candidate* temp = bfs_queue.front();
    bfs_queue.pop();
    if( abs( temp->pdgId() ) == abs( target_ID ) ){
      // Moving to bottom of single decay chain
      while( temp->numberOfDaughters() == 1 ){
        temp = temp->daughter( 0 );
      }

      return temp;
    }

    for( unsigned i = 0; i < temp->numberOfDaughters(); ++i ){
      bfs_queue.push( temp->daughter( i ) );
    }
  }

  return nullptr;
}

/**
 * @brief Check if particles is intermediate part in the decay chain (i.e it has
 * radiative corrections to the particle momentum)
 *
 * This function checks to see if the input particle has any daughter particle
 * has the same partice id as the input particle. If such a daughter particle
 * exists, then the particle is considered intermediate.
 */
bool
IsIntermediate( const reco::Candidate* x )
{
  for( unsigned i = 0; i < x->numberOfDaughters(); ++i ){
    if( x->daughter( i )->pdgId() == x->pdgId() ){
      return true;
    }
  }

  return false;
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
  if( x->numberOfDaughters() == 0 ){ return x; }

  for( unsigned i = 0; i < x->numberOfDaughters(); ++i ){
    if( x->daughter( i )->pdgId() == x->pdgId() ){
      if( x->numberOfDaughters() <= 2 ){
        return GetLastInChain( x->daughter( i ) );
      } else {
        const double pt  = x->pt();
        const double dpt = x->daughter( i )->pt();

        if( fabs( dpt - pt ) / pt < 0.1 ){
          return GetLastInChain( x->daughter( i ) );
        }
      }
    }
  }


  return x;
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
 * @brief Get the decendants of a particle in the decay topology that matches a
 * certain criteria.
 *
 * The user is responsible to passing the function uses to processes a candidate
 * particle in the decay topology. Currently the function must be self-contained
 * (the function argument is only the decandent candidate).
 *
 * The user can also add a flag that tells the function how deep into the
 * topology to fetch the decendants. As the decay topology is difficult to define
 * in terms of decay 'depth', with radiative correction sharing the same topology
 * as regular decays, the only two options are for immedidate and all.
 */
std::vector<const reco::Candidate*>
GetDecendants(
  const reco::Candidate* x,
  bool (*                func )( const reco::Candidate* ),
  const int              flag  )
{
  std::vector<const reco::Candidate*> ans;
  std::queue<const reco::Candidate*> bfs_queue;
  bfs_queue.push( GetLastInChain( x ) );

  auto PushQueue = [&bfs_queue]( const reco::Candidate* m ){
                     for( unsigned i = 0; i < m->numberOfDaughters(); ++i ){
                       bfs_queue.push( GetLastInChain( m->daughter( i ) ) );
                     }
                   };

  while( !bfs_queue.empty() ){
    const reco::Candidate* temp = bfs_queue.front();
    bfs_queue.pop();

    if( func( temp ) && temp != x ){
      ans.push_back( temp );
      if( flag != GET_IMMEDIATE ){
        PushQueue( temp );
      }
    } else {
      PushQueue( temp );
    }

  }

  return ans;
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
 * @brief Simple function to check if particle is a hadron.
 *
 * PDGID paring flag. Not accurate but close enough for 99.9% of use cases
 */
bool
IsSMHadron( const reco::Candidate* x )
{
  return abs( x->pdgId() ) >  100 && abs( x->pdgId() ) < 100000;
}


}/* usr */
