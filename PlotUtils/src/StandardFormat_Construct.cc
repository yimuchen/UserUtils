#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/STLUtils/OStreamUtils.hpp"
#include "UserUtils/MathUtils/interface/Measurement/Format.hpp"
#include "UserUtils/PlotUtils/interface/StandardPlotFormat.hpp"
#else
#include "UserUtils/Common/STLUtils/OStreamUtils.hpp"
#include "UserUtils/MathUtils/Measurement/Format.hpp"
#include "UserUtils/PlotUtils/StandardPlotFormat.hpp"
#endif

#include "TTree.h"


namespace usr {

namespace plt {

namespace fmt {

BatchRequest::BatchRequest( const std::string& filename )
{
  initialize( usr::FromJsonFile( filename ) );
}

BatchRequest::BatchRequest( const usr::pt::ptree& tree )
{
  initialize( tree );
}

void BatchRequest::initialize( const usr::pt::ptree& tree )
{
  if( CheckQuery( tree, "plots" ) ){
    for( const auto& subtree : tree.get_child( "plots" ) ){
      histlist.push_back( HistRequest( subtree.second ) );
    }
  }

  if( CheckQuery( tree, "signals" ) ){
    for( const auto& subtree : tree.get_child( "signals" ) ){
      signallist.push_back( Process( subtree.second ) );
    }
  }

  if( CheckQuery( tree, "background" ) ){
    for( const auto& subtree : tree.get_child( "background" ) ){
      background.push_back( ProcessGroup( subtree.second ) );
    }
  }

  if( CheckQuery( tree, "data sets" ) ){
    data = ProcessGroup( tree.get_child( "Data sets" ) );

    for( const auto d : data ){
      _total_luminosity += d.effective_luminosity;
    }
  }

  if( CheckQuery( tree, "uncertainties" ) ){
    for( const auto& subtree : tree.get_child( "uncertainties" ) ){
      uncertainties.push_back( Uncertainty( subtree.second ) );
    }
  }

}

// ------------------------------------------------------------------------------

Uncertainty::Uncertainty( const usr::pt::ptree& tree ) :
  name( GetSingle<std::string>( tree, "name" ) ),
  key( GetSingleOptional<std::string>( tree, "key name", "" ) ),
  norm_uncertainty( usr::Measurement( 1, 0, 0 ) )
{
  if( CheckQuery( tree, "norm uncertainty" ) ){
    std::vector<double> unc = GetList<double>( tree, "norm uncertainty" );
    if( unc.size() == 0 ){
      usr::fout( "Normalization uncertainty in wrong format! Igoring\n" );
    } else if( unc.size() == 1 ){
      norm_uncertainty = usr::Measurement( 1, unc[0], unc[0] );
    } else if( unc.size() == 2 ){
      norm_uncertainty = usr::Measurement( 1, unc[0], unc[1] );
    } else {
      usr::fout( "Normalization uncertainty in wrong format! "
        "Igoring extra entries \n" );
      norm_uncertainty = usr::Measurement( 1, unc[0], unc[1] );
    }
  }

  if( norm_uncertainty.CentralValue() == 1 &&
      norm_uncertainty.AbsUpperError() == 0 &&
      norm_uncertainty.AbsLowerError() == 0 &&
      name != "Cross Section" &&
      key == "" ){
    key = name;
  }
}


// ------------------------------------------------------------------------------

HistRequest::HistRequest( const usr::pt::ptree& tree ) :
  name( GetSingle<std::string>( tree, "name" ) ),
  xaxis( GetSingleOptional<std::string>( tree, "xaxis", "" ) ),
  units( GetSingleOptional<std::string>( tree, "units", "" ) ),
  yaxis( GetSingleOptional<std::string>( tree, "yaxis", "Events" ) ),
  filekey( GetSingleOptional<std::string>( tree, "filekey", name ) ),
  type( GetSingleOptional<std::string>( tree, "type", "ratio" ) ),
  logy( GetSingleOptional<bool>( tree, "logy", false ) )
{}

// ------------------------------------------------------------------------------

ProcessGroup::ProcessGroup(){}

ProcessGroup::ProcessGroup( const usr::pt::ptree& tree ) :
  name( GetSingle<std::string>( tree, "display" ) ),
  latex_name( GetSingleOptional<std::string>( tree, "latex", name ) ),
  color( GetSingleOptional<std::string>( tree, "color", "#000000" ) )
{
  for( const auto& subtree : tree.get_child( "processes" ) ){
    push_back( Process( subtree.second ) );
  }
}

// ------------------------------------------------------------------------------

Process::Process( const usr::pt::ptree& tree ) :
  name( GetSingle<std::string>( tree, "display" ) ),
  latex_name( GetSingleOptional<std::string>( tree, "latex", "" ) ),
  generator( GetSingleOptional<std::string>( tree, "generator", "" ) ),
  cross_section_source( GetSingleOptional<std::string>( tree,
    "cross section source", "" ) ),
  cross_section( CheckQuery( tree, "cross section" ) ?
                 GetSingle<usr::Measurement>( tree, "cross section" ) :
                 usr::Measurement( 1, 0, 0 ) ),
  file( GetSingle<std::string>( tree, "file" ) ),
  color( GetSingleOptional<std::string>( tree, "color", "#0000FF" ) ),
  key_prefix( GetSingleOptional<std::string>( tree, "key prefix", "" ) ),
  scale( GetSingleOptional( tree, "scale", 1.0 ) ),
  effective_luminosity( GetSingleOptional<double>( tree, "luminosity", 0 ) ),
  run_range_min( CheckQuery( tree, "run range" ) ?
                 GetList<unsigned>( tree, "run range" ).at( 0 ) : 0 ),
  run_range_max( run_range_min != 0 ?
                 GetList<unsigned>( tree, "run range" ).at( 1 )  : 0 )
{
  _file = TFile::Open( file.c_str(), "READ" );

  if( effective_luminosity == 0 ){
    if( _file == nullptr ){ return; }
    TTree* lumi_tree = (TTree*)_file->Get( "Count" );
    if( lumi_tree == nullptr ){ return; }
    double original_events;
    lumi_tree->SetBranchAddress( "OriginalEvents", &original_events );
    lumi_tree->GetEntry( 0 );
    effective_luminosity = original_events / cross_section.CentralValue();

    if( run_range_max != 0 || run_range_min != 0 ){
      usr::fout( "Warning! Run range should only be specified for data events! "
        "Make sure you know what you are doing\n" );
    }
  } else {
    if( cross_section.CentralValue() != 1 || !cross_section_source.empty()
        || scale != 1 ){
      usr::fout( "Warning! Cross section and scaling options should only be used"
        "for simulated events, make sure you know what you are doing\n" );
    }
  }
}

std::string Process::MakeKey( const std::string& key ) const
{
  return key_prefix + key;
}

bool Process::CheckKey( const std::string& key ) const
{
  return _file->Get( MakeKey( key ).c_str() );
}

TH1D* Process::GetClone( const std::string& key ) const
{
  _file->cd( 0 );
  TH1D* ans = (TH1D*)( _file->Get( MakeKey( key ).c_str() )->Clone() );
  ans->SetDirectory( 0 );
  return ans;
}

TH1D* Process::GetNormalizedClone( const std::string& key ) const
{
  TH1D* ans = GetClone( key );
  ans->Scale( 1.0 / ans->Integral() );
  return ans;
}

TH1D* Process::GetScaledClone( const std::string& key, const double total ) const
{
  TH1D* ans = GetClone( key );
  ans->Scale( total / effective_luminosity );
  ans->Scale( scale );

  return ans;
}


// ** End of namespaces
}

}

}
