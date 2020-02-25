#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "UserUtils/EDMUtils/interface/PluginAlias.hpp"

namespace usr
{

class EDHistogramAnalyzer : public EDAnalyzer
{
public:
  EDHistogramAnalyzer( const edm::ParameterSet& );
  ~EDHistogramAnalyzer();

protected:
  edm::Service<TFileService> fs;
  std::map<std::string, TH1D*> _histmap;

  template<typename ... Args>
  inline TH1D& BookHist1D( const std::string& name, Args ... args )
  {
    _histmap[name] = fs->make<TH1D>( name.c_str(), "", args ... );
    return *( _histmap[name] );
  }

  inline TH1D& Hist( const std::string& x )
  {
    return *( _histmap[x] );
  }
};

EDHistogramAnalyzer::EDHistogramAnalyzer( const edm::ParameterSet& config  ) :
  PluginAlias( config )
{
  usesResource( "TFileService" );
}

EDHistogramAnalyzer::~EDHistogramAnalyzer()
{
}



}// namespace usr
