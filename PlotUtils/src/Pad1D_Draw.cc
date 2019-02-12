/**
 * @file    Pad1D_Draw.cc
 * @brief   Implementing the drawing functions that spawn new objects.
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/PlotUtils/interface/Pad1D.hpp"
#else
#include "UserUtils/PlotUtils/Pad1D.hpp"
#endif

#include <boost/format.hpp>


namespace usr  {

namespace plt  {

/**
 * @brief Drawing horizontal line
 *
 * Drawing a horizontal line across the pad at a fixed y value.
 * @param  y y value to plot the line
 * @param  c Color of the line using ROOT color indexing
 * @param  s Style of line using ROOT color indexing
 * @param  w Width of the line in pixels
 * @return   Return a reference to the created line for additional user styling.
 */
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

/**
 * @brief drawing vertical line
 *
 * Drawing a vertical line on the pad at a fixed x value.
 * @param  x x value to plot the line
 * @param  c Color of the line using ROOT color indexing
 * @param  s Style of line using ROOT color indexing
 * @param  w Width of the line in pixels
 * @return   Return a reference to the created line for additional user styling.
 */
TLine&
Pad1D::DrawVLine(
  const double  x,
  const Color_t c,
  const Style_t s,
  const Width_t w )
{
  TLine& line = _frame.MakeObj<TLine>(
    x,
    Yaxis().GetXmin(),
    x,
    Yaxis().GetXmax() );
  line.SetLineColor( c );
  line.SetLineStyle( s );
  line.SetLineWidth( w );
  PadBase::PlotObj( line );
  return line;
}

/**
 * @brief Drawing CMS label
 *
 * Drawing the CMS label inside the frame at the top left corner.
 * Additional styling will include:
 * - The font size of the label will be a "large" variant to the Canvas settings
 * - The Top left corner of the text would be set to 1.5 times the tick length.
 * - The man label (CMS) would be set to bold
 * - The second label (if defined, such as "Preliminary") would be set to
 *   italic.
 * @param tag  The secondary tag to be displayed below the CMS label font.
 * @param main The main tag to be plotted (default to CMS but could be changed
 *             if you are working for another experiment)
 */
void
Pad1D::DrawCMSLabel( const std::string& tag, const std::string& main )
{
  const float tmp     = FontSize();
  const float xmargin = 1.5 * Yaxis().GetTickLength();
  const float ymargin = 1.5 * Xaxis().GetTickLength();
  const float cursorx = GetLeftMargin() + xmargin;
  const float cursory = 1 - GetTopMargin() - ymargin;
  _latex.SetTextSize( Font().large() );

  SetTextCursor( cursorx, cursory, font::top_left );
  WriteLine( "#bf{"+main+"}" );
  if( tag != "" ){
    WriteLine( "#it{" + tag + "}" );
  }

  _latex.SetTextSize( tmp );
}


/**
 * @brief Drawing luminosity label.
 *
 * Writing the Luminosity label outside of the frame on the top-right corner.
 * This string in directly pass to the TLatex object, so all styling here would
 * be user defined. The font setting would be identical to that of the canvas
 * setting.
 * @param x the string to use as the luminosity label
 */
void
Pad1D::DrawLuminosity( const std::string& x )
{
  const float ymargin = 0.3*Xaxis().GetTickLength();
  const float cursorx = 1 - GetRightMargin();
  const float cursory = 1 - GetTopMargin() + ymargin;

  SetTextCursor( cursorx, cursory, font::bottom_right );
  WriteLine( x );
}

/**
 * @brief Drawing luminosity label.
 *
 * Writing the luminosity label in the standard format of
 * "XX.X fb^{-1} (13TeV)"
 * @param lumi The luminosity in units of fb-1
 */
void
Pad1D::DrawLuminosity( const double lumi )
{
  DrawLuminosity(
    boost::str( boost::format( "%.1f fb^{-1} (13 TeV)" ) % lumi )
    );
}

}/* plt  */

}/* usr  */
