#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "UserUtils/EDMUtils/interface/PluginAlias.hpp"

#include "TH1D.h"
#include "TH2D.h"

namespace usr
{

class EDHistogramAnalyzer : public EDAnalyzer
{
public:
  EDHistogramAnalyzer( const edm::ParameterSet& );
  ~EDHistogramAnalyzer();

protected:
  edm::Service<TFileService>   fs;
  std::map<std::string, TH1D*> _histmap;
  std::map<std::string, TH2D*> _hist2dmap;

  template<typename ... Args>
  inline TH1D&
  BookHist1D( const std::string& name, Args ... args )
  {
    _histmap[name] = fs->make<TH1D>( name.c_str(), "", args ... );
    return *( _histmap[name] );
  }


  template<typename ... Args>
  inline TH2D&
  BookHist2D( const std::string& name, Args ... args )
  {
    _hist2dmap[name] = fs->make<TH2D>( name.c_str(), "", args ... );
    return *( _hist2dmap[name] );
  }


  inline TH1D&
  Hist( const std::string& x )
  {
    return *( _histmap[x] );
  }


  inline TH2D&
  Hist2D( const std::string& x )
  {
    return *( _hist2dmap[x] );
  }


};

}// namespace usr
