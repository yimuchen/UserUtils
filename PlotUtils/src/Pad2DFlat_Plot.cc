#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/Maths.hpp"
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"
#include "UserUtils/PlotUtils/interface/Pad2DFlat.hpp"
#include "UserUtils/PlotUtils/interface/PlotCommon.hpp"
#else
#include "UserUtils/Common/Maths.hpp"
#include "UserUtils/Common/STLUtils/StringUtils.hpp"
#include "UserUtils/PlotUtils/Pad2DFlat.hpp"
#include "UserUtils/PlotUtils/PlotCommon.hpp"
#endif

#include "CmdSetAttr.hpp"
#include "TColor.h"
#include "TGraphErrors.h"
#include "TLegendEntry.h"
#include "TStyle.h"

namespace usr
{

namespace plt
{

/**
 * Plotting a two dimensional histogram support the following options:
 *
 * - Plot2DF: Defining how the the data should be represented. The following
 *   types are supported.
 *   - `plot2df::heat`: Heat map, corresponds to the `"COLZ"` options for TH2
 *     plotting in @ROOT
 *   - `plot2df::cont` Plotting the contour lines, using the "CONT3" options for
 *      TH2 plotting
 *   - `plot2df::heatcont`: Plotting both the heat map and contour lines.
 *
 * - EntryText: Adding text for display in the legend. Currently only the `cont`
 *   and `heatcont` will add a legend entry with the line styling displayed. The
 *   `heat` option assumes that this object will be used for the z axis colour
 *   pallet, so it will only add a text entry in the legend without a display.
 */
TH2D&
Pad2DFlat::PlotHist( TH2D& hist, const std::vector<RooCmdArg>& arglist )
{
  const RooArgContainer args( arglist, { Plot2DF( plot2df::heat )  } );

  const int opt = args.GetInt( "Plot2DF" );

  const std::string legopt = opt == plot2df::heat ?
                             "" :
                             opt == plot2df::heatcont ?
                             "L" :
                             opt == plot2df::cont ?
                             "L" :
                             opt == plot2df::density ?
                             "PF"  :
                             "PFLE";

  for( const auto&& func : *( hist.GetListOfFunctions() ) ){
    func->SetBit( TF1::kNotDraw, true );
  }

  // Forcing stuff to not be displayed
  hist.SetTitle( "" );
  hist.SetStats( 0 );

  // Parsing plotting flag
  switch( opt ){
  case  plot2df::heat:
    PlotObj( hist, "COLZ" );
    SetAxisFont();
    break;
  case plot2df::heattext:
    PlotObj( hist, "COLZ TEXT" );
    SetAxisFont();
    break;
  case   plot2df::heatcont:
    PlotObj( hist, "COLZ" );
    SetAxisFont();
    PlotObj( hist, "CONT3 SAME" );
    break;
  case  plot2df::cont:
    PlotObj( hist, "CONT3 SAME" );
    SetAxisFont();
    break;
  case plot2df::box:
    PlotObj( hist, "BOX SAME" );
    break;
  case plot2df::density:
    PlotObj( hist, "SCAT SAME" );
    break;
  case plot2df_dummy:
    PlotObj( hist, ( args.GetStr( "Plot2DF" )+" SAME" ).c_str() );
    break;

  default:
    std::cerr << "Skipping over invalid value" << std::endl;
    break;
  }

  if( args.Has( "EntryText" ) ){
    _legend.AddEntry( &hist,
                      args.Get( "EntryText" ).getString(
                        0 ),
                      legopt.c_str() );
  }

  SetLineAttr( hist, args );
  SetFillAttr( hist, args );
  SetMarkAttr( hist, args );

  return hist;
}


/**
 * Plotting a two dimensional graph support the following options:
 *
 * - Plot2DF: Defining how the the data should be represented. The following
 *   types are supported.
 *   - `plot2df::heat`: Heat map, corresponds to the "CONT4" options for
 *     TGraph2D plotting in @ROOT
 *   - `plot2df::cont` Plotting the contour lines, using the "CONT3" options for
 *      TH2 plotting
 *   - `plot2df::heatcont`: Plotting both the heat map and contour lines.
 *
 * - EntryText: Adding text for display in the legend. Currently only the `cont`
 *   and `heatcont` will add a legend entry with the line styling displayed. The
 *   `heat` option assumes that this object will be used for the z axis colour
 *   pallet, so it will only add a text entry in the legend without a display.
 * @brief
 */
TGraph2D&
Pad2DFlat::PlotGraph( TGraph2D& graph, const std::vector<RooCmdArg>& arglist )
{
  const RooArgContainer args( arglist, { Plot2DF( plot2df::heat )  } );

  const int opt = args.GetInt( "Plot2DF" );

  const std::string legopt = opt == plot2df::heat ?
                             "" :
                             opt == plot2df::heatcont ?
                             "L" :
                             opt == plot2df::cont ?
                             "L" :
                             opt == plot2df::box ?
                             "F" :
                             "PFLE";

  if( !GetAxisObject() ){
    auto& axishist = MakeObj<TH2D>(
      ( "axishist"+RandomString( 12 ) ).c_str(),
      "",
      10,
      graph.GetXminE(),
      graph.GetXmaxE(),
      10,
      graph.GetYminE(),
      graph.GetYmaxE());
    axishist.SetStats( 0 );
    PadBase::PlotObj( axishist, "AXIS" );
    SetAxisFont();
  }

  for( const auto&& func : *( graph.GetListOfFunctions() ) ){
    func->SetBit( TF1::kNotDraw, true );
  }

  switch( opt ){
  case plot2df::heat:
    PlotObj( graph, "CONT4Z SAME" );
    break;
  case  plot2df::heatcont:
    PlotObj( graph, "CONT4Z SAME" );
    PlotObj( graph, "CONT3 SAME" );
    break;
  case plot2df::cont:
    PlotObj( graph, "CONT3 SAME" );
    break;
  case plot2df_dummy:
    PlotObj( graph, args.GetStr( "Plot2DF" ).c_str() );
    break;
  default:
    std::cerr << "Skipping over invalid value" << std::endl;
    break;
  }

  if( args.Has( "EntryText" ) ){
    _legend.AddEntry( &graph,
                      args.Get( "EntryText" ).getString(
                        0 ),
                      legopt.c_str() );
  }

  SetLineAttr( graph, args );
  SetFillAttr( graph, args );
  SetMarkAttr( graph, args );

  return graph;
}


/**
 * Plotting of a 2 dimensional graph is done by generating a TGraph2D object via
 * the sampling the function. All other plotting functionalities are identical
 * to the the PlotGraph function.
 */
TGraph2D&
Pad2DFlat::PlotFunc( TF2& func, const std::vector<RooCmdArg>& arglist )
{
  static const unsigned sep = 300;

  const double xmin = func.GetXmin();
  const double xmax = func.GetXmax();
  const double ymin = func.GetYmin();
  const double ymax = func.GetYmax();
  const double dx   = ( xmax-xmin ) / sep;
  const double dy   = ( ymax-ymin ) / sep;

  std::vector<double> x;
  std::vector<double> y;
  std::vector<double> z;

  for( unsigned i = 0; i < sep; ++i ){
    for( unsigned j = 0; j < sep; ++j ){
      x.push_back( xmin+( i+0.5 ) * dx );
      y.push_back( ymin+( j+0.5 ) * dy );
      z.push_back( func.Eval( x.back(), y.back() ) );
    }
  }

  TGraph2D& graph = MakeObj<TGraph2D>( x.size(), x.data(), y.data(), z.data() );

  graph.SetName( usr::RandomString( 12 ).c_str() );

  return PlotGraph( graph, arglist );
}


/**
 * Plotting a regular 1D graph object (TGraph as oppose to TGraph2D) object
 * supports the following commands:
 * - PlotType: specifying how the data should be represented. The supported plot
 *   types are:
 *   - `plottype::scatter`: Scatter point with error bars (no end ticks)
 *   - `plottype::simplefunc`: Simple polyline joining the points.
 * - EntryText: Adding text into the legend.
 */
TGraph&
Pad2DFlat::Plot1DGraph( TGraph& graph, const std::vector<RooCmdArg>& arglist )
{
  const RooArgContainer args( arglist, {PlotType( plottype::scatter ) } );

  const int opt = args.GetInt( "PlotType" );

  std::string legopt = "PLE";

  // Parsing plotting flag
  if( opt == plottype::scatter ){
    PadBase::PlotObj( graph, "PZ0" );
  } else if( opt == plottype::simplefunc ){
    PadBase::PlotObj( graph, "L" );
    legopt = "L";
  } else if( opt == plottype_dummy ){
    // Special case for raw options parsing. (Must remove axis and add string)
    PadBase::PlotObj( graph, args.GetStr( "PlotType" ).c_str() );
  } else {// Skipping over stuff
    std::cerr << "Skipping over invalid value" << std::endl;
  }

  // Resetting underlying histogram range:
  graph.GetHistogram()->SetMaximum( usr::plt::GetYmax( graph ) );
  graph.GetHistogram()->SetMinimum( usr::plt::GetYmin( graph ) );

  if( args.Has( "EntryText" ) ){
    _legend.AddEntry( &graph,
                      args.Get( "EntryText" ).getString(
                        0 ),
                      legopt.c_str() );
  }

  SetLineAttr( graph, args );
  SetFillAttr( graph, args );
  SetMarkAttr( graph, args );

  return graph;
}


/**
 * @details Given a TGraph2D, the z coordinates will be used to modify the
 * color of the plots. As the TGraph2D PCOL plotting methods will for the
 * creation of a 3D plot, we will not be using that method. Rather we will be
 * grouping the creating a list of TGraphs each of their colors assigned
 * individually. As this method is potentially memory intensive, users are
 * advised to not use this method for very large data sets.
 *
 * Grouping of the z values is done by getting the maximum and minimum z values
 * of the TGraph2D data points (errors are omitted), then binning according to
 * the number of color values available in the current color pallet (typically
 * 255), these values of then uses as the edge values for separating the bin
 * values.
 *
 *
 */
TList&
Pad2DFlat::PlotColGraph( TGraph2D& g, const std::vector<RooCmdArg>& arglist )
{
  const RooArgContainer args( arglist,
                              { MarkerStyle( sty::mkrcircle ),
                                MarkerSize( 0.2 )} );

  // Setting up the data points for
  const auto                palette = TColor::GetPalette();
  const unsigned            n_color = palette.GetSize();
  const double              zmin    = g.GetZmin();
  const double              zmax    = g.GetZmax();
  const std::vector<double> edges   = LinSpace( zmin, zmax, n_color );

  // Data for grouping
  std::vector<std::vector<double> > x_group( n_color, std::vector<double>() );
  std::vector<std::vector<double> > ex_group( n_color, std::vector<double>() );
  std::vector<std::vector<double> > y_group( n_color, std::vector<double>());
  std::vector<std::vector<double> > ey_group( n_color, std::vector<double>() );

  // Looping over original data set and grouping.
  for( int i = 0 ; i < g.GetN(); ++i ){
    const double   z   = g.GetZ()[i];
    const unsigned bin =
      std::lower_bound( edges.begin(), edges.end(), z )-edges.begin();
    x_group[bin].push_back( g.GetX()[i] );
    y_group[bin].push_back( g.GetY()[i] );
    if( g.GetEX() ){

      ex_group[bin].push_back( g.GetEX()[i] );
      ey_group[bin].push_back( g.GetEY()[i] );
    } else {
      ex_group[bin].push_back( 0 );
      ey_group[bin].push_back( 0 );
    }
  }

  // Before plotting anything else, we will need to setup to axis object:
  if( !GetAxisObject() ){
    TH2D& axishist = MakeObj<TH2D>(
      ( "HISTAXIS"+RandomString( 6 ) ).c_str(),
      "",
      2,
      g.GetXminE(),
      g.GetXmaxE(),
      2,
      g.GetYminE(),
      g.GetYmaxE() );
    axishist.Reset();
    axishist.SetStats( 0 );
    axishist.Fill( g.GetXmaxE()+10, g.GetYminE()+10, g.GetZmax() );
    PlotObj( axishist, "COLZ" );
    axishist.SetMaximum( g.GetZmax() );
    axishist.SetMinimum( g.GetZmin() );
    this->SetAxisFont();
    if( args.Has( "EntryText" ) ){
      _legend.AddEntry( &axishist,
                        args.Get( "EntryText" ).getString(
                          0 ),
                        "" );
    }
  }

  // Looping over the non-trivial grouping results and plotting
  TList& ans = MakeObj<TList>();

  for( unsigned i = 0 ; i < n_color; ++i ){
    if( x_group[i].size() == 0 ){ continue; }
    TGraphErrors* sg = new TGraphErrors(
      x_group[i].size(),
      x_group[i].data(),
      y_group[i].data(),
      ex_group[i].data(),
      ey_group[i].data() );

    std::vector<RooCmdArg> single_args;
    for( const auto a : args ){
      if( a.GetName() == std::string( "EntryText" ) ){ continue; }
      single_args.push_back( a );
    }
    single_args.push_back( PlotType( scatter ));
    single_args.push_back( LineColor( palette[i] ));
    single_args.push_back( MarkerColor( palette[i] ));

    Plot1DGraph( sg, single_args );

    ans.Add( sg );
  }

  return ans;
}


/**
 * In 2D plotting, we are assuming the important stuff is being plotted first:
 * Important takes up more area (with heat plots, for example), so we will not
 * need to reverse the legend entry as with the Pad1D objects. The TLegend will
 * be generated on the left side of the frame. The size will be adjusted to have
 * everything fit snugly in the legend.
 */
void
Pad2DFlat::MakeLegend()
{
  // Early exit if Legend wasn't requested
  if( !_legend.GetListOfPrimitives() ){ return; }
  if( !_legend.GetListOfPrimitives()->GetEntries() ){ return; }
  TPad_().cd();
  float width  = 0;
  float height = 1.2 * LineHeight()
                 * _legend.GetListOfPrimitives()->GetEntries();

  for( const auto&& obj : *_legend.GetListOfPrimitives() ){
    const char*label  = ( (TLegendEntry*)obj )->GetLabel();
    TLatex*    textmp = new TLatex( 0, 0, label );
    textmp->SetTextFont( FontFace() );
    textmp->SetTextSize( FontSize() );
    width =
      std::max( int(width), int(1.3 * textmp->GetXsize() * AbsWidth()+5) );
    delete textmp;
  }

  width *= 1.1;// Relieving spacing a little
  width += 1.0 * LineHeight();// Reserving space for legend icon boxes.

  const float xmin = 0.5 * float(LineHeight() ) / AbsWidth();
  const float ymax = 1-GetTopMargin();
  const float xmax = GetLeftMargin()-5 * float(LineHeight() ) / AbsWidth();
  const float ymin = ymax-height / AbsHeight();
  _legend.SetX1NDC( xmin );
  _legend.SetX2NDC( xmax );
  _legend.SetY1NDC( ymin );
  _legend.SetY2NDC( ymax );

  if( !TPad_().FindObject( &_legend ) ){
    PadBase::PlotObj( _legend );
  }
}

}/** plt */

}/* usr */
