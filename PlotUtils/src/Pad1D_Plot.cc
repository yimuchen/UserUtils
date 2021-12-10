/**
 * @file    Pad1D_Plot.cc
 * @brief   Implementing the data plotting functions.
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/Maths.hpp"
#include "UserUtils/Common/interface/STLUtils/OStreamUtils.hpp"
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"
#include "UserUtils/MathUtils/interface/Miscellaneous.hpp"
#include "UserUtils/PlotUtils/interface/Pad1D.hpp"
#else
#include "UserUtils/Common/Maths.hpp"
#include "UserUtils/Common/STLUtils/OStreamUtils.hpp"
#include "UserUtils/Common/STLUtils/StringUtils.hpp"
#include "UserUtils/MathUtils/Miscellaneous.hpp"
#include "UserUtils/PlotUtils/Pad1D.hpp"
#endif

#include <limits>
#include <random>

#include "CmdSetAttr.hpp"

#include "Math/SpecFunc.h"
#include "TDecompChol.h"
#include "TFitResult.h"
#include "TGraphErrors.h"
#include "TList.h"
#include "TMatrixDEigen.h"
#include "TStyle.h"

// static variables for new object generation
static const std::string genaxisname = "axishist";

namespace usr
{

namespace plt
{

/**
 * @details
 * Plotting 1D histograms accepts these options:
 * - PlotType: Defining how the binned data should be represented on the pad,
 *   following types are supported
 *   - plottype::hist (default) - standard square curve/block diagram for
 *     displaying a diagram, equivalent to the `"HIST"` options provided in the
 *     TH1 object.
 *   - plottype::scatter - Plotting points with error bars. Complying to the CMS
 *     plotting convention with the horizontal error bar being suppressed for
 *     fixed bin-width data.
 *   - plottype::histstack - Stacking this histogram into the histogram stack on
 *     the Pad. If the stack doesn't already exist, a new stack is created. Note
 *     that all histograms in the stack will be plotted with the hist style and
 *     cannot be changed. Notice that the stack must be plotted consecutively,
 *     meaning that any non histstack options would cause the stack to be
 *     finalized and plotted onto the Pad.
 *   - plottype::histnewstack - Force the creation of the new histogram stack,
 *     used only if you are plotting two histogram stacks in the same Pad.
 *   - plottype::histerr - Drawing histogram uncertainty as a shaded box region.
 *   - Raw string: The user can use the ROOT style string options to define how
 *     the Histogram options should be plotted. The strings `"SAME"` and
 *`"HIST"`
 *     would be handed by the functions and will be stripped.
 *
 * - EntryText: String to add in the legend entry. If this options is not
 *   present, then this object will NOT appear in the generated legend. Notice
 *   that the attributes to display in the legend would be generated from the
 *   PlotType used.
 *
 * - TrackY: Whether or not the y-axis range should be adjusted according to the
 *   newly added histogram. By default, only the max values of the histogram
 *   would be used.
 *
 * - PlotUnder: Specifying the that plot object be plotted underneath a specific
 *   object. Not that if the object specified in PlotUnder is not found in the
 *   pad, this option will have no effect and will raise no excpetions.
 *
 * One side note is that fitted functions will have its DrawOptions cleared from
 * the histogram! The user should be the one explicitly invoking the plotting
 * behavior, given more freedom to what the final plot will look like (See the
 * Pad1D::PlotFunc() method for more details). This design aspect is in stark
 * contrast with the design of ROOT objects. So beware of unwanted behaviour.
 */
TH1D&
Pad1D::PlotHist( TH1D& obj, const std::vector<RooCmdArg>& arglist )
{
  // If TProfile is given, send to PlotProfile function instead
  if( obj.InheritsFrom( TProfile::Class() ) ){
    return PlotProfile( dynamic_cast<TProfile&>( obj ), arglist );
  }

  // Getting the flags
  const RooArgContainer args( arglist, {
        PlotType( hist ), TrackY( tracky::max ), FillColor( 0, 0 ),
        MarkerStyle( sty::mkrcircle ), MarkerSize( 0.05 )
      } );

  if( !GetAxisObject() ){
    CreateAxisObject( obj, args );
  }

  // Forcing no statistics. and wiping title
  obj.SetStats( 0 );
  obj.SetTitle( "" );
  obj.SetBinErrorOption( TH1::kPoisson );// This options will not affect
  // weighted histograms, only the non-weighted ones.

  // Removing the poly marker from TSpectrum search function
  TObject*polymarker = obj.GetListOfFunctions()->FindObject( "TPolyMarker" );
  obj.GetListOfFunctions()->RecursiveRemove( polymarker );

  // Forcing fit functions to not be drawn
  for( const auto&& func : *( obj.GetListOfFunctions() ) ){
    func->SetBit( TF1::kNotDraw, true );
  }

  // Running the draw commands.
  const int pt = args.GetInt( "PlotType" );

  // Flushing the _working stack if hist is no longer used
  if( pt != histstack && _workingstack ){
    PlotObj( _workingstack, "HIST SAME NOCLEAR" );
    _workingstack = nullptr;
  }

  // Plot case for different plot types
  switch( pt ){
  case plottype::hist:
    PlotObj( obj, "HIST SAME" );
    break;
  case plottype::scatter:
    obj.GetXaxis()->IsVariableBinSize() ?
    PlotObj( obj, "P L E SAME" ) :
    PlotObj( obj, "P E X0 SAME" );
    break;
  case plottype::histerr:
    PlotObj( obj, "E2 SAME" );
    break;
  case plottype::histstack:
    if( _workingstack  == 0 ){
      _workingstack = &MakeObj<THStack>(
        ( "stack"+RandomString( 12 ) ).c_str(),
        "" );
    }
    _workingstack->Add( &obj, "HIST" );
    break;
  case plottype::histnewstack:
    _workingstack = &MakeObj<THStack>(
      ( "stack"+RandomString( 12 ) ).c_str(),
      "" );
    _workingstack->Add( &obj, "HIST" );
    break;
  case plottype::plottype_dummy:
    PlotObj( obj, ( args.GetStr( "PlotType" )+" SAME" ).c_str() );
    break;
  default:
    usr::log::PrintLog( usr::log::INTERNAL,
                        usr::fstr( "Skipping over invalid value (%d)", pt ) );
  }

  if( _workingstack ){
    TrackObjectY( *_workingstack, args.GetInt( "TrackY" ) );
  } else {
    TrackObjectY( obj, args.GetInt( "TrackY" ) );
  }

  // Adding legend
  if( args.Has( "EntryText" ) ){
    AddLegendEntry( obj, args.Get( "EntryText" ), args.Get( "PlotType" ) );
  }

  // Moving to under something
  if( args.Has( "PlotUnder" ) ){
    if( _workingstack ){
      PadBase::MoveTargetToBefore( *_workingstack, args.GetObj( "PlotUnder" ) );
    } else {
      PadBase::MoveTargetToBefore( obj, args.GetObj( "PlotUnder" ) );
    }
  }

  SetLineAttr( obj, args );
  SetFillAttr( obj, args );
  SetMarkAttr( obj, args );

  return obj;
}


/**
 * @brief  Plotting a TProfile object
 *
 * Due to inheritance between TProfile and the TH1D, the original PlotHist
 * doesn't work with TProfile. Instead, this functions generates a new TH1D from
 * the TProfile using the ProjectionX function. The return value would then be
 * the newly generated histogram. So beware of unexpected behaviour.
 */
TH1D&
Pad1D::PlotProfile( TProfile& obj, const std::vector<RooCmdArg>& args )
{
  // Generating new profile objects
  TH1D*_newhist = obj.ProjectionX( usr::RandomString( 6 ).c_str(), "E" );
  ClaimObject( _newhist );
  return PlotHist( _newhist, args );
}


/**
 * @brief Plotting a TEfficiency object
 *
 * Supported options are:
 * - PlotType: Definining how the data should be presented
 *   - plottype::scatter (default) - Plotting points with error bars,
 *     unfortunately, because the TEfficiency object has plotting functions
 *     similar to TGraphs and not TH1Ds, we cannot force the uniform bin display
 *     to have no x-error bars.
 *   - plottype::hist - Plotting as histograms. Because graphical options are
 *     using TGraphs and not TH1Ds, we are using the bar options ("B"). So we
 *     cannot have an unfilled color.
 *   - plottype::histerr - Drawing uncertainty as a shaded box region.
 *   - Raw string: The user can use the ROOT style string options to define how
 *     the Histogram options should be plotted. The strings `"SAME"` and
 *`"HIST"`
 *     would be handed by the functions and will be stripped.
 *
 * - EntryText: String to add in the legend entry. If this options is not
 *   present, then this object will NOT appear in the generated legend. Notice
 *   that the attributes to display in the legend would be generated from the
 *   PlotType used.
 *
 * - TrackY: Whether or not the y-axis range should be adjusted according to the
 *   newly added graph. By default, both the maximum and minimum will be tracked
 *   if this graph is the first thing to be plotted on the pad (excluding the
 *   histogram used for handling the axis), otherwise, nothing will be tracked.
 *
 * - PlotUnder: Specifying the that plot object be plotted underneath a specific
 *   object. Not that if the object specified in PlotUnder is not found in the
 *   pad, this option will have no effect and will raise no excpetions.
 */
TEfficiency&
Pad1D::PlotEff( TEfficiency& obj, const std::vector<RooCmdArg>& arglist )
{
  // Getting the flags
  const RooArgContainer args( arglist, {
        PlotType( scatter ), TrackY( tracky::both ),
        FillColor( usr::plt::col::cyan )
      } );

  if( obj.GetDimension() != 1 ){
    usr::log::PrintLog( usr::log::INTERNAL,
                        "Can only plot 1 Dimensional TEfficiency objects" );
    return obj;
  }

  if( !GetAxisObject() ){
    CreateAxisObject( obj, args );
  }

  // Forcing no statistics. and wiping title
  obj.SetTitle( "" );

  // Running the draw commands.
  const int pt = args.GetInt( "PlotType" );

  // Plot case for different plot types
  switch( pt ){
  case plottype::hist:
    PlotObj( obj, "B SAME" );
    gStyle->SetBarWidth( 1.05 );// For getting rid of the thin empty
    break;
  case plottype::scatter:
    PlotObj( obj, "PZ SAME" );
    break;
  case plottype::histerr:
    PlotObj( obj, "E2 SAME" );
    break;
  case plottype::plottype_dummy:
    PlotObj( obj, ( args.GetStr( "PlotType" )+" SAME" ).c_str() );
    break;
  default:
    usr::log::PrintLog( usr::log::INTERNAL,
                        usr::fstr( "Skipping over invalid value (%d)", pt ) );
  }

  TrackObjectY( obj, args.GetInt( "TrackY" ) );

  // Adding legend
  if( args.Has( "EntryText" ) ){
    AddLegendEntry( obj, args.Get( "EntryText" ), args.Get( "PlotType" ) );
  }

  // Moving to under something
  if( args.Has( "PlotUnder" ) ){
    PadBase::MoveTargetToBefore( obj, args.GetObj( "PlotUnder" ) );
  }

  SetLineAttr( obj, args );
  SetFillAttr( obj, args );
  SetMarkAttr( obj, args );

  return obj;
}


/**
 * Plotting of the TGraph object has the following supporting options:
 *
 * - `PlotType`: Defining how the data should be represented on the Pad. The
 *   supported types are:
 *   - `plottype::simplefunc`(default): single polyline joining the data points.
 *     This is used if the graph represents a function sample.
 *   - `plottype::fittedfunc`: a polyline joining the data points with the Y
 *     error represented with a shaded region. This is used if the graph
 *     represents a function sample with additional sampling for fitting
 *     uncertainties
 *   - `plottype::scatter`: plotting the data as data points with error bars.
 *     Unlike the histograms, this would not attempt to adjust the x error bars.
 *
 * - EntryText: String to add in the legend entry. If this options is not
 *   present, then this object will NOT appear in the generated legend. Notice
 *   that the attributes to display in the legend would be generated from the
 *   PlotType used.
 *
 * - TrackY: Whether or not the y-axis range should be adjusted according to the
 *   newly added graph. By default, both the maximum and minimum will be tracked
 *   if this graph is the first thing to be plotted on the pad (excluding the
 *   histogram used for handling the axis), otherwise, nothing will be tracked.
 *
 * - PlotUnder: Specifying the that plot object be plotted underneath a specific
 *   object. Not that if the object specified in PlotUnder is not found in the
 *   pad, this option will have no effect and will raise no excpetions.
 *
 * One side note is that fitted functions will have its DrawOptions cleared from
 * the histogram! The user should be the one explicitly invoking the plotting
 */
TGraph&
Pad1D::PlotGraph( TGraph& obj, const std::vector<RooCmdArg>& arglist )
{
  // Early Exit for Graphs without any data points
  if( obj.GetN() <= 0 ){
    usr::log::PrintLog( usr::log::INTERNAL,
                        "Cannot plot TGraphs with no data points!" );
    return obj;
  }

  // Getting the flags
  const RooArgContainer args( arglist, {
        PlotType( simplefunc ), !GetAxisObject() ?
        TrackY( tracky::both ) :
        TrackY( tracky::none ), FillColor( usr::plt::col::white, 0 ),
        MarkerStyle( sty::mkrcircle ), MarkerSize( 0.05 ), ExtendXRange( true )
      } );

  // If no axis are available. Generating a TH1 object for axis:
  if( !GetAxisObject() ){
    CreateAxisObject( obj, args );
  }

  // Object fixing
  obj.SetTitle( "" );// Forcing clear title. This should be handled by Canvas.

  // Forcing fit functions to not be drawn
  for( const auto&& func : *( obj.GetListOfFunctions() ) ){
    func->SetBit( TF1::kNotDraw, true );
  }

  const int pt = args.GetInt( "PlotType" );

  switch( pt ){
  case simplefunc:
    PadBase::PlotObj( obj, "LX" );
    break;
  case fittedfunc:
    PadBase::PlotObj( obj, "3" );// Draw Error with fill region and then
    PadBase::PlotObj( obj, "LX" );// Draw the central line. All errors disabled.
    break;
  case scatter:
    // Points, no error bar end ticks, show error bar for points  outside range.
    PadBase::PlotObj( obj, "PZ0" );
    break;
  case plottype_dummy:
    PlotObj( obj, args.GetStr( "PlotType" ).c_str() );
    break;
  default:
    usr::log::PrintLog( usr::log::INTERNAL,
                        usr::fstr( "Skipping over invalid value (%d)", pt ) );
    break;
  }

  TrackObjectY( obj, args.GetInt( "TrackY" ) );

  // Adding legend
  if( args.Has( "EntryText" ) ){
    AddLegendEntry( obj, args.Get( "EntryText" ), args.Get( "PlotType" ) );
  }

  // Moving to under something
  if( args.Has( "PlotUnder" ) ){
    PadBase::MoveTargetToBefore( obj, args.GetObj( "PlotUnder" ) );
  }

  // Setting styling attributes
  SetLineAttr( obj, args );
  SetFillAttr( obj, args );
  SetMarkAttr( obj, args );

  return obj;
}


/**
 * Plotting a TF1 object is done by generating a TGraph with 300 samples points
 * across the x axis, and plotting the TGraph instead. All TGraph plotting
 * options will be available for the TF1 plotting. There is a new plotting
 * options VisualizeError, which generates a TGraphErrors graph by randomly
 * sampling the parameter space according to the correlation matrix given by a
 * @ROOT{TFitResult}.
 */
TGraph&
Pad1D::PlotFunc( TF1& func, const std::vector<RooCmdArg>& arglist )
{
  const RooArgContainer args( arglist, {
        PlotType(// Inserting default plotting style
          RooArgContainer::CheckList( arglist, "VisualizeError" ) ?
          fittedfunc :
          simplefunc ), RooFit::Precision( 1e-3 )
      }
                              );

  // If no axis are available. Generating a TH1 object for axis:
  if( !GetAxisObject() ){
    CreateAxisObject( func, args );
  }

  return PlotGraph( MakeTF1Graph( func, args ), args );
}


/**
 * @brief  Making the central value of the function into a TGraph in
 * preparation for plotting.
 *
 * Notice that the caller of this function should handle the ownership of the
 * generated TGraph object.
 */
TGraph*
Pad1D::MakeTF1GraphCentral( const TF1& func, const double precision )
{
  TF1            f       = func; // Making a copy of the function
  const double   xmax    = f.GetXmax();
  const double   xmin    = f.GetXmin();
  const unsigned xsample = 1 / ( precision )+1;

  std::vector<double> x( xsample );
  std::vector<double> y( xsample );

  // Getting common elements for graph generation
  for( unsigned i = 0; i < xsample; ++i ){
    const double xval = xmin+i * ( xmax-xmin ) * precision;
    const double yval = f.Eval( xval );
    x[i] = xval;
    y[i] = yval;
  }

  TGraph* ans = new TGraph( x.size(), x.data(), y.data() );
  ans->SetName( ( func.GetName()+std::string( "_gengraph" )
                  +usr::RandomString( 6 )).c_str() );
  return ans;
}


/**
 * @brief Creating the TF1 with uncertainty, ignoring the correlation between
 * the various fitting parameters.
 *
 * Here we will simply call the MakeTF1GraphMatrix method, with a perfectly
 * diagonal covariance matrix.
 */
TGraphAsymmErrors*
Pad1D::MakeTF1GraphNoCorr( const TF1&   func,
                           const double precision,
                           const double z )
{
  // Constructing the diagonal covariance matrix
  TMatrixDSym corr( func.GetNpar() );
  for( int i = 0 ; i < func.GetNpar(); ++i  ){
    corr[i][i] = func.GetParError( i ) * func.GetParError( i );
  }

  return MakeTF1GraphMatrix( func, corr, precision, z );
}


/**
 * @brief Creating the TF1 with uncertainty, given the covariance matrix of the
 * fitting paramters.
 *
 * Here we are assuming that the fit results gives  a sufficiently
 * ellipsoid-like uncertainty for the fit paramters around the function fit
 * values. The principle radii of the ellipsoid is then given by the eigenvalue
 * and eigenvector of the covariance matrix. We then take a sample of parameter
 * points on this ellipsoid (scaled by the desired "sigma interval"),  and use
 * this to compute an envelope of the function given the fit result.
 *
 * For fixed parameters, the default components along the fixed parameter index
 * will be zero. As this breaks the eigenvector decomposition function, we will
 * construct the matrix such that the diagonal value will always be non-zero,
 */
TGraphAsymmErrors*
Pad1D::MakeTF1GraphMatrix( const TF1&         func,
                           const TMatrixDSym& _corr,
                           const double       precision,
                           const double       z )
{
  std::unique_ptr<TGraph> central( MakeTF1GraphCentral( func, precision ));

  // Making a duplicate of the original function object.
  TF1         f    = func;
  TMatrixDSym corr = _corr;

  // Making sure the matrix is not sigular
  assert( f.GetNpar() == corr.GetNcols() );
  for( int i = 0 ; i < f.GetNpar() ; ++i ){
    if( corr[i][i] == 0 ){
      corr[i][i] = 0.000001;
    }
  }

  const unsigned      npar = f.GetNpar();
  const TMatrixDEigen eigen( corr );

  for( unsigned i = 0 ; i < npar ; ++i ){
    if( eigen.GetEigenValuesIm()[i] != 0 ){
      usr::log::PrintLog(
        usr::log::INTERNAL,
        "Warning! Covariance matrix generated imaginary eigenvalue components!"
        "Reverting to uncorrelated version of function generation" );
      return MakeTF1GraphNoCorr( func, precision, z );
    }
  }

  TMatrixD radii = eigen.GetEigenValues();
  radii.Sqrt();

  // preparing container for uncertainties.
  std::vector<double>       yerrhi( central->GetN(), 0.0 );
  std::vector<double>       yerrlo( central->GetN(), 0.0 );
  const std::vector<double> zeros( central->GetN(), 0.0 );

  // calculating number of samples based on the surface area of an n-sphere:
  double nsamples = 400;
  nsamples *= TMath::Power( TMath::Pi(), double(npar) / 2 );
  nsamples /= TMath::Gamma( double(npar) / 2 );

  for( unsigned i = 0 ; i < nsamples ; ++i ){
    const TVectorD shift = ( radii * eigen.GetEigenVectors()).T()
                           * usr::RandomOnSphere( npar ) * z;

    // Shifting the paramters
    for( unsigned j = 0 ; j < npar; ++j  ){
      f.SetParameter( j, func.GetParameter( j )+shift[j] );
    }

    // Evaluating the various function values
    for( int j = 0 ; j < central->GetN(); ++j ){
      const double x      = central->GetX()[j];
      const double cent_y = central->GetY()[j];
      const double test_y = f.Eval( x );

      yerrhi[j] = std::max( test_y-cent_y, yerrhi.at( j ));
      yerrlo[j] = std::max( cent_y-test_y, yerrlo.at( j ));
    }
  }

  TGraphAsymmErrors* ans = new TGraphAsymmErrors( central->GetN(),
                                                  central->GetX(),
                                                  central->GetY(),
                                                  zeros.data(),
                                                  zeros.data(),
                                                  yerrlo.data(),
                                                  yerrhi.data());
  ans->SetName( ( func.GetName()+std::string( "_gengraph" )
                  +usr::RandomString( 6 )).c_str() );
  return ans;
}


/**
 * @brief Generating a graph representation of the TF1 object.
 */
TGraph&
Pad1D::MakeTF1Graph( TF1& func, const RooArgContainer& args  )
{
  const double precision = args.GetDouble( "Precision" );

  TGraph* g;
  if( !args.Has( "VisualizeError" ) ){
    g = MakeTF1GraphCentral( func, precision );
  } else {
    const TFitResult& fit =
      dynamic_cast<const TFitResult&>( args.GetObj( "VisualizeError" ) );
    const double zval = args.GetDouble( "VisualizeError" );
    const int    corr = args.GetInt( "VisualizeError" );

    g = corr ?
        MakeTF1GraphMatrix( func, fit.GetCovarianceMatrix(), precision, zval ) :
        MakeTF1GraphNoCorr( func, precision, zval );
  }
  ClaimObject( g );
  return *g;
}


/**
 * Function for plotting all of the RooAbsData objects, the supported options
 * allowed include:
 * - *Any* RooCmdArg supported by the RooAbsData::plotOn function. These will
 *   take precedence to custom defined options. With the excpetion of
 *   RooFit::DrawOptions.
 *
 * - PlotType: Specifying the representation for the data object on the Pad. Any
 *   of the types valid for the PlotGraph functions would be valid. By default,
 *   the scatter plot method would be used.
 *
 * - EntryText: String to add in the legend entry. If this options is not
 *   present, then this object will NOT appear in the generated legend. Notice
 *   that the attributes to display in the legend would be generated from the
 *   PlotType used. Notice that this method is overwritten by the
 *   RooFit::Invisible option, which forces the object to be hidden.
 *
 * - TrackY: Whether or not the y-axis range should be adjusted according to the
 *   newly added graph. By default, only the maximum value would be used.
 *
 * Additional automation of options include suppressing the X error bars of the
 * generated graph if an equal binning is used for the data set and the
 * RooFit::XErrorSize is not specified. This is to align the plotting style
 * conventions within the CMS collaboration. Another change to the graphs is
 * that
 * the y uncertainties in the zero bins are suppressed, again, to align with the
 * plotting style conventions.
 *
 * The function returns a reference to the generated TGraph object by the
 * underlying RooPlot object, The object would be owned by the RooPlot and be
 * destroyed when the Pad1D goes out of scope.
 */
TGraphAsymmErrors&
Pad1D::PlotData( RooAbsData& data, const std::vector<RooCmdArg>& arglist )
{
  const RooArgContainer args( arglist, {
        PlotType( scatter ), TrackY( tracky::max )
      }
                              );

  TGraphAsymmErrors& ans = MakeDataGraph( data, args );

  if( !args.Has( "Invisible" ) ){
    if( _prevrangetype == rangetype::aut ){
      // Forcing the range type to be histogram like.
      _prevrangetype = rangetype::hist;
    }
    PlotGraph( ans, args );
  }

  return ans;
}


/**
 * @brief Helper method for generating the a TGraph representing a RooAbsData
 * plot.
 *
 * This will detect the binning method specified by the various options (using
 * the present binning of the RooFrame variable otherwise), and in the case of
 * uniform binning, suppress the x axis error bars, as specified by the CMS
 * publication conventions. Additionally, Bins with zero entires would have
 * their
 * y error bars suppressed.
 */
TGraphAsymmErrors&
Pad1D::MakeDataGraph( RooAbsData& data, const RooArgContainer& args )
{
  // RooCmdArg for suppressing the X errors in the final graph
  static const RooCmdArg suppressxerror = RooFit::XErrorSize( 0 );

  // Generating the requirements for actual RooFit PlotOn calls
  RooLinkedList oplist = args.MakeRooList();

  auto IsUniform = [this]( const RooCmdArg& cmd )->bool {
                     // Double assignment will always be uniform
                     if( cmd.getDouble( 0 ) != cmd.getDouble( 1 ) ){
                       return true;
                     }

                     // Getting plot variable bining scheme
                     if( cmd.getString( 0 ) ){
                       const auto binname = cmd.getString( 0 );
                       const auto plotvar = this->_frame.getPlotVar();
                       if( plotvar->hasBinning( binname ) ){
                         return plotvar->getBinning( binname ).isUniform();
                       }
                     } else if( cmd.getObject( 0 ) ){
                       auto binobj =
                         dynamic_cast<const RooAbsBinning*>( cmd.getObject( 0 ) );
                       if( binobj  ){
                         return binobj->isUniform();
                       }
                     }
                     return true;// Returning true by default
                   };

  // Option for suppressing x error bars
  if( !args.Has( "Binning" ) ){
    if( _frame.getPlotVar()->getBinning().isUniform() ){
      oplist.Add( suppressxerror.Clone() );
    }
  } else if( IsUniform( args.Get( "Binning" ) ) ){
    oplist.Add( suppressxerror.Clone() );
  }

  // Generating the object via RooFit
  TGraphAsymmErrors& ans = GenGraph( data, oplist );

  // Additional Fixes for RooFit generated objects
  for( int i = 0; i < ans.GetN(); ++i ){
    if( ans.GetY()[i] == 0 ){// Suppressing errors in zero bins
      ans.SetPointError( i, 0, 0, 0, 0 );
    }
  }

  return ans;
}


/**
 * Plotting of RooAbsPdf objects by generating the TGraphs using a RooPlot
 * object
 * and plotting the graphs onto the TPad. The supported options are:
 *
 * - *Any* RooCmdArg supported by the RooAbsData::plotOn function. These will
 *   take precedence to custom defined options, with the excpetion of the
 *   RooFit::DrawOptions method.
 *
 * - PlotType: Specifying the representation for the data object on the Pad. Any
 *   of the types valid for the PlotGraph functions would be valid. If not
 *   specified, the simplefunc or fittedfunc methods will be used, depending on
 *   if the RooFit::VisualizeError is used.
 *
 * - EntryText: String to add in the legend entry. If this options is not
 *   present, then this object will NOT appear in the generated legend. Notice
 *   that the attributes to display in the legend would be generated from the
 *   PlotType used.
 *
 * - TrackY: Whether or not the y-axis range should be adjusted according to the
 *   newly added graph. By default, RooPdf objects would not be used to adjust
 *   the axis range.
 *
 * Additional automation of options include additional generation for
 * RooFit::VisualizeError. The stock RooPlot generates are contour line for the
 * uncertainty range rather than a line with error, making styling of a PDF with
 * uncertainty rather tedious. This functions takes the generated TGraphs by the
 * RooPlot and recalculated a TGraph with uncertainty. The newly calculated
 * graph
 * will be placed under the ownership of the Pad.
 */
TGraph&
Pad1D::PlotPdf( RooAbsPdf& pdf, const std::vector<RooCmdArg>& arglist )
{
  const RooArgContainer args( arglist, {// Defining default arguments
        TrackY( tracky::max ), // Default track y: only top
        RooArgContainer::CheckList( arglist, "VisualizeError" ) ?
        PlotType( fittedfunc ) :
        PlotType( simplefunc )                         // default plot style.
      }
                              );
  TGraph& ans = MakePdfGraph( pdf, arglist );

  if( !args.Has( "Invisible" ) ){// Allow for invisible stuff to exist
    if( _prevrangetype == rangetype::aut ){
      // Forcing the range type to be histogram like.
      _prevrangetype = rangetype::hist;
    }
    PlotGraph( ans, args );
  }

  return ans;
}


/**
 * @brief Helper function for generating a TGraph representation of a RooAbsPdf
 * plot
 *
 * In the case the RooFit::Visualization is used, this function generates two
 * graphs via the in-built RooFit methods , one for the central plot, and the
 * other the contor of the uncertainty. This function then recalculates the two
 * graphs into a single TGraph with uncertainty, which is makes manipulating the
 * the plot results of a single RooAbsPdf instance more intuitive (changing the
 * plot style of a single object, rather than two).
 */
TGraph&
Pad1D::MakePdfGraph( RooAbsPdf& pdf, const RooArgContainer& args )
{
  // Augmenting the Visualize Error command. We have to manually insert a
  // RooArgSet containing all the floating variables directly in the
  // VisualizeError instance. Otherwise the generation of the error band might
  // fail.
  auto CorrectVisError =
    [this]( const RooAbsPdf& pdf,
            const usr::RooArgContainer& args )->RooCmdArg {
      if( args.Has( "VisualizeError" ) ){
        auto& cmd = args.Get( "VisualizeError" );
        if( cmd.getSet( 0 ) ){
          // If Visualized Parameter set is already specified, then simply
          // return the original parameter
          return cmd;
        } else {
          RooCmdArg           ans( cmd );
          const RooFitResult& fit =
            *dynamic_cast<const RooFitResult*>( cmd.getObject( 0 ) );
          RooArgSet*cloneParams =
            pdf.getObservables( fit.floatParsFinal() );
          ans.setSet( 0, *cloneParams );
          this->ClaimObject( cloneParams );
          return ans;
        }
      } else {
        return RooCmdArg::none();
      }
    };

  const RooCmdArg viscmd  = CorrectVisError( pdf, args );
  RooLinkedList   roolist = args.MakeRooList( {"VisualizeError"} );

  if( !args.Has( "VisualizeError" ) ){
    // Nothing special needs to be done for simple plotting.
    return GenGraph( pdf, roolist );
  } else {
    TGraph& centralgraph = GenGraph( pdf, roolist );
    roolist.Add( viscmd.Clone() );
    TGraph& errorgraph = GenGraph( pdf, roolist );

    // Map for storing uncertainty
    std::map<double, std::pair<double, double> > fiterr;

    for( int i = 0; i < errorgraph.GetN(); ++i ){
      const double x = errorgraph.GetX()[i];
      const double y = errorgraph.GetY()[i];
      if( !fiterr.count( x ) ){
        fiterr[x] = std::make_pair( 1e10, -1e10 );
      }
      fiterr.at( x ).first  = std::min( fiterr.at( x ).first, y  );
      fiterr.at( x ).second = std::max( fiterr.at( x ).second, y );
    }

    // The new TGraphAsymmErrors to store the results.
    TGraphAsymmErrors& ans = MakeObj<TGraphAsymmErrors>( fiterr.size() );
    unsigned           i   = 0;// index for looping

    for( const auto& fiterrval : fiterr ){
      const double x   = fiterrval.first;
      const double y   = centralgraph.Eval( x );
      const double ylo = fiterrval.second.first;
      const double yhi = fiterrval.second.second;
      ans.SetPoint( i, x, y );
      ans.SetPointError( i, 0, 0, y-ylo, yhi-y );
      ++i;
    }

    return ans;
  }
}


/**
 * @brief Helper function for generating RooAbsPdf plots onto a Pad.
 *
 * Throws and invalid_argument exception when the plotting call fails.
 */
TGraph&
Pad1D::GenGraph( RooAbsPdf& pdf, RooLinkedList& arglist )
{
  // Suppressing plotting messages
  RooMsgService::instance().setGlobalKillBelow( RooFit::WARNING );

  RooPlot*test = pdf.plotOn( &_frame, arglist );
  if( !test ){
    throw std::invalid_argument(
            "Bad argument list or object, plotting failed" );
  }

  auto& graph = _frame.LastPlot<TGraph>();
  graph.SetName( ( pdf.GetName()+RandomString( 6 ) ).c_str() );

  // Since this is a PDF and there is no weighting issue, we
  // enforce the fact that the graph should be positive definite.
  for( int i = 0; i < graph.GetN(); ++i ){
    const double x = graph.GetX()[i];
    const double y = graph.GetY()[i];
    if( y <= 0.0 ){
      graph.SetPoint( i, x, 1e-50 );
    }
  }

  return graph;
}


/**
 * @brief Helper function for generating RooAbsData plots onto a Pad.
 *
 * Throws an invalid_argument exception when the plotting call fails.
 */
TGraphAsymmErrors&
Pad1D::GenGraph( RooAbsData& data, RooLinkedList& arglist )
{
  // Suppressing plotting messages
  RooMsgService::instance().setGlobalKillBelow( RooFit::WARNING );

  // Generating plotting information
  RooPlot*test = data.plotOn( &_frame, arglist );
  if( !test ){
    throw std::invalid_argument(
            "Bad argument list or object, plotting failed" );
  }
  return _frame.LastPlot<TGraphAsymmErrors>();
}


/**
 * @brief Changing the stored _datamin, and _datamax variable according to
 * object
 *
 * Moving to a private helper function to reduce verbosity in main
 * implementation
 * function
 */
void
Pad1D::TrackObjectY( const TObject& obj, const int tracky )
{
  // Perfroming the axis range setting
  if( tracky == tracky::min || tracky == tracky::both ){
    const double obj_min = obj.InheritsFrom( TH1D::Class() ) ?
                           GetYmin( &dynamic_cast<const TH1D&>( obj ) ) :
                           obj.InheritsFrom( TGraph::Class() ) ?
                           GetYmin( &dynamic_cast<const TGraph&>( obj ) ) :
                           obj.InheritsFrom( THStack::Class() ) ?
                           GetYmin( &dynamic_cast<const THStack&>( obj ) ) :
                           obj.InheritsFrom( TEfficiency::Class() ) ?
                           GetYmin( &dynamic_cast<const TEfficiency&>( obj ) ) :
                           0;
    _datamin = obj_min == 0 ?
               _datamin :
               std::min( _datamin, obj_min );
  }
  if( tracky == tracky::max || tracky == tracky::both ){
    const double obj_max = obj.InheritsFrom( TH1D::Class() ) ?
                           GetYmax( &dynamic_cast<const TH1D&>( obj ) ) :
                           obj.InheritsFrom( TGraph::Class() ) ?
                           GetYmax( &dynamic_cast<const TGraph&>( obj ) ) :
                           obj.InheritsFrom( THStack::Class() ) ?
                           GetYmax( &dynamic_cast<const THStack&>( obj ) ) :
                           obj.InheritsFrom( TEfficiency::Class() ) ?
                           GetYmax( &dynamic_cast<const TEfficiency&>( obj ) ) :
                           0;
    _datamax = std::max( _datamax, obj_max );
  }
  AutoSetYRange();
}


void
Pad1D::PlotCreatedAxisObject( TH1D& axisobj )
{
  axisobj.Reset();
  axisobj.SetStats( 0 );
  axisobj.SetTitle( "" );
  axisobj.SetName( ( genaxisname+RandomString( 6 ) ).c_str() );
  PlotObj( axisobj, "AXIS" );
  this->SetAxisFont();
}


void
Pad1D::CreateAxisObject( const TH1D& oldhist, const RooArgContainer& args )
{
  // MUST ust a clone, otherwise messes with THStack
  TH1D& axisobj = MakeObj<TH1D>( oldhist );
  PlotCreatedAxisObject( axisobj );
}


void
Pad1D::CreateAxisObject( const TF1& func, const RooArgContainer& args )
{
  // If no axis are available. Generating a TH1 object for axis:
  TH1D& axishist = MakeObj<TH1D>(
    ( genaxisname+RandomString( 10 ) ).c_str(),
    "",
    10,
    func.GetXmin(),
    func.GetXmax() );
  PlotCreatedAxisObject( axishist );
}


void
Pad1D::CreateAxisObject( const TGraph& graph, const RooArgContainer& args )
{
  const double xmin = GetXmin( graph );
  const double xmax = GetXmax( graph );
  const double diff = args.GetInt( "ExtendXRange" ) ?
                      xmax-xmin :
                      0;
  TH1D& axishist = MakeObj<TH1D>(
    ( genaxisname+RandomString( 6 ) ).c_str(),
    "",
    10,
    xmin-0.1 * diff,
    xmax+0.1 * diff );
  PlotCreatedAxisObject( axishist );
}


void
Pad1D::CreateAxisObject( const TEfficiency& obj, const RooArgContainer& args )
{
  // MUST ust a clone, otherwise messes with THStack
  TH1D& axisobj = MakeObj<TH1D>( *( (TH1D*)obj.GetTotalHistogram() ) );
  PlotCreatedAxisObject( axisobj );
}

}/* plt */

}/* usr  */
