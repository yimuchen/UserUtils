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
#include <algorithm>

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
    usr::ExceptJSONObj( map, "io settings" );
    iosetting = IOSetting( map["io settings"] );
  }

  if( map.HasMember( "plots" ) ){
    usr::ExceptJSONList( map, "plots" );

    for( const auto& histmap : map["plots"].GetArray() ){
      histlist.push_back( HistRequest( histmap ) );
    }
  }

  if( map.HasMember( "signals" ) ){
//    usr::ExceptJSONList( map, "signals" );

    for( const auto& sig_map : map["signals"].GetArray() ){
      signallist.push_back( Process( sig_map, this ) );
    }
  }

  if( map.HasMember( "background" ) ){
    usr::ExceptJSONList( map, "background" );

    for( const auto& bkg_map : map["background"].GetArray() ){
      background.push_back( ProcessGroup( bkg_map, this ) );
    }
  }

  if( map.HasMember( "data sets" ) ){
    usr::ExceptJSONObj( map, "data sets" );
    data = ProcessGroup( map["data sets"], this );

    for( const auto& d : data ){
      _total_luminosity += d.effective_luminosity;
    }
  } else {
    // Monte Carlo only system setting total luminosity to 1 to avoid crashing
    _total_luminosity = 1;
  }

  if( map.HasMember( "uncertainties" ) ){
    usr::ExceptJSONList( map, "uncertainties" );

    for( const auto& unc_map : map[ "uncertainties" ].GetArray() ){
      uncertainties.push_back( Uncertainty( unc_map ) );
    }
  }

}

/**
 * @class usr::plt::fmt::IOSetting
 * @details
 *
 * For the declaring common io settings, the following options are possible. None
 * of the entires are mandatory, all entries will default to an empty string if
 * not specified.
 *
 * - input prefix: The input prefix to add to all files. Think the working path
 *   to where the files are stored.
 * - key prefix: A common prefix to add to all histogram keys. Think the
 *   TDirectory used for storing histogram files (like those generated in
 *   TFileServices in CMSSW.)
 * - output prefix: Output prefix to save generated plot files. Think specifying
 *   a path to where the outputs should be passed.
 * - output postfix: Output postfix for the plot files before the file extension.
 */
IOSetting::IOSetting(){}

IOSetting::IOSetting( const usr::JSONMap& map )
{
  input_prefix   = JSONEntry<std::string>( map, "input prefix",   "" );
  key_prefix     = JSONEntry<std::string>( map, "key prefix",     "" );
  output_prefix  = JSONEntry<std::string>( map, "output prefix",  "" );
  output_postfix = JSONEntry<std::string>( map, "output postfix", "" );
}

/**
 * @class usr::plt:fmt::Uncertainty
 * @details
 *
 * For declaring of uncertainty sources, the following options are available.
 * Upright entires would be mandatory.
 *
 * - name: The name of the uncertainty source, required as a unique tagger.
 * - *norm uncertainty*: For normalization only uncertainty, we can specify a how
 *   the obtain the scale  up and  scale down histograms using a usr::Measurement
 *   declaration (an array like [1,0.025,0.026]), the relative uncertain will be
 *   used for scaling the central histogram up and down.
 * - *key name*: By default the uncertainty is assumed to be a shape like
 *   uncertainty, with uncertainty "up" and "down" shapes stored as shifted
 *   histograms. The effects of shape distortion is performed bin-by-bn using the
 *   histograms is done bin by bin. The key name is then used to specify which
 *   histograms in the same file should be used as the shifted templates. For
 *   each histogram request, the `histogram name + key name + "Up"` and
 *   `histogram name + key name + "Down"` will be taken as the shifted templates.
 *   In case the templates are not found, the central template will be used
 *   (which is equivalent to saying this process is not affected by the specified
 *   uncertainty). In the case that the key name is not specified, and the norm
 *   uncertainty is not specifed, the key name default to the "name" entry in the
 *   decalration.
 *
 * A special uncertainty source should be noted here, and that is the "cross
 * section" uncertainty, since uncertainties on the process cross sections are
 * more normalization uncertainties that are process specific. To activate the
 * treatment of the cross section uncertainties, simply add a uncertainty source
 * with the name "cross section" and nothing else.
 */
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

/**
 * @class usr::plt::fmt::HistRequest
 * @details
 *
 * For a histogram request entry, the following options are available. Upright
 * entires are mandatory.
 *
 * - name: The name of the generated histogram plot results.
 * - *xaxis*: The title of the x axis. Leaving blank if not specified.
 * - *yaxis*: The title of the y axis. Defaults to "Events" is not specified.
 * - *units*: The units of the x axis. Leaving blank if not specified. Notice
 *   that this entry will also affect the y axis display title (see the
 *   Pad1D::SetHistAxisTitle for more details).
 * - *filekey*: The histogram "key" to get the histogram for each processes.
 * - *type*: What to display in the secondary pad: Allowed values:
 *   - "simple": No secondary pad.
 *   - "ratio" (default): The ratio between the data and the background is
 *     displayed.
 *   - "pull" : The pull of the data vs the background uncertainty is displayed.
 * - *logy*: Whether or not to display the y axis in the main pad in log scale.
 *   Default to false.
 */
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

/**
 * @class usr::plt::fmt::ProcessGroup
 * @details
 *
 * For defining a group of processes, the following options are available.
 * Upright entries are mandatory.
 *
 * - display: The string to display for the process group in the plots,
 *   accepts ROOT flavored Latex string.
 * - *latex*: The string to display for generation of latex tables, notice that
 *   backslashes will need to be doubled up for "literal backslashes". If not
 *   specificed, it will assume the same value of "display".
 * - *color*: The color to display plots for this particular group. The string
 *   can either be a standard hex code ("#012ABC"), or any color name listed in
 *   the `usr::plt::col` namespace ("black", "midnight"...etc). Defaults to
 *   "black" is not otherwise specified.
 * - processes: An array of processes declaration. See
 *   usr::plt::fmt::Process for a complete statement of their options.
 */


ProcessGroup::ProcessGroup(){}

/**
 * @brief Construction only possible through parent BatchProcess instance.
 */
ProcessGroup::ProcessGroup( const usr::JSONMap& map,
                            const BatchRequest* ptr )
{
  name       = JSONEntry<std::string>( map, "display" );
  latex_name = JSONEntry<std::string>( map, "latex",  name );
  color      = JSONEntry<std::string>( map, "color", "black" );

  usr::ExceptJSONList( map, "processes" );

  for( const auto& submap : map[ "processes" ].GetArray() ){
    push_back( Process( submap, ptr ) );
  }
}

/**
 * @class usr::plt::fmt::Process
 * @details
 *
 * For defining a process, the following options ar available. Upright entires
 * are mandatory.
 *
 * - display: The string to display for the process in the plots. Access ROOT
 *   flavored latex string.
 * - file: The file containing the relevent histogram files. Notice that the path
 *   that will be open a process histogram file would have the prefix added in
 *   the "io settings" of the parent BatchProcess instance.
 * - "key prefix": Prefix string needed to add to histogram keys to get a
 *   relevent histogram. Helpful if a histogram file contains histograms for
 *   multiple processes.
 * - *latex*: The string to display for latex tables Notice that backslashed will
 *   need to be doubled up for "literal backslashed". If not specified, it will
 *   assume the same value of "display".
 * - *cross section*: The cross section with uncertainty of the processes in
 *   units of \f$\text{pb}\f$s. The declaration is done in the form of a array
 *   with at least one number and at most three numbers. The first is the central
 *   value, with the latter two being the shift up and shift down uncertainties.
 *   Defaults to \f$1\pm0\text{pb}\f$.
 * - *generator*: The generator used for creating the process files. Default to
 *   an empty string.
 * - *cross section source*: How the cross section was calculated. Defaults to an
 *   empty string.
 * - *scale*: Additional factor to scale the process for presentation aesthetics.
 *   Default to 1.0.
 * - *luminosity*: The luminosity of the process (for data), or the effective
 *   luminosity of the unfiltered process (for simulated events), in units of
 *   \f$\text{pb}^{-1}\f$. For data processes, this must be present to allow for
 *   the calculation of the total luminosity. For simulated processes, if this
 *   entry is omitted, then the program will look for a TTree named "Count" and a
 *   leaf named "OriginalCount" to get the total number of events in the
 *   unfiltered process, the effective luminosity is then calculated from the
 *   original count and the given cross section. If the TTree entry doesn't
 *   exists, then the effective luminosity defaults to 1.0, which is helpful if
 *   we are only distribution comparison.
 * - *run range*: The begin and end of the run range of the data collected. This
 *   is only used for table generation. Defaults to [0,0].
 * - *color*: Color to display for the process. The string can either be a color
 *   hes string ("#012ABC"), or any of the color found in the usr::plt::col name
 *   space  ("black", "midnight"...etc), defaults to "blue" if not specified.
 * - *transparency*, to color transparency of the process.
 */
/**
 * @brief Construnction only possible through parent BatchProcess instance.
 */
Process::Process( const usr::JSONMap& map, const BatchRequest* ptr  )
{
  name                 = map["display"].GetString();
  latex_name           = JSONEntry<std::string>( map, "latex", name );
  generator            = JSONEntry<std::string>( map, "generator", "" );
  cross_section_source = JSONEntry<std::string>( map, "cross section source"
                                               , "" );

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

/**
 * @brief Generating the key from a histogram request string
 *
 * The final key will be the "io settings" key prefix + the process key prefix +
 * the requested key.
 */
std::string
Process::MakeKey( const std::string& key ) const
{
  return Parent().iosetting.key_prefix + key_prefix + key;
}

/**
 * @brief Checking if the key corresponding to a histogram request exists.
 *
 * We will not be checking for object types here.
 */
bool
Process::CheckKey( const std::string& key ) const
{
  return _file->Get( MakeKey( key ).c_str() );
}

/**
 * @brief Getting a cloned copy of the histogram object of the histogram request
 * string.
 *
 * It would be here that we attempt to check the type of the object, using the
 * results of the dynamic_cast function for pointers. A failed dynamic cast will
 * result in a `nullptr`.
 */
TH1D*
Process::GetClone( const std::string& key ) const
{
  _file->cd( 0 );
  TH1D* ans = dynamic_cast<TH1D*>(
    _file->Get( MakeKey( key ).c_str() )->Clone() );
  ans->SetDirectory( 0 );
  return ans;
}

/**
 * @brief Getting a normalized copy of the histogram object of the histogram
 * request string.
 *
 * Basically the a scaled version of the GetClone function, where the returned
 * histogram is normalized to an integral of 1. This function is mainly used for
 * distribution comparison.
 */
TH1D*
Process::GetNormalizedClone( const std::string& key ) const
{
  TH1D* ans = GetClone( key );
  ans->Scale( 1.0 / ans->Integral() );
  return ans;
}

/**
 * @brief Getting a correctly scaled copy of the histogram object of the
 * histogram request string.
 *
 * The second parameter indicates the total luminosity the process should be
 * scaled to. Additional scale factors from the process definition will be
 * applied.
 */
TH1D*
Process::GetScaledClone( const std::string& key, const double total ) const
{
  TH1D* ans = GetClone( key );
  ans->Scale( total / effective_luminosity );
  ans->Scale( scale );

  return ans;
}

/**
 * @brief Getting a clone of a 2D histogram of the histogram object of a
 * histogram request string.
 *
 * Essentially the same as the GetClone function, except this time casting to a
 * TH2D pointer instead.
 */
TH2D*
Process::Get2DClone( const std::string& key ) const
{
  _file->cd( 0 );
  TH2D* ans = dynamic_cast<TH2D*>(
    _file->Get( MakeKey( key ).c_str() )->Clone() );
  ans->SetDirectory( 0 );
  return ans;
}

/**
 * @brief Opening the histogram file of a process.
 *
 * This also handles the updating of the effective luminosity, by looking into
 * the file contents to see if corresponding TTree leaves exists.
 */
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


  if( effective_luminosity == 0.0 || effective_luminosity == 1.0 ){
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

/**
 * @brief Updating the stored io settings instance.
 *
 * We will also need to re-initialize files of each process in the BatchRequest.
 */
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

/**
 * @brief Updating the stored io settings instance.
 *
 */
void
BatchRequest::UpdateKeyPrefix( const std::string& x )
{
  iosetting.key_prefix = x;
}

/**
 * @brief Updating the stored io settings instance.
 *
 */
void
BatchRequest::UpdateOutputPrefix( const std::string& x )
{
  iosetting.output_prefix = x;
}

/**
 * @brief Updating the stored io settings instance.
 *
 */
void
BatchRequest::UpdateoutputPostfix( const std::string& x )
{
  iosetting.output_postfix = x;
}

// ** End of namespaces
}

}

}
