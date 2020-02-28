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
  if( CheckQuery( tree, "Plot List" ) ){
    for( const auto& subtree : tree.get_child( "Plot List" ) ){
      histlist.push_back( HistRequest( subtree.second ) );
    }
  }

  if( CheckQuery( tree, "Signal Processes" ) ){
    for( const auto& subtree : tree.get_child( "Signal Processes" ) ){
      signallist.push_back( Process( subtree.second ) );
    }
  }

  if( CheckQuery( tree, "Background Processes" ) ){
    for( const auto& subtree : tree.get_child( "Background Processes" ) ){
      background.push_back( ProcessGroup( subtree.second ) );
    }
  }

  if( CheckQuery( tree, "Data sets" ) ){
    data = ProcessGroup( tree.get_child( "Data sets" ) );

    for( const auto d : data ){
      _total_luminosity += d.effective_luminosity;
    }
  }

  if( CheckQuery( tree, "Uncertainties" ) ){
    for( const auto& subtree : tree.get_child( "Uncertainties" ) ){
      uncertainties.push_back( Uncertainty( subtree.second ) );
    }
  }

}

// ------------------------------------------------------------------------------

Uncertainty::Uncertainty( const usr::pt::ptree& tree ) :
  name( GetSingle<std::string>( tree, "name" ) ),
  key( GetSingleOptional<std::string>( tree, "Key name", "" ) ),
  norm_uncertainty( usr::Measurement( 1, 0, 0 ) )
{
  if( CheckQuery( tree, "Norm uncertainty" ) ){
    std::vector<double> unc = GetList<double>( tree, "Norm uncertainty" );
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
  logy( GetSingleOptional<bool>( tree, "logy", true ) )
{}

// ------------------------------------------------------------------------------

ProcessGroup::ProcessGroup(){}

ProcessGroup::ProcessGroup( const usr::pt::ptree& tree ) :
  name( GetSingle<std::string>( tree, "Display" ) ),
  latex_name( GetSingleOptional<std::string>( tree, "Latex", name ) ),
  color( GetSingleOptional<std::string>( tree, "Color", "#000000" ) )
{
  for( const auto& subtree : tree.get_child( "processes" ) ){
    push_back( Process( subtree.second ) );
  }
}

// ------------------------------------------------------------------------------

Process::Process( const usr::pt::ptree& tree ) :
  name( GetSingle<std::string>( tree, "Display" ) ),
  latex_name( GetSingleOptional<std::string>( tree, "Latex", "" ) ),
  generator( GetSingleOptional<std::string>( tree, "Generator", "" ) ),
  cross_section_source( GetSingleOptional<std::string>( tree,
    "Cross Section Source", "" ) ),
  cross_section( CheckQuery( tree, "Cross Section" ) ?
                 GetSingle<usr::Measurement>( tree, "Cross Section" ) :
                 usr::Measurement( 1, 0, 0 ) ),
  file( GetSingle<std::string>( tree, "File" ) ),
  scale( GetSingleOptional( tree, "Scale", 1.0 ) ),
  effective_luminosity( GetSingleOptional<double>( tree, "Luminosity", 0 ) ),
  run_range_min( CheckQuery( tree, "Run range" ) ?
                 GetList<unsigned>( tree, "Run range" ).at( 0 ) : 0 ),
  run_range_max( run_range_min != 0 ?
                 GetList<unsigned>( tree, "Run range" ).at( 1 )  : 0 )
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

bool Process::CheckKey( const std::string& key ) const
{
  return _file->FindKey( key.c_str() );
}

TH1D* Process::GetClone( const std::string& key ) const
{
  _file->cd( 0 );
  TH1D* ans = (TH1D*)( _file->Get( key.c_str() )->Clone() );
  ans->SetDirectory( 0 );
  return ans;
}

TH1D* Process::GetScaledClone( const std::string& key, const double total ) const
{
  TH1D* ans = GetClone( key );
  ans->Scale( total / effective_luminosity );
  ans->Scale( scale );

  return ans;
}

}

}

}
