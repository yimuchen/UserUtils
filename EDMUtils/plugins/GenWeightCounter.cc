/**
 * @file    GenWeightCounter.cc
 * @brief   Summing of LHE weights per run
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 * @details
 */
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"

#include "UserUtils/Common/interface/Maths.hpp"
#include "UserUtils/Common/interface/STLUtils.hpp"
#include "UserUtils/EDMUtils/interface/Counter.hpp"
#include "UserUtils/EDMUtils/interface/PluginAlias.hpp"

/**
 * @brief A complete EDM plugin for calculating and caching generator weights.
 * @ingroup EDMUtils
 *
 * @details
 * Additional examples of how to allow a edm plugins to process run/lumi level
 * production could be found in the CMSSW
 * [EventCountProducer](http://cmsdoxygen.web.cern.ch/cmsdoxygen/CMSSW_10_1_7/doc/html/d0/d83/EventCountProducer_8cc_source.html)
 * Class.
 *
 * The rational of this class is that the sum-of-weights is frequently used for
 * calculating the selection efficiency and for isolating the impacts of
 * generator uncertainties (PDF and QCD scale) on acceptance and theoretical
 * cross sections.
 *
 * As the framework is setup such that run-level data can only be accessed at the
 * end fo the run, the user must define the weight families that needs to be
 * summed in the parameter set with the type `vint32` and under the name
 * "idlist". Weights listed in EDM files under the specified id's would first be
 * be divided by the weight under the central ID (i.e. the first id in the
 * provided list, then the normalized weights would be summed and stored at cache
 * level.
 *
 * One nuance that needs to be taken care of is the sign of the weights, since
 * using negative weights is a common strategy to correct for higher order
 * behaviours. The "divide" operation described earlier would only scale by the
 * absolute value of the central weight, leaving the sign of the various weights
 * untouched.
 *
 * Note that in the case that the user provides the weight id of 0, the input
 * file is assumed to be for data, and the sum of events will be calculated
 * instead (no additional PDF/QCD weights would be calculated).
 *
 * For reading the outputs of this plugin, read the deatiled sections of the
 * constructor.
 */
class GenWeightCounter :
  public usr::PluginAlias<edm::one::EDProducer<edm::one::WatchRuns,
                                               edm::EndRunProducer> >
{
public:
  explicit GenWeightCounter( const edm::ParameterSet& );
  ~GenWeightCounter();

private:
  virtual void produce( edm::Event&, const edm::EventSetup& ) override;
  virtual void beginRun( const edm::Run&, const edm::EventSetup& ) override;
  virtual void endRun( const edm::Run&, const edm::EventSetup& ) override;
  virtual void endRunProduce( edm::Run&, const edm::EventSetup& ) override;

  const edm::EDGetToken _lhesrc;
  std::vector<int> _idlist;

  double _evtsum;
  std::map<int, double> _sumlist;
};

using namespace edm;
using namespace std;

/**
 * @brief defining the requirements as wells as defining the output names.
 *
 * @details The parameterset passed to the plugins requires two things:
 *  - An input tag telling the plugin where to extract the lhe information
 *  - A `vint32` telling the plugin which weight variation to take into
 *    calculation. If you unsure which weight variations you should use, see
 *    [this
 *    page](https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideDataFormatGeneratorInterface)
 *    for more information. If you unsure which weight variations is contained in
 *    your edm file, consider the [DumpLHERunInfo](@ref DumpLHERunInfo.cc)
 *    command provided by yours truly.
 *
 * The output is stored in the form of the new usr::Counter class in the run
 * level, under the name "EventsCount" (for the simple sum of events), and the
 * "WeightSum<id>" for the sum-of-weight of a specific variation.
 */
GenWeightCounter::GenWeightCounter( const edm::ParameterSet& iConfig ) :
  PluginAlias( iConfig ),
  _lhesrc( GetToken<LHEEventProduct>( "lhesrc" ) ),
  _idlist( usr::RemoveDuplicate( iConfig.getParameter<std::vector<int> >( "idlist" ) ) )
{
  produces<usr::Counter, edm::InRun>( "EventsCount" );

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

  _evtsum = 0;
}

/*----------------------------------------------------------------------------*/

void
GenWeightCounter::produce( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
  edm::Handle<LHEEventProduct> _lhehandle;
  const int centralid = _idlist.front();
  std::map<int, double> rawweights;

  iEvent.getByToken( _lhesrc, _lhehandle );

  if( !_lhehandle.isValid() ){// Data, or LHE information is unavailable
    _sumlist.at( 0 ) += 1;
    _evtsum          += 1;
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

  std::unique_ptr<usr::Counter> ptr( new usr::Counter( _evtsum ) );
  iRun.put( std::move( ptr ), "EventsCount" );
}

/*----------------------------------------------------------------------------*/

void
GenWeightCounter::endRun( const edm::Run&, const edm::EventSetup& )
{}

/*----------------------------------------------------------------------------*/

DEFINE_FWK_MODULE( GenWeightCounter );
