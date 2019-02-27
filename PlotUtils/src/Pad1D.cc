/**
 * @file    Pad1D.cc
 * @brief   Implementation of the Pad1D member functions
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/Format.hpp"
#include "UserUtils/PlotUtils/interface/Pad1D.hpp"
#else
#include "UserUtils/Common/Format.hpp"
#include "UserUtils/PlotUtils/Pad1D.hpp"
#endif

#include <limits>

namespace usr  {

namespace plt {

/**
 * @brief Constructing a Pad1D using a @ROOT{RooRealVar}
 *
 * @details This will make the Pad1D instance properly spawn the _frame that is
 * capable for generating plotting objects for RooFit data objects.
 */
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

/**
 * @brief Construct a Pad1D with a limit x range.
 */
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

/**
 * @brief Construct a Pad1D with only dimension specifications
 *
 * This would the be most typical construction for the plotting of ROOT objects.
 * Note that the axis range would be determined by the object that is plotted
 * first into the Pad. So take care of plotting order if that is an issue..
 */
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

/**
 * @brief Additional routines after pad spawning
 *
 * In case a RooPlot is requested, ust that to draw the frame first to
 * avoid graphs messing up the plot range.
 */
void
Pad1D::InitDraw()
{
  // Early exit if no RooPlot is required;
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

/**
 * @brief additional steps before canvas saving.
 *
 * Additional steps:
 * 1. Redrawing axis to ensure nothing is covering the frame.
 * 2. Making the legend object, and ensuring all specificed legend entries
 *    are placed in the legend.
 */
void
Pad1D::Finalize()
{
  TPad::cd();
  RedrawAxis();
  MakeLegend();
  MakeLegend();
}

/**
 * Since all ownership is handled by _frame, do additional deletion class is
 * needed.
 */
Pad1D::~Pad1D()
{}

/**
 * Aside from calling the TPad::SetLogy() function, the AutoSetYRange is also
 * called to adjust the Y axis range appropriately.
 */
void
Pad1D::SetLogy( int x )
{
  TPad::SetLogy( x );
  AutoSetYRange();
}

void
Pad1D::SetLogx( int x )
{
  TPad::SetLogx( x );
}


}/* plt */

}/* usr  */

#undef AXISOBJ_ACCESS
