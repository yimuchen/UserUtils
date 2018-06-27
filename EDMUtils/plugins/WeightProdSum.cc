/**
 * @file    WeightProdSum.cc
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 * @brief   product of listed weights with run level caching
 */
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "UserUtils/EDMUtils/interface/Counter.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

/**
 * @brief class for calculating the product of weight.
 * @ingroup EDMUtils
 *
 * @details
 * In HEP analysis, simulated events are typically weighted by various scale
 * factors. and the actual "number of events" represented by a single simulated
 * event would be product of all the various scale events. This plugin does
 * just that: given a list of weight to use, calculated the product of all
 * the weights, storing the per-event weight product, and also saving the
 * sum of the weight product per run.
 */
class WeightProdSum :
  public edm::one::EDProducer<edm::one::WatchRuns, edm::EndRunProducer>
{
public:
  explicit
  WeightProdSum( const edm::ParameterSet& );
  ~WeightProdSum();

private:
  virtual void beginRun( const edm::Run&, const edm::EventSetup& ) override;
  virtual void endRun( const edm::Run&, const edm::EventSetup& ) override;
  virtual void endRunProduce( edm::Run&, const edm::EventSetup& ) override;
  virtual void produce( edm::Event&, const edm::EventSetup& ) override;

  // Getting objects from vector of sums
  std::vector<edm::EDGetToken> _weightsrclist;
  edm::Handle<double> _weighthandle;

  double _weightsum;
};

/**
 * Construction expects the a parameter of type vInputTag, listing the
 * doubles to be used in the product calculation. The weight product
 * per-event would be stored under the label "WeightProd", and the sum of
 * weight products stored at run level would be under the object label
 * "WeightSum" (requires the usr::Counter class to read).
 */
WeightProdSum::WeightProdSum( const edm::ParameterSet& iConfig )
{
  const auto taglist
    = iConfig.getParameter<std::vector<edm::InputTag> >( "weightlist" );

  for( const auto& tag : taglist ){
    _weightsrclist.push_back( consumes<double>( tag ) );
  }

  produces<double>(                   "WeightProd" );
  produces<usr::Counter, edm::InRun>( "WeightSum" );
}

WeightProdSum::~WeightProdSum(){}

/*-----------------------------------------------------------------------------
 *  Main control flow.
   --------------------------------------------------------------------------*/
void
WeightProdSum::beginRun( const edm::Run&, const edm::EventSetup& )
{
  _weightsum = 0;
}

/*----------------------------------------------------------------------------*/

void
WeightProdSum::produce( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
  if( iEvent.isRealData() ){
    _weightsum += 1;// Always ++ for data
    return;// Do not attempt to read weights
  }

  std::unique_ptr<double> weight( new double(1.0) );

  for( const auto& src : _weightsrclist ){
    // No error detection, throw exception if weight is not found.
    iEvent.getByToken( src, _weighthandle );
    *weight *= *_weighthandle;
  }

  _weightsum += *weight;

  iEvent.put( std::move(weight), "WeightProd" );
}

/*----------------------------------------------------------------------------*/

void
WeightProdSum::endRun( const edm::Run&, const edm::EventSetup& ){}

/*----------------------------------------------------------------------------*/

void
WeightProdSum::endRunProduce( edm::Run& iRun, const edm::EventSetup& )
{
  // Weight sum should be number of events for data.
  std::unique_ptr<usr::Counter> sumptr( new usr::Counter( _weightsum ) );
  iRun.put( std::move(sumptr), "WeightSum" );
}

/*----------------------------------------------------------------------------*/

DEFINE_FWK_MODULE( WeightProdSum );
