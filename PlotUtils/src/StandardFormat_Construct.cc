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

  if( CheckQuery( tree, "io settings" ) ){
    iosetting = IOSetting( tree.get_child( "io settings" ) );
  }

  if( CheckQuery( tree, "plots" ) ){
    for( const auto& subtree : tree.get_child( "plots" ) ){
      histlist.push_back( HistRequest( subtree.second ) );
    }
  }

  if( CheckQuery( tree, "signals" ) ){
    for( const auto& subtree : tree.get_child( "signals" ) ){
      signallist.push_back( Process( subtree.second, this ) );
    }
  }

  if( CheckQuery( tree, "background" ) ){
    for( const auto& subtree : tree.get_child( "background" ) ){
      background.push_back( ProcessGroup( subtree.second, this ) );
    }
  }

  if( CheckQuery( tree, "data sets" ) ){
    data = ProcessGroup( tree.get_child( "Data sets" ), this );

    for( const auto d : data ){
      _total_luminosity += d.effective_luminosity;
    }
  } else {
    // Monte Carlo only system setting total luminosity to 1 to avoid crashing
    _total_luminosity = 1 ;
  }

  if( CheckQuery( tree, "uncertainties" ) ){
    for( const auto& subtree : tree.get_child( "uncertainties" ) ){
      uncertainties.push_back( Uncertainty( subtree.second ) );
    }
  }

}

// ------------------------------------------------------------------------------

IOSetting::IOSetting(){}

IOSetting::IOSetting( const usr::pt::ptree& tree ) :
  input_prefix( GetSingleOptional<std::string>( tree, "input prefix", "" ) ),
  key_prefix( GetSingleOptional<std::string>( tree, "key prefix", "" ) ),
  output_prefix( GetSingleOptional<std::string>( tree, "output prefix", "" ) ),
  output_postfix( GetSingleOptional<std::string>( tree, "output postfix", "" ) )
{}


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

ProcessGroup::ProcessGroup( const usr::pt::ptree& tree,
                            const BatchRequest*   ptr ) :
  name( GetSingle<std::string>( tree, "display" ) ),
  latex_name( GetSingleOptional<std::string>( tree, "latex", name ) ),
  color( GetSingleOptional<std::string>( tree, "color", "#000000" ) )
{
  for( const auto& subtree : tree.get_child( "processes" ) ){
    push_back( Process( subtree.second, ptr ) );
  }
}

// ------------------------------------------------------------------------------

Process::Process( const usr::pt::ptree& tree, const BatchRequest* ptr  ) :
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
  effective_luminosity( GetSingleOptional<double>( tree, "luminosity", 1.0 ) ),
  run_range_min( CheckQuery( tree, "run range" ) ?
                 GetList<unsigned>( tree, "run range" ).at( 0 ) : 0 ),
  run_range_max( run_range_min != 0 ?
                 GetList<unsigned>( tree, "run range" ).at( 1 )  : 0 ),
  _file( nullptr ),
  parent( ptr )
{
  OpenFile();
}

std::string Process::MakeKey( const std::string& key ) const
{
  return Parent().iosetting.key_prefix + key_prefix + key;
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

void Process::OpenFile()
{
  if( _file ){ _file->Close();  }

  const int root_error_level = gErrorIgnoreLevel;

  // Suppressing function printing error for missing file.
  gErrorIgnoreLevel = kError;
  _file
    = TFile::Open( ( Parent().iosetting.input_prefix+file ).c_str()
                 , "READ" );
  gErrorIgnoreLevel = root_error_level;

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

void BatchRequest::UpdateInputPrefix( const std::string& x )
{
  iosetting.input_prefix = x;

  for( auto& signal : signallist ){
    signal.OpenFile();
  }

  for( auto& group : background ){
    for( auto& process : group ){
      process.OpenFile();
    }
  }

  for( auto& process : data  ){
    process.OpenFile();
  }
}

void BatchRequest::UpdateKeyPrefix( const std::string& x )
{
  iosetting.key_prefix = x;
}

void BatchRequest::UpdateOutputPrefix( const std::string& x )
{
  iosetting.output_prefix = x;
}

void BatchRequest::UpdateoutputPostfix( const std::string& x )
{
  iosetting.output_postfix = x;
}

// ** End of namespaces
}

}

}
