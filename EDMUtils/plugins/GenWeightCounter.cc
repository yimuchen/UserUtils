/**
 * @file    GenWeightCounter
 * @brief   Summing of LHE weights per run
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 * @details
 *  Main documentation of the framework tools used could be found in the CMSSW
 *  EventCountProducer Class.
 *
 *  The rational of this class is that the sum-of-weights is frequently used
 *  for calculating the selection efficiency and for isolating the impacts of
 *  generator uncertainties (PDF and QCD scale) on acceptance and theoretical
 *  cross sections.
 *
 *  Due to the framework setup, the user must define the weight families that
 *  needs to be summed in the python settings file. The python setting file
 *  will include lists of common weights id, as well as provide a command for
 *  dumping the weights id that exist in an EDM file. The first weight in the
 *  list corresponds to the "central" weight, to be used, and only the sign
 *  will be used
 *
 *  Note that in the case that the user provides the weight id of 0, the input
 *  file is assumed to be for data, and the sum of events will be calculated
 *  instead.
 */
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
// user include files
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"

#include "UserUtils/Common/interface/STLUtils.hpp"
#include "UserUtils/Common/interface/Maths.hpp"
#include "UserUtils/EDMUtils/interface/Counter.hpp"
#include "UserUtils/EDMUtils/interface/PluginAlias.hpp"

/*-----------------------------------------------------------------------------
 * Class definition
   ---------------------------------------------------------------------------*/
class GenWeightCounter :
  public edm::one::EDProducer<edm::one::WatchRuns, edm::EndRunProducer>,
  public virtual usr::PluginAlias
{
public:
  explicit
  GenWeightCounter( const edm::ParameterSet& );
  ~GenWeightCounter();

private:
  virtual void produce( edm::Event&, const edm::EventSetup& ) override;
  virtual void beginRun( const edm::Run&, const edm::EventSetup& ) override;
  virtual void endRun( const edm::Run&, const edm::EventSetup& ) override;
  virtual void endRunProduce( edm::Run&, const edm::EventSetup& ) override;

  // Common settings objects
  const edm::EDGetToken _lhesrc;
  std::vector<int> _idlist;

  // Active variable for summing
  std::map<int, double> _sumlist;

  // Helper private functions
};

using namespace edm;
using namespace std;

/*------------------------------------------------------------------------------
 *   Constructor and destructor - Setting up naming
   ---------------------------------------------------------------------------*/
GenWeightCounter::GenWeightCounter( const edm::ParameterSet& iConfig ) :
  PluginAlias( iConfig ),
  _lhesrc( GetToken<LHEEventProduct>( "lhesrc" ) ),
  _idlist( usr::RemoveDuplicate( iConfig.getParameter<std::vector<int> >( "idlist" ) ) )
{
  // Always alias a "central sum"
  produces<usr::Counter, edm::InRun>( "EventsCount" );

  // General sum-of-weight containers
  for( const auto id : _idlist ){
    produces<usr::Counter, edm::InRun>( "WeightSum" + std::to_string( id ) );
    _sumlist[id] = 0;
  }

  _sumlist[0] = 0;
}

GenWeightCounter::~GenWeightCounter(){}

/*-----------------------------------------------------------------------------
 *   Main control flow
   ---------------------------------------------------------------------------*/
void
GenWeightCounter::beginRun( const edm::Run&, const edm::EventSetup& )
{
  for( auto& sum  : _sumlist ){
    sum.second = 0;
  }
}

/*----------------------------------------------------------------------------*/

void
GenWeightCounter::produce( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
  edm::Handle<LHEEventProduct> _lhehandle;
  const int centralid = _idlist.front();
  std::map<int, double> rawweights;

  iEvent.getByToken( _lhesrc, _lhehandle );

  if( !_lhehandle.isValid() ){ // Data, or LHE information is unavailable
    _sumlist.at( 0 ) += 1;
    return;
  } else {
    // Getting the required raw weights.
    for( const auto& weight : _lhehandle->weights() ){
      const int id = std::stoi( weight.id );
      if( _sumlist.count( id ) ){
        rawweights[id] = weight.wgt;
      }
    }

    const double centralweight = rawweights.at( centralid );

    for( const auto& weight : rawweights ){
      const int id           = weight.first;
      const double rawweight = weight.second;
      _sumlist.at( id ) += std::fabs( rawweight/centralweight )
                           * usr::sgn( rawweight );
    }

    _sumlist.at( 0 ) += usr::sgn( centralweight );
  }
}

/*----------------------------------------------------------------------------*/

void
GenWeightCounter::endRunProduce( edm::Run& iRun, const EventSetup& iSetup )
{
  for( const auto sumpair : _sumlist ){
    if( sumpair.first == 0 ){ continue; }
    const int id        = sumpair.first;
    const double sumval = sumpair.second;
    std::unique_ptr<usr::Counter> ptr( new usr::Counter( sumval ) );
    const std::string name = "WeightSum" + std::to_string( id );

    iRun.put( std::move( ptr ), name );
  }

  std::unique_ptr<usr::Counter> ptr(
    new usr::Counter( _sumlist.at( _idlist.front() ) ) );
  iRun.put( std::move( ptr ), "EventsCount" );
}

/*----------------------------------------------------------------------------*/

void
GenWeightCounter::endRun( const edm::Run&, const edm::EventSetup& )
{}

/*----------------------------------------------------------------------------*/

DEFINE_FWK_MODULE( GenWeightCounter );
