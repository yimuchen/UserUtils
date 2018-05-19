/**
 * @file    Pad1D.cc
 * @brief   Implementation of the Pad1D member functions
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */
#include "UserUtils/Common/interface/Format.hpp"
#include "UserUtils/Common/interface/STLUtils/MinMax.hpp"
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"
#include "UserUtils/PlotUtils/interface/Pad1D.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <limits>

#include "TLegendEntry.h"

namespace usr  {

namespace plt {
/*-----------------------------------------------------------------------------
 *  Constructor and destructor
   --------------------------------------------------------------------------*/
Pad1D::Pad1D( const PadSize& size, const RangeByVar& range ) :
  PadBase( size ),
  _frame( range ),
  _workingstack( nullptr ),
  _datamax( 1 ),
  _datamin( 0.3 ),
  _prevrangetype( rangetype::aut )
{
  // Common setup for pads
  TPad::SetTicks( 1, 1 );
  _init_legend();
}

/*----------------------------------------------------------------------------*/

Pad1D::Pad1D( const PadSize& size, const double min, const double max ) :
  PadBase( size ),
  _frame( min, max ),
  _workingstack( nullptr ),
  _datamax( 1 ),
  _datamin( 0.3 ),
  _prevrangetype( rangetype::aut )
{
  // Common setup for pads
  TPad::SetTicks( 1, 1 );
  _init_legend();
}

/*----------------------------------------------------------------------------*/
Pad1D::Pad1D( const PadSize& size ) :
  PadBase( size ),
  _workingstack( nullptr ),
  _datamax( 1 ),
  _datamin( 0.3 ),
  _prevrangetype( rangetype::aut )
{
  // Common setup for pads
  TPad::SetTicks( 1, 1 );
  _init_legend();
}

/*----------------------------------------------------------------------------*/

void
Pad1D::InitDraw()
{
  // Early exist is no RooPlot is required;
  if( !_frame.AxisHistPtr() ){ return; }

  // Drawing the frame histogram for defining axis
  PadBase::PlotObj( _frame.AxisHist(), "AXIS" );
  // Setting up the name and title of RooPlot:
  SetAxisFont();

  if( _frame.getPlotVar() ){
    SetHistAxisTitles(
      _frame.getPlotVar()->GetTitle(),
      _frame.getPlotVar()->getUnit()
      );
  }
}

/*----------------------------------------------------------------------------*/

void
Pad1D::_init_legend()
{
  _legend.SetTextFont( ParentCanvas().FontFace() );
  _legend.SetTextSize( ParentCanvas().FontSize() );
  _legend.SetBorderSize( 0 );
  _legend.SetFillColorAlpha( 0, 0 );
}

/*----------------------------------------------------------------------------*/

void
Pad1D::Finalize()
{
  // Redrawing axis to ensure nothing covers the frame.
  TPad::cd();
  RedrawAxis();
  MakeLegend();
}

Pad1D::~Pad1D()
{}

/*-----------------------------------------------------------------------------
 *  Root Object plotting functions
   --------------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------------*/

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

/*-----------------------------------------------------------------------------
 *  RooFit plot functions
   --------------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------------*/

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

/*-----------------------------------------------------------------------------
 *  Additional Plotting functions
   --------------------------------------------------------------------------*/
TLine&
Pad1D::DrawHLine(
  const double  y,
  const Color_t c,
  const Style_t s,
  const Width_t w )
{
  TLine& line = _frame.MakeObj<TLine>( GetXaxisMin(), y, GetYaxisMin(), y );
  line.SetLineColor( c );
  line.SetLineStyle( s );
  line.SetLineWidth( w );
  PadBase::PlotObj( line );
  return line;
}

TLine&
Pad1D::DrawVLine(
  const double  x,
  const Color_t c,
  const Style_t s,
  const Width_t w )
{
  TLine& line = _frame.MakeObj<TLine>(
    x,
    std::numeric_limits<double>::min(),
    x,
    std::numeric_limits<double>::max() );
  line.SetLineColor( c );
  line.SetLineStyle( s );
  line.SetLineWidth( w );
  PadBase::PlotObj( line );
  return line;
}


/*-----------------------------------------------------------------------------
 *  Axis access functions
   --------------------------------------------------------------------------*/
static double unfound = std::numeric_limits<double>::quiet_NaN();

TObject*
Pad1D::GetAxisObject() const
{
  if( !GetListOfPrimitives() ){ return nullptr; }
  // Looping through contents in the TPad, similar to the function used in
  // TPad::RedrawFrame https://root.cern.ch/doc/master/
  // src_2TPad_8cxx_source.html#l05219

  TIter next( GetListOfPrimitives() );
  TObject* obj;

  while( ( obj = next() ) ){
    if( obj->InheritsFrom( TH1::Class() )
        || obj->InheritsFrom( TGraph::Class() ) ){
      return obj;
    }
  }

  return nullptr;
}

/*----------------------------------------------------------------------------*/

void
Pad1D::SetAxisFont()
{
  Xaxis().SetLabelFont( FontFace() );
  Xaxis().SetTitleFont( FontFace() );
  Xaxis().SetTitleSize( FontSize() );
  Xaxis().SetLabelSize( font::fontsize( font::mult_small() * FontSize() ) );

  Yaxis().SetLabelFont( FontFace() );
  Yaxis().SetTitleFont( FontFace() );
  Yaxis().SetTitleSize( FontSize() );
  Yaxis().SetLabelSize( font::fontsize( font::mult_small() * FontSize() ) );
}

/*----------------------------------------------------------------------------*/

#define AXISOBJ_ACCESS( ACTION, DEFAULT )                 \
  TObject* axisobj = GetAxisObject();                     \
  if( !axisobj ){                                         \
    return DEFAULT;                                       \
  } else if( axisobj->InheritsFrom( TH1::Class() ) ){     \
    return dynamic_cast<TH1*>( axisobj )->ACTION;         \
  } else if( axisobj->InheritsFrom( TGraph::Class() ) ){  \
    return dynamic_cast<TGraph*>( axisobj )->ACTION;      \
  } else if( axisobj->InheritsFrom( THStack::Class() ) ){ \
    return dynamic_cast<THStack*>( axisobj )->ACTION;     \
  } else {                                                \
    return DEFAULT;                                       \
  }

#define AXISOBJ_ADJUST( ACTION )                          \
  TObject* axisobj = GetAxisObject();                     \
  if( !axisobj ){                                         \
    return;                                               \
  } else if( axisobj->InheritsFrom( TH1::Class() ) ){     \
    return dynamic_cast<TH1*>( axisobj )->ACTION;         \
  } else if( axisobj->InheritsFrom( TGraph::Class() ) ){  \
    return dynamic_cast<TGraph*>( axisobj )->ACTION;      \
  } else if( axisobj->InheritsFrom( THStack::Class() ) ){ \
    return dynamic_cast<THStack*>( axisobj )->ACTION;     \
  }


TAxis*
Pad1D::GetXaxis() const { AXISOBJ_ACCESS( GetXaxis(), nullptr ); }

TAxis*
Pad1D::GetYaxis() const { AXISOBJ_ACCESS( GetYaxis(), nullptr ); }

TAxis&
Pad1D::Xaxis(){ return *GetXaxis(); }

TAxis&
Pad1D::Yaxis(){ return *GetYaxis(); }

const TAxis&
Pad1D::Xaxis() const { return *GetXaxis(); }

const TAxis&
Pad1D::Yaxis() const { return *GetYaxis(); }

double
Pad1D::GetDataMax() const { return _datamax; }

double
Pad1D::GetDataMin() const { return _datamin; }

double
Pad1D::GetXaxisMin() const { return Xaxis().GetXmin(); }

double
Pad1D::GetXaxisMax() const { return Xaxis().GetXmax(); }

double
Pad1D::GetYaxisMax() const { AXISOBJ_ACCESS( GetMaximum(), unfound ); }

double
Pad1D::GetYaxisMin() const { AXISOBJ_ACCESS( GetMinimum(), unfound ); }

void
Pad1D::SetYaxisMax( const double x ){ AXISOBJ_ADJUST( SetMaximum( x ) ); }

void
Pad1D::SetYaxisMin( const double x ){ AXISOBJ_ADJUST( SetMinimum( x ) ); }

void
Pad1D::AutoSetYRange( const rangetype type )
{
  rangetype optype =
    type != rangetype::aut ? type :
    _prevrangetype != rangetype::aut ? _prevrangetype :
    rangetype::aut;

  if( optype == rangetype::aut ){
    if( !PadBase::GetListOfPrimitives() ){
      optype = rangetype::aut;
    }
    TIter next( PadBase::GetListOfPrimitives() );
    TObject* iter;

    while( ( iter = next() ) ){
      if( ( iter->InheritsFrom( TH1::Class() )
            && ( (TH1*)iter )->GetXaxis() != _frame.GetXaxis() )
          || ( iter->InheritsFrom( THStack::Class() ) )
          // ignoring the RooPlot frame defining histogram
          // (using this weird syntax since this function is both)
           ){
        optype = rangetype::hist;
        break;
      } else if( iter->InheritsFrom( TGraph::Class() ) ){

        optype = rangetype::graph;
      }
    }
  }

  if( optype == rangetype::hist ){
    AutoSetYRangeHist();
  } else if( optype == rangetype::graph ){
    AutoSetYRangeGraph();
  } else if( optype == rangetype::pull ){
    AutoSetYRangePull();
  } else if( optype == rangetype::ratio ){
    AutoSetYRangeRatio();
  } else {
  }

  _prevrangetype = optype;
}

/*----------------------------------------------------------------------------*/

void
Pad1D::AutoSetYRangeHist()
{
  if( !GetLogy() ){// None log scale
    if( _datamin > 0 ){
      SetYaxisMin( _datamin );
      SetYaxisMax( _datamax * 1.25 );// Leaving to .2 ratio gap at top
    } else {
      const double diff = _datamax - _datamin;
      SetYaxisMax( _datamin + diff * 1.25 );
      SetYaxisMax( _datamax - diff * 1.25 );
    }
  } else {
    const double opmin = _datamin < 0 ? 1e-6 : _datamin;
    const double diff  = std::log10( _datamax / opmin );
    SetYaxisMax( opmin * std::pow( 10, diff * 1.25 ) );
    SetYaxisMin( _datamin );
    // Minium value is more restrictive
  }
}

/*----------------------------------------------------------------------------*/

void
Pad1D::AutoSetYRangeGraph()
{
  if( !GetLogy() ){// None log scale
    const double diff = _datamax - _datamin;
    SetYaxisMax( _datamin + diff * 1.2 );
    SetYaxisMin( _datamax - diff * 1.2 );
  } else {
    const double opmin = _datamin < 0 ? 1e-6 : _datamin;
    const double diff  = std::log10( _datamax / opmin );
    SetYaxisMax( opmin * std::pow( 10, diff * 1.2 ) );
    SetYaxisMin( _datamax / std::pow( 10, diff*1.1 ) );
    // Minium value is a little smaller
  }
}

/*----------------------------------------------------------------------------*/

void
Pad1D::AutoSetYRangeRatio()
{
  const double diff = std::min( std::max( _datamax - 1, 1-_datamin ), 0.6 );
  // rounding to the closest .1 to avoid weird axis tick labels
  const double opdiff = std::ceil( diff * 10 ) /10.;
  SetYaxisMax( 1 + opdiff *1.05 );
  SetYaxisMin( 1 - opdiff *1.05 );
}

/*----------------------------------------------------------------------------*/

void
Pad1D::AutoSetYRangePull()
{
  const double diff = std::min( std::max( _datamax, -_datamin ), 5.0 );
  // rounding to the closest .1 to avoid weird axis tick labels
  const double opdiff = std::ceil( diff * 10 ) /10.;
  SetYaxisMax( 1 + opdiff *1.1 );
  SetYaxisMin( 1 - opdiff *1.1 );
}

void
Pad1D::SetLogy( int x )
{
  TPad::SetLogy( x );
  AutoSetYRange();
}

/*-----------------------------------------------------------------------------
 *  Legend access title
   --------------------------------------------------------------------------*/
void
Pad1D::AddLegendEntry(
  TH1D&              hist,
  const std::string& title,
  const RooCmdArg&   plotopt )
{
  const std::string legopt =
    plotopt.getInt( 0 ) == plottype::hist ? "LF" :
    plotopt.getInt( 0 ) == plottype::histerr ? "F" :
    plotopt.getInt( 0 ) == plottype::histstack ? "F" :
    plotopt.getInt( 0 ) == plottype::histnewstack ? "F" :
    ( plotopt.getInt( 0 ) == plottype::scatter
      && hist.GetXaxis()->IsVariableBinSize() ) ? "PLE" :
    ( plotopt.getInt( 0 ) == plottype::scatter
      && !hist.GetXaxis()->IsVariableBinSize() ) ? "PE" :
    ( plotopt.getString( 0 ) ) ? "PLFE" :
    "";
  if( legopt != "" ){
    _legstack.push( {&hist, title, legopt} );
  }
}

void
Pad1D::AddLegendEntry(
  TGraph&            graph,
  const std::string& title,
  const RooCmdArg&   plotopt )
{
  bool isfixedwidth = ( graph.GetErrorXlow( 0 ) == graph.GetErrorXhigh( 0 ) );
  double prevxerr   = graph.GetErrorXlow( 0 );

  for( int i = 1; i < graph.GetN() && isfixedwidth; ++i ){
    isfixedwidth = ( graph.GetErrorXlow( i ) == graph.GetErrorXhigh( i ) );
    isfixedwidth = ( graph.GetErrorXlow( i ) == prevxerr );
  }

  const std::string legopt =
    plotopt.getInt( 0 ) == plottype::simplefunc ? "L" :
    plotopt.getInt( 0 ) == plottype::fittedfunc ? "LF" :
    plotopt.getInt( 0 ) == plottype::scatter && isfixedwidth ? "PE" :
    plotopt.getInt( 0 ) == plottype::scatter && !isfixedwidth ? "PLE" :
    plotopt.getString( 0 ) ? "PLFE" :
    "";

  if( legopt != "" ){
    _legstack.push( {&graph, title, legopt} );
  }
}

void
Pad1D::MakeLegend()
{
  // Flushing the contents in the stack.
  while( !_legstack.empty() ){
    const auto& ent = _legstack.top();
    _legend.AddEntry( ent.obj, ent.entry.c_str(), ent.legopt.c_str() );
    _legstack.pop();
  }

  // Early exit if Legend wasn't requested
  if( !_legend.GetListOfPrimitives() ){ return; }
  if( !_legend.GetListOfPrimitives()->GetEntries() ){ return; }

  float width  = 0;
  float height = LineHeight() * _legend.GetListOfPrimitives()->GetEntries();

  TPad::cd();

  for( const auto&& obj : *_legend.GetListOfPrimitives() ){
    const char* label = ( (TLegendEntry*)obj )->GetLabel();
    TLatex* textmp    = new TLatex( 0, 0, label );
    textmp->SetTextFont( FontFace() );
    textmp->SetTextSize( FontSize() );
    width = std::max( int(width), int(textmp->GetXsize() * AbsWidth() ) );
    delete textmp;
  }

  width *= 1.2;// Releiving spacing a little
  width += 1.5 * LineHeight();// Adding box for legend icon.

  // For whatever reason, the y corordinates of the TLegend counts from the
  // top.... Don't ask. Just ROOT things
  const float xmax =
    ( 1 - GetRightMargin() - 1.2*Yaxis().GetTickLength() ) * AbsWidth();
  const float ymax =
    ( GetTopMargin() + 1.2*Xaxis().GetTickLength() ) * AbsHeight();
  _legend.SetBBoxX2( xmax );
  _legend.SetBBoxY2( ymax + height );
  _legend.SetBBoxX1( xmax - width );
  _legend.SetBBoxY1( ymax );

  if( !TPad::FindObject( &_legend ) ){ PadBase::PlotObj( _legend ); }
}


/*----------------------------------------------------------------------------*/

const double Pad1D::autobinwidth     = -1;
const double Pad1D::forcevarbinwidth = std::numeric_limits<double>::quiet_NaN();

void
Pad1D::SetAxisTitle(
  TAxis&             x,
  const std::string& title,
  const std::string& unit
  )
{
  const std::string optitle =
    unit == "" ? title : title + " [" + boost::trim_copy( unit ) +  "]";

  x.SetTitle( optitle.c_str() );
}
/*----------------------------------------------------------------------------*/

void
Pad1D::SetHistAxisTitles(
  const std::string& title,
  const std::string& unit,
  const double       forcebinwidth )
{
  if( !GetAxisObject() ){
    std::cout << "Error! Axis object doesn't exist yet!" << std::endl;
    return;
  }
  // X axis is straight forwards
  SetAxisTitle( Xaxis(), title, unit );

  // y axis part is more complicated.
  boost::format uniwidthfmt( "Events/ %s" );
  boost::format varwidthfmt( "Events" );

  const std::string binwidthstr
    = forcebinwidth != autobinwidth ? forcebinwidth :
      usr::fmt::base::decimal( Xaxis().GetBinWidth( 0 ), -2 );

  // At most 2 digits after decimal point, stripping trailing zeros
  const std::string den
    = binwidthstr == "1" && unit == "" ? "" :
      boost::str( boost::format( "[ %s %s ]" )%binwidthstr%unit );

  const std::string ytitle
    = Xaxis().IsVariableBinSize()       ? boost::str( varwidthfmt ) :
      forcebinwidth == forcevarbinwidth ? boost::str( varwidthfmt ) :
      den == ""                         ? boost::str( varwidthfmt ) :
      boost::str( uniwidthfmt % den );

  SetAxisTitle( Yaxis(), ytitle );
}

/*-----------------------------------------------------------------------------
 *  Additional Latex options
   --------------------------------------------------------------------------*/
void
Pad1D::DrawCMSLabel( const std::string& tag, const std::string& main )
{
  const float tmp     = FontSize();
  const float xmargin = 1.5 * Yaxis().GetTickLength();
  const float ymargin = 1.5 * Xaxis().GetTickLength();
  const float cursorx = GetLeftMargin() + xmargin;
  const float cursory = 1 - GetTopMargin() - ymargin;
  _latex.SetTextSize( FontSize() * font::mult_large() );

  SetTextCursor( cursorx, cursory, font::top_left );
  WriteLine( "#bf{"+main+"}" );
  if( tag != "" ){
    WriteLine( "#it{" + tag + "}" );
  }

  _latex.SetTextSize( tmp );
}

/*----------------------------------------------------------------------------*/

void
Pad1D::DrawLuminosity( const double lumi )
{
  DrawLuminosity(
    boost::str( boost::format( "%.1f fb^{-1} (13 TeV)" ) % lumi )
    );
}

/*----------------------------------------------------------------------------*/

void
Pad1D::DrawLuminosity( const std::string& x )
{
  const float ymargin = 0.3*Xaxis().GetTickLength();
  const float cursorx = 1 - GetRightMargin();
  const float cursory = 1 - GetTopMargin() + ymargin;

  SetTextCursor( cursorx, cursory, font::bottom_right );
  WriteLine( x );
}

}/* plt */

}/* usr  */

#undef AXISOBJ_ACCESS
