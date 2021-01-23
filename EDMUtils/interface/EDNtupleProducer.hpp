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
  TTree* tree;

  std::map<std::string, std::unique_ptr<std::vector<float> > > _float_collection;
  std::map<std::string, std::unique_ptr<std::vector<int> > > _int_collection;
  std::map<std::string, std::unique_ptr<std::vector<bool> > > _bool_collection;

  template<typename>
  void AddCollection( const std::string& );

  template<typename T>
  std::vector<T>& Col( const std::string& );

  //

private:
  void analyze( const edm::Event&, const edm::EventSetup& ) final;
  virtual void np_analyze( const edm::Event&, const edm::EventSetup& ) = 0;
  void clear_collections();
};


}// namespace usr
