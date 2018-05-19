/**
 * @file    Simple1DCanvas.hpp
 * @brief   A canvas with single 1D pad. Good for fast plotting!
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */
#ifndef USERUTILS_PLOTUTILS_SIMPLE1DCANVAS_HPP
#define USERUTILS_PLOTUTILS_SIMPLE1DCANVAS_HPP

#include "UserUtils/PlotUtils/interface/Canvas.hpp"
#include "UserUtils/PlotUtils/interface/Pad1D.hpp"

namespace usr  {

namespace plt  {

class Simple1DCanvas : public Canvas
{
public:
  static length_t default_width;
  static length_t default_height;

  Simple1DCanvas(
    const RangeByVar&,
    const length_t width  = default_width,
    const length_t height = default_height,
    const FontSet& = FullPageFont
    );

  Simple1DCanvas(
    const length_t width  = default_width,
    const length_t height = default_height,
    const FontSet& = FullPageFont
    );

  virtual
  ~Simple1DCanvas ();

  Pad1D&
  Pad(){ return GetPad<Pad1D>( 0 ); }

  // Overloading the internal Pad1D functions to look like directly
  // 'plotting on this canvas'. Using inline variadic template for easier
  // future proofing. (Must explicitly state the first argument, otherwise
  // funny stuff happens with the variadic interface and stuff doesn't get
  // plotted properly)
#define PASSTHROUGH_PLOTFUNC( FUNC_NAME, PLOT_TYPE, RET_TYPE )   \
  template<typename ... Args>                                    \
  inline RET_TYPE &FUNC_NAME( PLOT_TYPE& obj, Args ... args )    \
  { return this->Pad().FUNC_NAME( obj, args ... );               \
  }                                                              \
  template<typename ... Args>                                    \
  inline RET_TYPE &FUNC_NAME( PLOT_TYPE* obj, Args ... args )    \
  { return this->Pad().FUNC_NAME( obj, args ... );}

  PASSTHROUGH_PLOTFUNC( PlotHist,  TH1D,       TH1D );
  PASSTHROUGH_PLOTFUNC( PlotGraph, TGraph,     TGraph );
  PASSTHROUGH_PLOTFUNC( PlotData,  RooAbsData, TGraphAsymmErrors );
  PASSTHROUGH_PLOTFUNC( PlotPdf,   RooAbsPdf,  TGraph );
#undef PASSTHROUGH_PLOTFUNC

#define PASSTHOUGH_VOIDFUNC( FUNC_NAME ) \
  template<typename ... Args>            \
  inline void FUNC_NAME( Args ... args ) \
  { this->Pad().FUNC_NAME( args ... ); }

  PASSTHOUGH_VOIDFUNC( DrawCMSLabel );
  PASSTHOUGH_VOIDFUNC( DrawLuminosity );
  PASSTHOUGH_VOIDFUNC( SetLogy );
  PASSTHOUGH_VOIDFUNC( SetHistAxisTitles );
#undef PASSTHOUGH_VOIDFUNC

protected:
  void _init_margin();// Setting up default margins

};

}/* plt  */

}/* usr  */


#endif/* end of include guard: USERUTILS_PLOTUTILS_SIMPLE1DCANVAS_HPP  */
