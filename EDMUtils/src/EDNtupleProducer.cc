#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"
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
  if( !_float_collection.count( name )){
    throw std::invalid_argument( usr::fstr( "No float collection [%s]",
                                            name ).c_str() );
  }
  return *_float_collection[name];
}


template<>
std::vector<int>&
EDNtupleProducer::Col<int>( const std::string& name )
{
  if( !_int_collection.count( name ) ){
    throw std::invalid_argument( usr::fstr( "No int collection [%s]",
                                            name ).c_str() );
  }
  return *_int_collection[name];
}


template<>
std::vector<bool>&
EDNtupleProducer::Col<bool>( const std::string& name )
{
  if( !_bool_collection.count( name ) ){
    throw std::invalid_argument( usr::fstr( "No bool collection [%s]",
                                            name ).c_str());
  }
  return *_bool_collection[name];
}


template<>
void
EDNtupleProducer::AddValue<float>( const std::string& name )
{
  _float_map[name] = std::make_unique<float>();
  tree->Branch( name.c_str(), _float_map[name].get() );
}


template<>
void
EDNtupleProducer::AddValue<int>( const std::string& name )
{
  _int_map[name] = std::make_unique<int>();
  tree->Branch( name.c_str(), _int_map[name].get() );
}


template<>
void
EDNtupleProducer::AddValue<bool>( const std::string& name )
{
  _bool_map[name] = std::make_unique<bool>();
  tree->Branch( name.c_str(), _bool_map[name].get() );
}


template<>
void
EDNtupleProducer::SetValue<>( const std::string& name, const double x )
{
  *_float_map[name] = x;
}


template<>
void
EDNtupleProducer::SetValue<>( const std::string& name, const float x )
{
  *_float_map[name] = x;
}


template<>
void
EDNtupleProducer::SetValue<>( const std::string& name, const int x  )
{
  *_int_map[name] = x;
}


template<>
void
EDNtupleProducer::SetValue<>( const std::string& name, const bool x )
{
  *_bool_map[name] = x;
}


void
EDNtupleProducer::analyze( const edm::Event& i, const edm::EventSetup& j )
{
  clear_collections();
  np_analyze( i, j );
  tree->Fill();
}


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
