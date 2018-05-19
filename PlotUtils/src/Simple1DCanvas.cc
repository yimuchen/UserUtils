/**
 * @file    Simple1DCanvas.hpp
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */

#include "UserUtils/PlotUtils/interface/Simple1DCanvas.hpp"

namespace usr  {

namespace plt  {

/*-----------------------------------------------------------------------------
 *  Constructor and destructor
   --------------------------------------------------------------------------*/
length_t Simple1DCanvas::default_width  = 0.8 * len::a4textwidth_default();
length_t Simple1DCanvas::default_height = 0.8 * len::a4textwidth_default();

Simple1DCanvas::Simple1DCanvas(
  const length_t width,
  const length_t height,
  const FontSet& fontset
  ) : Canvas( width, height, fontset )
{
  Add<Pad1D>( PadSize( 0, 0, 1, 1 ) );
  _init_margin();
}


Simple1DCanvas::Simple1DCanvas(
  const RangeByVar& range,
  const length_t    width,
  const length_t    height,
  const FontSet&    fontset
  ) : Canvas( width, height, fontset )
{
  Add<Pad1D>( PadSize( 0, 0, 1, 1 ), range );
  _init_margin();
}

Simple1DCanvas::~Simple1DCanvas(){}

// First defining the default margins on the object:
// 3  lineheight for the left and bottom margin
// 1.5 lineheight for the top canvas
// Right is automatically assigned such that the framebox would be the
// same ratio as the canvas if such a value is greater than 0.3 lineheight.
// Setting to 0.3 line height otherwise.
void
Simple1DCanvas::_init_margin()
{
  Pad().SetTopMargin( 1.5*LineHeight()/Height() );
  Pad().SetLeftMargin( 3.5*LineHeight()/Width() );
  Pad().SetBottomMargin( 2.5*LineHeight()/Height() );
  Pad().SetRightMargin( std::max(
      ( 4.0*LineHeight()/Height() - ( 3.5*LineHeight()/Width() ) ),
      ( 0.3*LineHeight()/Width() ) )
    );
}

}/* plt  */

}/* usr  */
