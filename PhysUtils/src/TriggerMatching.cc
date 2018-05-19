/*******************************************************************************
*
*  Filename    : TriggerMatching.cc
*  Description : Implementation of trigger matching algorithms
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*  Main reference :
*    https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookMiniAOD2016#Trigger
*******************************************************************************/
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/RecoCandidate/interface/RecoCandidate.h"
#include "FWCore/Common/interface/TriggerNames.h"

#include "UserUtils/PhysUtils/interface/TriggerMatching.hpp"
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"

#include <regex>

#include "TLorentzVector.h"

namespace usr {

/*******************************************************************************
*   User Level functions
*******************************************************************************/
bool
HasTriggerMatch(
  const reco::Candidate&                        recoobj,
  const pat::TriggerObjectStandAloneCollection& trgobjlist,
  const std::string&                            reqtrigger,
  const std::string&                            reqfilter,
  const edm::TriggerNames&                      trgnames,
  const double                                  mindelta_r
  )
{
  auto match = MatchTrgObj(
    recoobj,
    trgobjlist,
    reqtrigger,
    reqfilter,
    trgnames,
    mindelta_r );
  return match != trgobjlist.end();
}

/******************************************************************************/

TLorentzVector
TriggerMatchP4(
  const reco::Candidate&                        recoobj,
  const pat::TriggerObjectStandAloneCollection& trgobjlist,
  const std::string&                            reqtrigger,
  const std::string&                            reqfilter,
  const edm::TriggerNames&                      trgnames,
  const double                                  mindelta_r
  )
{
  auto match = MatchTrgObj(
    recoobj,
    trgobjlist,
    reqtrigger,
    reqfilter,
    trgnames,
    mindelta_r );
  if( match != trgobjlist.end() ){
    return TLorentzVector( match->px(), match->py(), match->pz(), match->energy() );
  } else {
    return TLorentzVector( 0, 0, 0, 0 );
  }
}

/*******************************************************************************
*   Core function implementations
*******************************************************************************/

pat::TriggerObjectStandAloneCollection::const_iterator
MatchTrgObj(
  const reco::Candidate&                        recoobj,
  const pat::TriggerObjectStandAloneCollection& trgobjlist,
  const std::string&                            reqtrigger,
  const std::string&                            reqfilter,
  const edm::TriggerNames&                      trgnames,
  const double                                  mindelta_r
  )
{
  for( auto objiter = trgobjlist.begin(); objiter != trgobjlist.end(); ++objiter ){
    auto obj = *objiter;  // making duplicate
    obj.unpackPathNames( trgnames );

    if( TrigObjMatchPathFilter( obj, reqtrigger, reqfilter )
        && reco::deltaR( recoobj, obj ) < mindelta_r ){
      return objiter;
    }
  }

  return trgobjlist.end();
}

/******************************************************************************/

bool
TrigObjMatchPathFilter(
  const pat::TriggerObjectStandAlone& obj,
  const std::string&                  path,
  const std::string&                  filter
  )
{
  bool hasmatchpath   = false;
  bool hasmatchfilter = false;

  const std::regex pathreg( usr::GlobToRegex( path ) );

  for( const auto& objpath : obj.pathNames() ){
    if( std::regex_match( objpath, pathreg ) && obj.hasPathName( objpath ) ){
      hasmatchpath = true;
      break;
    }
  }

  const std::regex filterreg( usr::GlobToRegex( filter ) );

  for( const auto& objfilter : obj.filterLabels() ){
    if( std::regex_match( objfilter, filterreg ) && obj.hasFilterLabel( objfilter ) ){
      hasmatchfilter = true;
      break;
    }
  }

  return hasmatchpath && hasmatchfilter;
}

} /* usr */
