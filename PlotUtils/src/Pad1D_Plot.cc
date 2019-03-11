/**
 * @file    Pad1D_Plot.cc
 * @brief   Implementing the data plotting functions.
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/Maths.hpp"
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"
#include "UserUtils/PlotUtils/interface/Pad1D.hpp"
#else
#include "UserUtils/Common/Maths.hpp"
#include "UserUtils/Common/STLUtils/StringUtils.hpp"
#include "UserUtils/PlotUtils/Pad1D.hpp"
#endif

#include <random>

#include "TGraphErrors.h"
#include "TDecompChol.h"
#include "TFitResult.h"
#include "TList.h"

namespace usr  {

namespace plt {

/**
 * Plotting 1D histograms accepts three types of options:
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
 *     finalised and plotted onto the Pad.
 *   - plottype::histnewstack - Force the creation of the new histogram stack,
 *     used only if you are plotting two histogram stacks in the same Pad.
 *   - plottype::histerr - Drawing histogram uncertainty as a shaded box region.
 *   - Raw string: The user can use the ROOT style string options to define how
 *     the Histogram options should be plotted. The strings `"SAME"` and
 *     `"HIST"` would be handed by the functions and will be stripped.
 *
 * - EntryText: String to add in the legend entry. If this options is not
 *   present, then this object will NOT appear in the generated legend. Notice
 *   that the attributes to display in the legend would be generated from the
 *   PlotType used.
 *
 * - TrackY: Whether or not the y-axis range should be adjusted according to the
 *   newly added histogram. By default, if only graphs exists in the pad
 *   (excluding the axis object), both the min and max values in the graph would
 *   be used; otherwise none of the values would be used.
 *
 * One side note is that fitted functions will have its DrawOptions cleared from
 * the histogram! The user should be the one explicitly invoking the plotting
 * behavior, given more freedom to what the final plot will look like (See the
 * Pad1D::PlotFunc method for more details). This design aspect is in stark
 * contrast with the design of ROOT objects. So beware of unwanted behaviour.
 */
TH1D&
Pad1D::PlotHist( TH1D& obj, const std::vector<RooCmdArg>& arglist )
{
  // Forcing no statistics. and wiping title
  obj.SetStats( 0 );
  obj.SetTitle( "" );

  // Getting the flags
  const RooArgContainer args( arglist );
  const int opt
    = !args.Has( PlotType::CmdName ) ? plottype::hist :
      args.Get( PlotType::CmdName ).getString( 0 ) ? plottype::plottype_dummy :
      args.Get( PlotType::CmdName ).getInt( 0 );
  const int trky
    = !args.Has( TrackY::CmdName ) ? TrackY::max :
      args.Get( TrackY::CmdName ).getInt( 0 ) == TrackY::aut ? TrackY::max :
      args.Get( TrackY::CmdName ).getInt( 0 );
  std::string optraw =
    !args.Has( PlotType::CmdName ) ?  "" :
    !args.Get( PlotType::CmdName ).getString( 0 ) ? "" :
    args.Get( PlotType::CmdName ).getString( 0 );

  if( !GetAxisObject() ){
    TH1D& axisobj = _frame.MakeObj<TH1D>( obj );
    // MUST ust a clone
    // otherwise messes with THStack
    axisobj.Reset();
    PlotObj( axisobj, "AXIS" );
    this->SetAxisFont();
  }

  // Forcing fit functions to not be drawn
  for( const auto&& func : *(obj.GetListOfFunctions()) ){
    func->SetBit( TF1::kNotDraw, true );
  }

  // Flushing the _working stack if hist is no longer used
  if( opt != plottype::histstack && _workingstack ){
    PlotObj( _workingstack, "HIST SAME NOCLEAR" );
    _workingstack = nullptr;
  }

  // Parsing plotting flag
  if( opt == plottype::hist ){
    PlotObj( obj, "HIST SAME" );

  } else if( opt == plottype::scatter ){
    if( obj.GetXaxis()->IsVariableBinSize() ){
      PlotObj( obj, "P L E SAME" );
    } else {
      PlotObj( obj, "P E X0 SAME" );
    }

  } else if( opt == plottype::histerr ){
    PlotObj( obj, "E2 SAME" );

  } else if( opt == plottype::histstack ){
    if( !_workingstack ){
      _workingstack =
        &_frame.MakeObj<THStack>( RandomString( 12 ).c_str(), "" );
    }
    _workingstack->Add( &obj );

  } else if( opt == plottype::histnewstack ){
    _workingstack = &_frame.MakeObj<THStack>( RandomString( 12 ).c_str(), "" );
    _workingstack->Add( &obj );

  } else if( opt == plottype::plottype_dummy && optraw != "" ){
    // Special case for raw options parsing. (Must remove axis and add string)
    ToUpper( optraw );
    StripSubstring( optraw, "AXIS" );
    PlotObj( obj, ( optraw+" SAME" ).c_str() );

  } else {
    std::cerr << "Skipping over invalid value" << std::endl;
  }

  // Parsing the tracking options
  if( trky == TrackY::min || trky == TrackY::both ){
    _datamin = std::min( _datamin, GetYmin( obj ) );
    if( _workingstack ){
      _datamin = std::min( _datamin, GetYmin( _workingstack ) );
    }
  }
  if( trky == TrackY::max || trky == TrackY::both ){
    _datamax = std::max( _datamax, GetYmax( obj ) );
    if( _workingstack ){
      _datamax = std::max( _datamax, GetYmax( _workingstack ) );
    }
  }
  AutoSetYRange();

  // Adding legend
  if( args.Has( EntryText::CmdName ) ){
    const PlotType plotopt =
      opt == plottype_dummy ? PlotType( optraw ) :
      PlotType( opt );
    const std::string leg = args.Get( EntryText::CmdName ).getString( 0 );
    AddLegendEntry( obj, leg, plotopt );
  }

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
 *   newly added graph. By default, only the maximum value of the histogram will
 *   be used to adjust the y-axis range.
 */
TGraph&
Pad1D::PlotGraph( TGraph& obj, const std::vector<RooCmdArg>& args )
{
  // Early Exit for Graphs without any data points
  if( obj.GetN() <=0 ){
    std::cerr << "Cannot plot TGraphs with no data points!" << std::endl;
    return obj;
  }


  obj.SetTitle( "" );// Forcing clear title. This should be handled by Canvas.

  // Getting the flags
  const RooArgContainer arglist( args );
  const int opt =
    !arglist.Has( PlotType::CmdName ) ? plottype::simplefunc :
    arglist.Get( PlotType::CmdName ).getString( 0 ) ? plottype_dummy :
    arglist.Get( PlotType::CmdName ).getInt( 0 );
  const int trky
    = !arglist.Has( TrackY::CmdName ) ? TrackY::aut : // more complex
      arglist.Get( TrackY::CmdName ).getInt( 0 );// parsing latter
  std::string optraw =
    !arglist.Has( PlotType::CmdName ) ?  "" :
    !arglist.Get( PlotType::CmdName ).getString( 0 ) ? "" :
    arglist.Get( PlotType::CmdName ).getString( 0 );

  // If no axis are available. Generating a TH1 object for axis:
  if( !GetAxisObject() ){
    auto& axishist = _frame.MakeObj<TH1F>(
      ( "axishist" + RandomString( 10 ) ).c_str(),
      ( "axishist" + RandomString( 10 ) ).c_str(),
      10, GetXmin( obj ), GetXmax( obj ) );
    axishist.SetStats( 0 );
    PadBase::PlotObj( axishist, "AXIS" );
    SetAxisFont();
  }

  // Forcing fit functions to not be drawn
  for( const auto&& func : *(obj.GetListOfFunctions()) ){
    func->SetBit( TF1::kNotDraw, true );
  }

  if( opt == plottype::simplefunc ){
    PadBase::PlotObj( obj, "LX" );
  } else if( opt == plottype::fittedfunc ){
    PadBase::PlotObj( obj, "3" );
    // Draw Error with fill region and then
    PadBase::PlotObj( obj, "LX" );
    // Draw the central line.all errors disabled.
  } else if( opt == plottype::scatter ){
    // Point, no error bar end ticks, and show error bar for points
    // outside range.
    PadBase::PlotObj( obj, "PZ0" );
  } else if( opt == plottype_dummy && optraw != "" ){
    ToUpper( optraw );
    StripSubstring( optraw, "A" );
    PlotObj( obj, ( optraw ).c_str() );
  } else {
    std::cerr << "Skipping over invalid value" << std::endl;
  }

  // Automatic tracking options  require determining the type of the first
  // plotted object. If anything already exists in the pad, track nothing,
  // otherwise attempt to track both.
  int optrky = trky;
  if( trky == TrackY::aut ){
    optrky = TrackY::both;
    if( !PadBase::GetListOfPrimitives() ){ optrky = TrackY::both; }
    TIter next( PadBase::GetListOfPrimitives() );
    TObject* iter;

    while( ( iter = next() ) ){
      if( ( iter->InheritsFrom( TH1::Class() )
            && ( (TH1*)iter )->GetXaxis() != _frame.GetXaxis() )
          || iter->InheritsFrom( THStack::Class() )
          || iter->InheritsFrom( TGraph::Class() )
          // ignoring the RooPlot frame defining histogram
          // (using this weird syntax since this function is both)
           ){
        optrky = TrackY::none;
        break;
      }
    }
  }

  // Perfroming the axis range setting
  if( optrky == TrackY::min || optrky == TrackY::both ){
    _datamin = std::min( _datamin, GetYmin( obj ) );
  }
  if( optrky == TrackY::max || optrky == TrackY::both ){
    _datamax = std::max( _datamax, GetYmax( obj ) );
  }
  AutoSetYRange();

  // Adding legend
  if( arglist.Has( EntryText::CmdName ) ){
    const PlotType plotopt =
      opt == plottype_dummy ? PlotType( optraw ) :
      PlotType( opt );
    const std::string leg = arglist.Get( EntryText::CmdName ).getString( 0 );
    AddLegendEntry( obj, leg, plotopt );
  }

  return obj;
}

/**
 * Plotting a TF1 object is done by generating a TGraph with 300 samples points
 * across the x axis, and plotting the TGraph instead. All TGraph plotting
 * options will be available for the TF1 plotting. There is a new plotting
 * options ShowFitErr, which generates a TGraphErrors graph by randomly
 * sampling the parameter space according to the correlation matrix given by a
 * @ROOT{TFitResult}.
 */
TGraph&
Pad1D::PlotFunc( TF1& func, const std::vector<RooCmdArg>& arglist )
{
  static const unsigned xsample = 300;
  static const unsigned psample = 1000;

  const RooArgContainer args( arglist );

  std::vector<double> x;
  std::vector<double> y;
  std::vector<double> yerr;
  std::vector<double> zero;

  std::vector<std::vector<double> > ysample;


  // If no axis are available. Generating a TH1 object for axis:
  if( !GetAxisObject() ){
    auto& axishist = _frame.MakeObj<TH1F>(
      ( "axishist" + RandomString( 10 ) ).c_str(),
      ( "axishist" + RandomString( 10 ) ).c_str(),
      10, func.GetXmin(), func.GetXmax() );
    axishist.SetStats( 0 );
    PadBase::PlotObj( axishist, "AXIS" );
    SetAxisFont();
  }

  const double xmax = func.GetXmax();
  const double xmin = func.GetXmin();

  // Getting common elements for graph generation
  for( unsigned i = 0; i < xsample; ++i ){
    const double xval = xmin + i* ( xmax-xmin )/( xsample -1 );
    const double yval = func.Eval( xval );

    x.push_back( xval );
    y.push_back( yval );
    yerr.push_back( 0 );
    ysample.push_back( {} );
    zero.push_back( 0 );
  }

  if( args.Has( ShowFitErr::CmdName ) ){
    const TFitResult* fit
      = (TFitResult*)args.Get( ShowFitErr::CmdName ).getObject( 0 );
    const double zval = args.Get( ShowFitErr::CmdName ).getDouble( 0 );

    const std::vector<double> bestfit_param = fit->Parameters();

    // Getting matrix for random parameter generation
    const TMatrixDSym cormatrix = fit->GetCorrelationMatrix();
    const TMatrixD tmatrix      = TDecompChol( cormatrix ).GetU();
    std::mt19937 gen;
    std::normal_distribution pdf( 0.0, zval );

    // Random sample for parameter space
    for( unsigned i = 0; i < psample; ++i ){
      TVectorD vec( tmatrix.GetNrows() );

      // Generating random variation using gaussian
      for( int j = 0; j < vec.GetNrows(); ++j ){
        vec[j] = pdf( gen );
      }

      // Tranforming random Gaussian matrix according to
      // correlation matrix
      vec = tmatrix * vec;

      // Shifting to central value of function.
      for( int j = 0; j < vec.GetNrows(); ++j ){
        func.SetParameter( j , vec[j] + bestfit_param[j] );
      }

      // Generate y-sample from randomly generated parameters.
      for( unsigned j = 0; j < xsample; ++j ){
        const double xval = x.at( j );
        ysample.at( j ).push_back( func.Eval( xval ) );
      }
    }

    // Calculating standard deviation
    for( unsigned i = 0; i < xsample; ++i ){
      yerr[i] = StdDev( ysample[i] );
    }

    TGraphErrors& graph = _frame.MakeObj<TGraphErrors>(  x.size(),
      x.data(), y.data(),
      zero.data(), yerr.data() );

    graph.SetName( RandomString(6).c_str() );
    return PlotGraph( graph, arglist );

  } else {
    TGraph& graph = _frame.MakeObj<TGraph>( x.size(), x.data(), y.data() );
    graph.SetName( RandomString(6).c_str() );
    return PlotGraph( graph, arglist );
  }
}


/**
 * Function for plotting all of the RooAbsData objects, the supported options
 * allowed include:
 * - *Any* RooCmdArg supported by the RooAbsData::plotOn function. These will
 *   take precedence to custom defined options.
 * - PlotType: Specifying the representation for the data object on the Pad.
 *   Any of the types valid for the PlotGraph functions would be valid. This
 *   options will only be used if the RooFit::DrawOption is not already
 *   specified. If neither PlotType or RooFit::DrawOption is specified, the
 *   scatter method would be used.
 *
 * - EntryText: String to add in the legend entry. If this options is not
 *   present, then this object will NOT appear in the generated legend. Notice
 *   that the attributes to display in the legend would be generated from the
 *   PlotType used.
 *
 * - TrackY: Whether or not the y-axis range should be adjusted according to
 *   the newly added graph. By default, only the maximum value would be used.
 *
 * Additional automation of options include suppressing the X error bars of the
 * generated graph if an equal binning is used for the data set and the
 * RooFit::XErrorSize is not specified. This is to align the plotting style
 * conventions within the CMS collaboration. Another change to the graphs is
 * that the y uncertainties in the zero bins are suppressed, again, to align
 * with the plotting style conventions.
 *
 * The function returns a reference to the generated TGraph object by the
 * underlying RooPlot object, The object would be owned by the RooPlot and be
 * destroyed when the Pad1D goes out of scope.
 */
TGraphAsymmErrors&
Pad1D::PlotData(
  RooAbsData&                   data,
  const std::vector<RooCmdArg>& args
  )
{
  static const RooCmdArg suppressxerror = RooFit::XErrorSize( 0 );// Default
  const RooArgContainer arglist( args );

  // Default parsing
  const bool addtopad     = !arglist.Has( "Invisible" );
  const RooCmdArg plotopt =
    arglist.Has( "DrawOption" ) ? arglist.Get( "DrawOption" ).getString( 0 ) :
    arglist.Has( PlotType::CmdName ) ? arglist.Get( PlotType::CmdName ) :
    PlotType( scatter );
  const RooCmdArg trkopt =
    arglist.Has( TrackY::CmdName ) ? arglist.Get( TrackY::CmdName ) :
    TrackY( TrackY::max );

  // Generating the requirements for actual RooFit PlotOn calls
  RooLinkedList oplist;

  for( const auto& arg : arglist ){
    if( arg.GetName() == PlotType::CmdName// Ignoring custom arguments
        || arg.GetName() == TrackY::CmdName
        || arg.GetName() == EntryText::CmdName ){ continue; }
    oplist.Add( arg.Clone() );
  }

  // Option for suppressing x error bars
  if( !arglist.Has( "Binning" ) ){
    // If no binning is specified, the frame will always use uniformed binning!
    oplist.Add( suppressxerror.Clone() );
  } else {
    const auto binopt = arglist.Get( "Binning" );
    if( binopt.getDouble( 0 ) != binopt.getDouble( 1 ) ){
      // Only double setting would be for uniformed binning
      oplist.Add( suppressxerror.Clone() );
    } else {
      if( binopt.getObject( 0 ) ){// Directly specifying binning
        auto binobj = dynamic_cast<const RooAbsBinning*>(
          binopt.getObject( 0 ) );
        if( binobj && binobj->isUniform() ){
          oplist.Add( suppressxerror.Clone() );
        }
      } else if( binopt.getString( 0 ) ){// Calling binning stored in variable
        const auto binname = binopt.getString( 0 );
        const auto plotvar = _frame.getPlotVar();
        if( plotvar->hasBinning( binname )
            && plotvar->getBinning( binname ).isUniform() ){
          oplist.Add( suppressxerror.Clone() );
        }
      }
    }
  }

  // Generating the object via RooFit
  TGraphAsymmErrors& ans = GenGraph( data, oplist );

  // Additional Fixes for RooFit generated objects
  for( int i = 0; i < ans.GetN(); ++i ){
    if( ans.GetY()[i] == 0 ){// Suppressing errors in zero bins
      ans.SetPointError( i, 0, 0, 0, 0 );
    }
  }

  if( _prevrangetype == rangetype::aut ){
    _prevrangetype = rangetype::hist;
  }

  if( addtopad ){
    PlotGraph( ans, plotopt, trkopt );
  }

  // Adding legend
  if( arglist.Has( EntryText::CmdName ) ){
    const std::string leg = arglist.Get( EntryText::CmdName ).getString( 0 );
    AddLegendEntry( ans, leg, plotopt );
  }

  return ans;
}

/**
 * Plotting of RooAbsPdf objects by generating the TGraphs using a RooPlot
 * object and plotting the graphs onto the TPad. The supported options are:
 *
 * - *Any* RooCmdArg supported by the RooAbsData::plotOn function. These will
 *   take precedence to custom defined options.
 *
 * - PlotType: Specifying the representation for the data object on the Pad.
 *   Any of the types valid for the PlotGraph functions would be valid. This
 *   options will only be used if the RooFit::DrawOption is not already
 *   specified. If neither PlotType or RooFit::DrawOption is specified, the
 *   scatter method would be used.
 *
 * - EntryText: String to add in the legend entry. If this options is not
 *   present, then this object will NOT appear in the generated legend. Notice
 *   that the attributes to display in the legend would be generated from the
 *   PlotType used.
 *
 * - TrackY: Whether or not the y-axis range should be adjusted according to
 *   the newly added graph. By default, RooPdf objects would not be used
 *   to adjust the axis range.
 *
 * Additional automation of options include additional generation for
 * RooFit::VisualizeError. The stock RooPlot generates are contour line for the
 * uncertainty range rather than a line with error, making styling of a PDF
 * with uncertainty rather tedious. This functions takes the generated TGraphs
 * by the RooPlot and recalculated a TGraph with uncertainty. The newly
 * calculated graph will be placed under the ownership of the RooPlot object.
 */
TGraph&
Pad1D::PlotPdf( RooAbsPdf& pdf, const std::vector<RooCmdArg>& arglist )
{
  const RooArgContainer args( arglist );
  const RooCmdArg visualerr =
    args.Has( "VisualizeError" ) ? args.Get( "VisualizeError" ) :
    RooCmdArg::none();
  const bool runfiterr   = args.Has( "VisualizeError" );
  const bool addtopad    = !args.Has( "Invisible" );
  const RooCmdArg pltopt =
    runfiterr ? PlotType( fittedfunc ) :
    args.Has( "DrawOption" ) ? args.Get( "DrawOption" ).getString( 0 ) :
    args.Has( PlotType::CmdName ) ? args.Get( PlotType::CmdName ) :
    PlotType( simplefunc );
  const RooCmdArg trkopt =
    args.Has( TrackY::CmdName ) ? args.Get( TrackY::CmdName ) :
    TrackY( TrackY::max );// Automatic

  RooLinkedList oplist;
  RooLinkedList oplist2;
  // second one for error part (RooLinkedList deletes objects when
  // getting parsed... so)

  for( const auto& arg : arglist ){
    if( arg.GetName() == PlotType::CmdName
        || arg.GetName() == TrackY::CmdName
        || arg.GetName() == EntryText::CmdName ){
      continue;
    }
    if( arg.GetName() != std::string( "VisualizeError" ) ){
      oplist.Add( arg.Clone() );
      oplist2.Add( arg.Clone() );
    } else {
      oplist2.Add( arg.Clone() );
    }
  }

  if( _prevrangetype == rangetype::aut ){ _prevrangetype = rangetype::hist; }

  TGraph* ans = nullptr;
  if( !runfiterr ){
    ans = &GenGraph( pdf, oplist );
    if( addtopad ){ PlotGraph( ans, pltopt, trkopt  ); }
  } else {
    TGraph& centralgraph = GenGraph( pdf, oplist );
    TGraph& errorgraph   = GenGraph( pdf, oplist2 );

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
    ans = &_frame.MakeObj<TGraphAsymmErrors>( fiterr.size() );
    unsigned i = 0;// index for looping

    for( const auto& fiterrval : fiterr ){
      const double x   = fiterrval.first;
      const double y   = centralgraph.Eval( x );
      const double ylo = fiterrval.second.first;
      const double yhi = fiterrval.second.second;
      ( (TGraphAsymmErrors*)ans )->SetPoint( i, x, y );
      ( (TGraphAsymmErrors*)ans )->SetPointError( i, 0, 0, y-ylo, yhi-y );
      ++i;
    }

    // Drawing the new graph
    if( addtopad ){
      PlotGraph( ans, pltopt, trkopt );
    }
  }


  // Adding legend
  if( args.Has( EntryText::CmdName ) ){
    const std::string leg = args.Get( EntryText::CmdName ).getString( 0 );
    AddLegendEntry( *ans, leg, pltopt );
  }

  return *ans;
}

TGraph&
Pad1D::GenGraph( RooAbsPdf& pdf, RooLinkedList& arglist )
{
  RooPlot* test = pdf.plotOn( &_frame, arglist );
  if( !test ){
    throw std::invalid_argument(
      "Bad argument list or object, plotting failed" );
  }
  return _frame.LastPlot<TGraph>();
}

TGraphAsymmErrors&
Pad1D::GenGraph( RooAbsData& data, RooLinkedList& arglist )
{
  RooPlot* test = data.plotOn( &_frame, arglist );
  if( !test ){
    throw std::invalid_argument(
      "Bad argument list or object, plotting failed" );
  }
  return _frame.LastPlot<TGraphAsymmErrors>();
}


}/* plt */

}/* usr  */
