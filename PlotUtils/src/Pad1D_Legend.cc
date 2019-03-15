/**
 * @file    Pad1D_Legend.cc
 * @brief   Implementation of legend generation for the Pad1D object.
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/PlotUtils/interface/Pad1D.hpp"
#else
#include "UserUtils/PlotUtils/Pad1D.hpp"
#endif

#include "TLegendEntry.h"

#include <boost/algorithm/string.hpp>

namespace usr  {

namespace plt  {

/**
 * @brief Legend creation with consistent font settings.
 */
void
Pad1D::_init_legend()
{
  _legend.SetTextFont( ParentCanvas().Font().fontface() );
  _legend.SetTextSize( ParentCanvas().Font().size() );
  _legend.SetBorderSize( 0 );
  _legend.SetFillColorAlpha( 0, 0 );
}


/**
 * @brief  Adding legend entry for a histogram object.
 *
 * Adding the a legend entry to the internal legend if the EntryText was used
 * for plotting a histogram. The PlotType argument used for plotting the
 * histogram would be used to determine what attributes should be used
 * in the legend:
 * - plottype::hist: Line and fill attributes
 * - plottype::histerr: Only the fill attribute (since we are using error boxes)
 * - pltotype::histstack: Only the fill attribute (avoid lines)
 * - plottype::scatter:  Point and Line attributes, whether the horizontal line
 *   would be used is also determined by the variable bin-width of the histgram.
 * - other/undetermined: all attributes would be used.
 *
 * @param hist    The histogram that the legend should link to.
 * @param title   The texted to be placed in the legend.
 * @param plotopt Options used for plotting histogram.
 */
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

  std::vector<std::string> lines;
  boost::split( lines, title, boost::is_any_of( "\n" ) );

  for( int i = lines.size()-1; i >= 0; i-- ){
    if( i > 0 ){
      _legstack.push( { nullptr, lines.at( i ), "" } );
    } else {
      _legstack.push( { &hist, lines.at( i ), legopt } );
    }
  }
}

/**
 * @ brief adding legend entry for graph object.
 *
 * Adding the a legend entry to the internal legend if the EntryText was used
 * for plotting a histogram. The PlotType argument used for plotting the
 * histogram would be used to determine what attributes should be used
 * in the legend:
 * - plotype::simplefunc: only the line attribute
 * - plotype::fittedfunc: line and fill attributes
 * - plottype::scatter: point and line attributes. whether the vertical or the
 *   horizontal line should be used is determined by the error bar in the graph.
 *   if the error is zero throughout the datapoints, the corresponding bars
 *   would not be added to the legend either.
 * - other/undetermined: all attributes would be used.
 *
 * @param graph   graph to be added to legend.
 * @param title   text for entry in legend.
 * @param plotopt plotting options used for graph object.
 */
void
Pad1D::AddLegendEntry(
  TGraph&            graph,
  const std::string& title,
  const RooCmdArg&   plotopt )
{
  const int plottype = plotopt.getInt( 0 );
  double x_width     = 0;
  double y_width     = 0;

  for( int i = 0; i < graph.GetN() && plottype == plottype::scatter; ++i ){
    x_width = graph.GetErrorXlow( i ) + graph.GetErrorXhigh( i );
    y_width = graph.GetErrorYlow( i ) + graph.GetErrorYhigh( i );
    // Early exit if both are not zero.
    if( x_width != 0 && y_width != 0 ){ break; }
  }

  const std::string erropt =
    x_width != 0 && y_width != 0 ? "LE" :
    x_width == 0 && y_width != 0 ? "E" :
    x_width != 0 && y_width == 0 ? "L" :
    "";

  const std::string legopt =
    plottype == plottype::simplefunc ? "L" :
    plottype == plottype::fittedfunc ? "LF" :
    plottype == plottype::scatter ? "P" + erropt :
    plotopt.getString( 0 ) ? "PLFE" :
    "";

  std::vector<std::string> lines;
  boost::split( lines, title, boost::is_any_of( "\n" ) );

  for( int i = lines.size()-1; i >= 0; i-- ){
    if( i > 0 ){
      _legstack.push( { nullptr, lines.at( i ), "" } );
    } else {
      _legstack.push( { &graph, lines.at( i ), legopt } );
    }
  }
}

/**
 * Generating the TLegend object at the inner top right of the pad.
 */
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

  TPad::cd();
  double width  = 0;
  double height = 0;

  for( const auto&& obj : *_legend.GetListOfPrimitives() ){
    // This should be absolute coordinates
    const double estwidth = EstimateLatexWidth(
      ( (TLegendEntry*)obj )->GetLabel() ) * FontSize() / AbsWidth();
    const double estheight = EstimateLatexHeight(
      ( (TLegendEntry*)obj )->GetLabel() ) * FontSize() / AbsHeight();
    // Gussing width based on character count alone
    width   = std::max( estwidth, width );
    height += std::max( 1.1* estheight, 1.4*RelLineHeight() );
  }

  width *= 1.1;// Relieving spacing a little
  width += 1.3*LineHeight() / AbsWidth();// Reserving space for legend icon boxes.

  const float xmax = 1 - GetRightMargin() - 1.2*Yaxis().GetTickLength();
  const float ymax = 1 - GetTopMargin() - 1.2*Xaxis().GetTickLength();
  const float xmin = xmax - width;
  const float ymin = ymax - height;
  _legend.SetX1NDC( xmin );
  _legend.SetX2NDC( xmax );
  _legend.SetY1NDC( ymin );
  _legend.SetY2NDC( ymax );

  if( !TPad::FindObject( &_legend ) ){
    PadBase::PlotObj( _legend );
    _legend.SetX1NDC( xmin );
    _legend.SetX2NDC( xmax );
    _legend.SetY1NDC( ymin );
    _legend.SetY2NDC( ymax );
  }
}

}/* plt  */

}/* usr  */
