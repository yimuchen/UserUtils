/**
 * @file    Pad1D_Plot.cc
 * @brief   Implementing the data plotting functions.
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"
#include "UserUtils/PlotUtils/interface/Pad1D.hpp"
#else
#include "UserUtils/Common/STLUtils/StringUtils.hpp"
#include "UserUtils/PlotUtils/Pad1D.hpp"
#endif

namespace usr  {

namespace plt {

/**
 * Plotting 1D histograms accepts three types of options:
 * - PlotType: Defining how the binned data should be represented on the pad,
 *   following types are supported
 *   - plottype::hist (default) - standard square curve/block diagram for
 *     displaying a diagram, equivalent to the `"HIST"` options provided in the
 *     TH1 object.
 *   - plottype::scatter - Plotting points with error bars. Complying to the
 *     CMS plotting convention with the horizonal error bar being suppressed for
 *     fixed bin-width datas.
 *   - plottype::histstack - Stacking this histogram into the histogram stack
 *     on the Pad. If the stack doesn't already exist, a new stack is created.
 *     Note that all histograms in the stack will be plotted with the hist style
 *     and cannot be changed. Notice that the stack must be plotted
 *     consecutively, meaning that any non histstack options would cause the
 *     stack to be finalised and plotted onto the Pad.
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
 * - TrackY: Whether or not the y-axis range should be adjusted according to
 *   the newly added histogram. By default, if only graphs exists in the pad
 *   (excluding the axis object), both the min and max values in the graph
 *   would be used; otherwise none of the values would be used.
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
      args.Get( PlotType::CmdName ).getString( 0 ) ? plottype::dummy_start :
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
    TH1D& axisobj = _frame.MakeObj<TH1D>( obj );// MUST ust a clone
    // otherwise messes with THStack
    axisobj.Reset();
    PlotObj( axisobj, "AXIS" );
    this->SetAxisFont();
  }

  // Flushing the _working stack if hist is nolonger used
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

  } else if( opt == plottype::dummy_start && optraw != "" ){
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
      opt == plottype::dummy_start ? PlotType( optraw ) :
      PlotType( opt );
    const std::string leg = args.Get( EntryText::CmdName ).getString( 0 );
    AddLegendEntry( obj, leg, plotopt );
  }

  return obj;
}

/**
 * Plotting of the TGraph object has the following supporting options:
 * - PlotType: Defining how the data should be represented on the Pad. The
 *   supported types are:
 *   - plottype::simplefunc(default): single polyline joining the data points.
 *     This is used if the graph representes a function sample.
 *   - plottype::fittedfunc: a polyline joining the data points with the Y error
 *     represented with a shaded region. This is used if the graph represents
 *     a function sample with additional sampling for fitting uncertainties
 *   - plottype::scatter: plotting the data as data points with error bars.
 *     Unlike the histograms, this would not attempt to adjust the x error bars.
 *
 * - EntryText: String to add in the legend entry. If this options is not
 *   present, then this object will NOT appear in the generated legend. Notice
 *   that the attributes to display in the legend would be generated from the
 *   PlotType used.
 *
 * - TrackY: Whether or not the y-axis range should be adjusted according to
 *   the newly added graph. By default, only the maximum value of the
 *   histogram will be used to asjust the y-axis range.
 */
TGraph&
Pad1D::PlotGraph( TGraph& obj, const std::vector<RooCmdArg>& args )
{
  obj.SetTitle( "" );// Forcing clear title. This should be handled by Canvas.

  // Getting the flags
  const RooArgContainer arglist( args );
  const int opt =
    !arglist.Has( PlotType::CmdName ) ? plottype::simplefunc :
    arglist.Get( PlotType::CmdName ).getString( 0 ) ? plottype::dummy_start :
    arglist.Get( PlotType::CmdName ).getInt( 0 );
  const int trky
    = !arglist.Has( "TrackY" ) ? TrackY::aut : // more complex
      arglist.Get( "TrackY" ).getInt( 0 );// parsing latter
  std::string optraw =
    !arglist.Has( PlotType::CmdName ) ?  "" :
    !arglist.Get( PlotType::CmdName ).getString( 0 ) ? "" :
    arglist.Get( PlotType::CmdName ).getString( 0 );

  // Issue as option 'A' cannot be used singularly
  const std::string aopt = GetAxisObject() ? "" : "A";

  if( opt == plottype::simplefunc ){
    PadBase::PlotObj( obj, ( aopt+"L" ).c_str() );
  } else if( opt == plottype::fittedfunc ){
    PadBase::PlotObj( obj, ( aopt+ "3" ).c_str() );
    // Draw Error with fill region and then
    PadBase::PlotObj( obj, ( aopt+"LX" ).c_str() );
    // Draw the central line.all errors disabled.
  } else if( opt == plottype::scatter ){
    // Point, no error bar end ticks, and show error bar for points
    // outside range.
    PadBase::PlotObj( obj, ( aopt+"PZ0" ).c_str() );
  } else if( opt == plottype::dummy_start && optraw != "" ){
    ToUpper( optraw );
    StripSubstring( optraw, "A" );
    PlotObj( obj, ( aopt + optraw ).c_str() );
  } else {
    std::cerr << "Skipping over invalid value" << std::endl;
  }

  if( aopt == "A" ){
    SetAxisFont();
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
      opt == plottype::dummy_start ? PlotType( optraw ) :
      PlotType( opt );
    const std::string leg = arglist.Get( EntryText::CmdName ).getString( 0 );
    AddLegendEntry( obj, leg, plotopt );
  }

  return obj;
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
  TGraphAsymmErrors& ans = GenRoofitGraph<TGraphAsymmErrors>( data, oplist );

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
 * by the RooPlot and reclaculated a TGraph with uncertainty. The newly
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
    ans = &GenRoofitGraph<TGraph>( pdf, oplist );
    if( addtopad ){ PlotGraph( ans, pltopt, trkopt  ); }
  } else {
    TGraph& centralgraph = GenRoofitGraph<TGraph>( pdf, oplist );
    TGraph& errorgraph   = GenRoofitGraph<TGraph>( pdf, oplist2 );

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

}/* plt */

}/* usr  */
