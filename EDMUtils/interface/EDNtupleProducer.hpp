#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "UserUtils/EDMUtils/interface/PluginAlias.hpp"

#include <memory>
#include <vector>

#include "TTree.h"

namespace usr
{

class EDNtupleProducer : public usr::EDAnalyzer
{
public:
  EDNtupleProducer( const edm::ParameterSet&, const std::string& treename );
  ~EDNtupleProducer();

protected:
  edm::Service<TFileService> fs;
  TTree*                     tree;

  std::map<std::string,
           std::unique_ptr<std::vector<float> > >             _float_collection;
  std::map<std::string, std::unique_ptr<std::vector<int> > >  _int_collection;
  std::map<std::string, std::unique_ptr<std::vector<bool> > > _bool_collection;

  std::map<std::string, std::unique_ptr<float> > _float_map;
  std::map<std::string, std::unique_ptr<int>  >  _int_map;
  std::map<std::string, std::unique_ptr<bool> >  _bool_map;

  template<typename>
  void AddCollection( const std::string& );

  template<typename T>
  std::vector<T>& Col( const std::string& );

  template<typename>
  void AddValue( const std::string& );

  template<typename T>
  void SetValue( const std::string&, const T );

private:
  void         analyze( const edm::Event&, const edm::EventSetup& ) final;
  virtual bool np_analyze( const edm::Event&, const edm::EventSetup& ) = 0;
  void         clear_collections();
};

}// namespace usr
