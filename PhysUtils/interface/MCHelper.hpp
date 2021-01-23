/**
 * @file    MCHelper.hpp
 * @brief   Helper functions and variables for MC truth information crawling.
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifndef USERUTILS_PHYSUTILS_MCHELPER_HPP
#define USERUTILS_PHYSUTILS_MCHELPER_HPP

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include <functional>
#include <vector>

namespace usr {

/**
 * @defgroup MCHelper MC Truth Helpers
 * @brief    Collections of helpers to get parse the truth information
 * @ingroup  PhysUtils
 * @{
 */
namespace mc {

/**
 * @brief
 * Standard model particle ids in enum format (add negative sign for
 * anti-particle)
 */
enum PID
{
  DOWN_QUARK_ID    = 1,
  UP_QUARK_ID      = 2,
  STRANGE_QUARK_ID = 3,
  CHARM_QUARK_ID   = 4,
  BOTTOM_QUARK_ID  = 5,
  TOP_QUARK_ID     = 6,

  ELEC_ID   = 11,
  ELECNU_ID = 12,
  MUON_ID   = 13,
  MUONNU_ID = 14,
  TAU_ID    = 15,
  TAUNU_ID  = 16,

  GLUON_ID       = 21,
  GAMMA_ID       = 22,
  Z_BOSON_ID     = 23,
  W_BOSON_ID     = 24,
  HIGGS_BOSON_ID = 25
};

/*------------------------------------------------------------------------------
 * Simple wrapper class for that flags for topology parsing.
   ---------------------------------------------------------------------------*/
class ParticleParser
{
public:
  ParticleParser();
  ParticleParser( bool( * )( const reco::Candidate* ) );
  ParticleParser( bool( * )( const reco::Candidate*
                           , const reco::Candidate* ) );
  bool operator()( const reco::Candidate*, const reco::Candidate* ) const;
  bool is_null() const;

private:
  std::function<bool(const reco::Candidate*)> _single;
  std::function<bool(const reco::Candidate*,
                     const reco::Candidate*)> _double;
};
typedef ParticleParser pp;

/*------------------------------------------------------------------------------
 * Commonly used particle parsing functions.
   ---------------------------------------------------------------------------*/
bool IsSMQuark( const reco::Candidate* );
bool IsSMHadron( const reco::Candidate* );
bool HasCorrectiveParent( const reco::Candidate* );
bool HasCorrectiveChildren( const reco::Candidate* );
bool HasDaughter( const reco::Candidate*, const ParticleParser& );
bool HasMother( const reco::Candidate*, const ParticleParser& );
bool NoTermination( const reco::Candidate* );

/*------------------------------------------------------------------------------
 *  Generic MC truth topology crawling function.
   ---------------------------------------------------------------------------*/
std::vector<const reco::Candidate*> FindAll(
  const std::vector<reco::GenParticle>&,
  const ParticleParser& );

std::vector<const reco::Candidate*> FindDecendants(
  const reco::Candidate*,
  const ParticleParser&,
  const ParticleParser& = ParticleParser() );

std::vector<const reco::Candidate*> FindAncestors(
  const reco::Candidate*,
  const ParticleParser&,
  const ParticleParser& = ParticleParser() );


const reco::Candidate* GetLastInChain( const reco::Candidate* );

const reco::Candidate* GetFirstInChain( const reco::Candidate* );

const reco::Candidate* GetLeastCommonAncestor(
  const reco::Candidate*                    root,
  const std::vector<const reco::Candidate*> list );

const reco::Candidate* GetDaughter( const reco::Candidate*, int );

math::XYZPoint GetGenVertex( const std::vector<reco::GenParticle>& );


/** @} */

}/* mc */

}/* usr */


#endif/* end of include guard: USERUTILS_PHYSUTILS_MCHELPER_HPP */
