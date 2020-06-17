/**
 * @file StandardFormat_Construct.cc
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2020-06-15
 *
 * @copyright Copyright (c) 2020
 *
 */

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

/**
 * @brief Construct a new Batch Request object from a json file string
 *
 * @param filename
 */
BatchRequest::BatchRequest( const std::string& filename )
{
  initialize( usr::FromJSONFile( filename ) );
}

BatchRequest::BatchRequest( const std::vector<std::string>& jsonfiles )
{
  initialize( usr::FromJSONFiles( jsonfiles ) );
}

BatchRequest::BatchRequest( const usr::JSONMap& map )
{
  initialize( map );
}

void
BatchRequest::initialize( const usr::JSONMap& map )
{
  if( map.HasMember( "io settings" ) ){
    iosetting = IOSetting( map["io settings"] );
  }

  if( map.HasMember( "plots" ) ){
    for( const auto& histmap : map["plots"].GetArray() ){
      histlist.push_back( HistRequest( histmap ) );
    }
  }

  if( map.HasMember( "signals" ) ){
    for( const auto& sig_map : map["signals"].GetArray() ){
      signallist.push_back( Process( sig_map, this ) );
    }
  }

  if( map.HasMember( "background" ) ){
    for( const auto& bkg_map : map["background"].GetArray() ){
      background.push_back( ProcessGroup( bkg_map, this ) );
    }
  }

  if( map.HasMember( "data sets" ) ){
    data = ProcessGroup( map["data sets"], this );

    for( const auto d : data ){
      _total_luminosity += d.effective_luminosity;
    }
  } else {
    // Monte Carlo only system setting total luminosity to 1 to avoid crashing
    _total_luminosity = 1;
  }

  if( map.HasMember( "uncertainties" ) ){
    for( const auto& unc_map : map[ "uncertainties" ].GetArray() ){
      uncertainties.push_back( Uncertainty( unc_map ) );
    }
  }

}

// ------------------------------------------------------------------------------

IOSetting::IOSetting(){}

IOSetting::IOSetting( const usr::JSONMap& map )
{
  input_prefix   = JSONEntry<std::string>( map, "input prefix", "" );
  key_prefix     = JSONEntry<std::string>( map, "key prefix", "" );
  output_prefix  = JSONEntry<std::string>( map, "output prefix", "" );
  output_postfix = JSONEntry<std::string>( map, "output postfix", "" );
}


// ------------------------------------------------------------------------------

Uncertainty::Uncertainty( const usr::JSONMap& map )
{
  name             = JSONEntry<std::string>( map, "name" );
  key              = JSONEntry<std::string>( map, "key name", "" );
  norm_uncertainty = JSONEntry<Measurement>( map, "norm uncertainty"
                                           , Measurement( 1, 0, 0 ) );
  if( norm_uncertainty.CentralValue() == 1 &&
      norm_uncertainty.AbsUpperError() == 0 &&
      norm_uncertainty.AbsLowerError() == 0 &&
      name != "Cross Section" &&
      key == "" ){
    key = name;
  }
}


// ------------------------------------------------------------------------------

HistRequest::HistRequest( const usr::JSONMap& map )
{
  name    = JSONEntry<std::string>( map, "name" );
  xaxis   = JSONEntry<std::string>( map, "xaxis", "" );
  units   = JSONEntry<std::string>( map, "units", "" );
  yaxis   = JSONEntry<std::string>( map, "yaxis", "Events" );
  filekey = JSONEntry<std::string>( map, "filekey", name );
  type    = JSONEntry<std::string>( map, "type", "ratio" );
  logy    = JSONEntry<bool>( map, "logy", false );

}

// ------------------------------------------------------------------------------

ProcessGroup::ProcessGroup(){}

ProcessGroup::ProcessGroup( const usr::JSONMap& map,
                            const BatchRequest* ptr )
{
  name       = JSONEntry<std::string>( map, "display" );
  latex_name = JSONEntry<std::string>( map, "latex",  name );
  color      = JSONEntry<std::string>( map, "color", "black" );

  for( const auto& submap : map[ "processes" ].GetArray() ){
    push_back( Process( submap, ptr ) );
  }
}

// ------------------------------------------------------------------------------

/**
 * @brief Constructing a process object using a parsed file.
 */
Process::Process( const usr::JSONMap& map, const BatchRequest* ptr  )
{
  name                 = map["display"].GetString();
  latex_name           = JSONEntry<std::string>( map, "latex", name );
  generator            = JSONEntry<std::string>( map, "generator", "" );
  cross_section_source = JSONEntry<std::string>( map, "cross section source", "" );

  file                 = JSONEntry<std::string>( map, "file" );
  color                = JSONEntry<std::string>( map, "color", "blue" );
  key_prefix           = JSONEntry<std::string>( map, "key prefix", "" );
  scale                = JSONEntry<double>( map, "scale", 1.0 );
  effective_luminosity = JSONEntry<double>( map, "luminosity", 0.0 );
  cross_section        = JSONEntry<Measurement>( map, "cross section"
                                               , usr::Measurement( 1, 0, 0 ) );

  // "run range"
  const auto run_range = JSONList<double>( map, "run range", {0, 0} );
  run_range_min = run_range.size() == 2 ? run_range[0] : 0;
  run_range_max = run_range.size() == 2 ? run_range[1] : 0;

  transparency = usr::JSONEntry<double>( map, "transparency", 1.0 );
  _file        = nullptr;
  parent       = ptr;

  OpenFile();
}

std::string
Process::MakeKey( const std::string& key ) const
{
  return Parent().iosetting.key_prefix + key_prefix + key;
}

bool
Process::CheckKey( const std::string& key ) const
{
  return _file->Get( MakeKey( key ).c_str() );
}

TH1D*
Process::GetClone( const std::string& key ) const
{
  _file->cd( 0 );
  TH1D* ans = (TH1D*)( _file->Get( MakeKey( key ).c_str() )->Clone() );
  ans->SetDirectory( 0 );
  return ans;
}

TH1D*
Process::GetNormalizedClone( const std::string& key ) const
{
  TH1D* ans = GetClone( key );
  ans->Scale( 1.0 / ans->Integral() );
  return ans;
}

TH1D*
Process::GetScaledClone( const std::string& key, const double total ) const
{
  TH1D* ans = GetClone( key );
  ans->Scale( total / effective_luminosity );
  ans->Scale( scale );

  return ans;
}

TH2D*
Process::Get2DClone( const std::string& key ) const
{
  _file->cd( 0 );
  TH2D* ans = (TH2D*)( _file->Get( MakeKey( key ).c_str() )->Clone() );
  ans->SetDirectory( 0 );
  return ans;
}

void
Process::OpenFile()
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
    auto DefaultLumi = [this](){
                         usr::fout( "Warning! Effective luminosity is found for "
                           "this process, defaulting the 1.0pb, If the "
                           "normalization is wrong, fix this issue\n" );
                         this->effective_luminosity = 1.0;
                       };
    if( _file == nullptr ){
      DefaultLumi();
      return;
    }
    TTree* lumi_tree = (TTree*)_file->Get( "Count" );
    if( lumi_tree == nullptr ){
      DefaultLumi();
      return;
    }
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

void
BatchRequest::UpdateInputPrefix( const std::string& x )
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

void
BatchRequest::UpdateKeyPrefix( const std::string& x )
{
  iosetting.key_prefix = x;
}

void
BatchRequest::UpdateOutputPrefix( const std::string& x )
{
  iosetting.output_prefix = x;
}

void
BatchRequest::UpdateoutputPostfix( const std::string& x )
{
  iosetting.output_postfix = x;
}

// ** End of namespaces
}

}

}
