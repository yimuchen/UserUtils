#include "UserUtils/EDMUtils/interface/EDNtupleProducer.hpp"

namespace usr
{

EDNtupleProducer::EDNtupleProducer( const edm::ParameterSet& pset,
                                    const std::string&       treename )
  : PluginAlias( pset )
{
  usesResource( "TFileService" );
  tree = fs->make<TTree>( treename.c_str(), treename.c_str() );
}

EDNtupleProducer::~EDNtupleProducer()
{}

template<>
void
EDNtupleProducer::AddCollection<float>( const std::string& name )
{
  _float_collection[name] = std::make_unique<std::vector<float> >();
  tree->Branch( name.c_str(), _float_collection[name].get() );
}

template<>
void
EDNtupleProducer::AddCollection<int>( const std::string& name )
{
  _int_collection[name] = std::make_unique<std::vector<int> >();
  tree->Branch( name.c_str(), _int_collection[name].get() );
}

template<>
void
EDNtupleProducer::AddCollection<bool>( const std::string& name )
{
  _bool_collection[name] = std::make_unique<std::vector<bool> >();
  tree->Branch( name.c_str(), _bool_collection[name].get() );
}

template<>
std::vector<float>&
EDNtupleProducer::Col<float>( const std::string& name )
{
  return *_float_collection[name];
}

template<>
std::vector<int>&
EDNtupleProducer::Col<int>( const std::string& name )
{
  return *_int_collection[name];
}

template<>
std::vector<bool>&
EDNtupleProducer::Col<bool>( const std::string& name )
{
  return *_bool_collection[name];
}

void
EDNtupleProducer::analyze( const edm::Event& i, const edm::EventSetup& j )
{
  clear_collections();
  np_analyze( i, j );
  tree->Fill();
};

void
EDNtupleProducer::clear_collections()
{
  for( auto& p : _float_collection ){
    p.second->clear();
  }

  for( auto& p : _int_collection ){
    p.second->clear();
  }

  for( auto& p : _bool_collection ){
    p.second->clear();
  }
}



}
