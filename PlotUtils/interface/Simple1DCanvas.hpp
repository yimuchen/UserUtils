/**
 * @file    Simple1DCanvas.hpp
 * @brief   A canvas with single 1D pad. Good for fast plotting!
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifndef USERUTILS_PLOTUTILS_SIMPLE1DCANVAS_HPP
#define USERUTILS_PLOTUTILS_SIMPLE1DCANVAS_HPP

#ifdef CMSSW_GIT_HASH
#include "UserUtils/PlotUtils/interface/Canvas.hpp"
#include "UserUtils/PlotUtils/interface/Pad1D.hpp"
#else
#include "UserUtils/PlotUtils/Canvas.hpp"
#include "UserUtils/PlotUtils/Pad1D.hpp"
#endif

namespace usr
{

namespace plt
{

class Simple1DCanvas : public Canvas
{
public:
  static length_t default_width;
  static length_t default_height;
  static FontSet  default_font;
  Simple1DCanvas( const RangeByVar&,
                  const length_t width  = default_width,
                  const length_t height = default_height,
                  const FontSet&        = default_font );
  Simple1DCanvas( const length_t width  = default_width,
                  const length_t height = default_height,
                  const FontSet&        = default_font );
  virtual ~Simple1DCanvas ();

  /**
   * @brief Returning reference to the created Pad1D.
   */
  inline Pad1D&Pad(){ return GetPad<Pad1D>( 0 ); }
  inline const Pad1D&
  Pad() const { return GetPad<Pad1D>( 0 ); }

#define DEFINE_PLOTTING( OTYPE, FNAME );          \
  inline OTYPE& FNAME( OTYPE & x )                \
  { return Pad().FNAME( x );                      \
  }                                               \
  inline OTYPE& FNAME( OTYPE * x )                \
  { return Pad().FNAME( x );                      \
  }                                               \
  template<typename ... Args>                     \
  inline OTYPE& FNAME( OTYPE & x, Args ... args ) \
  { return Pad().FNAME( x, args ... );            \
  }                                               \
  template<typename ... Args>                     \
  inline OTYPE& FNAME( OTYPE * x, Args ... args ) \
  { return Pad().FNAME( x, args ... );            \
  }                                               \

#define DEFINE_PLOTTING_RET( OTYPE, RTYPE, FNAME ); \
  inline RTYPE FNAME( OTYPE & x )                   \
  { return Pad().FNAME( x );                        \
  }                                                 \
  inline RTYPE FNAME( OTYPE * x )                   \
  { return Pad().FNAME( x );                        \
  }                                                 \
  template<typename ... Args>                       \
  inline RTYPE FNAME( OTYPE & x, Args ... args )    \
  { return Pad().FNAME( x, args ... );              \
  }                                                 \
  template<typename ... Args>                       \
  inline RTYPE FNAME( OTYPE * x, Args ... args )    \
  { return Pad().FNAME( x, args ...  );             \
  }                                                 \


  /**
   * @{
   * @brief passing through for Pad1D plotting function so that user can 'plot'
   * on this canvas type. (Require explicitly defining the interface to avoid
   * compile time type ambiguities of reference type.)
   */
  DEFINE_PLOTTING( TH1D, PlotHist );
  DEFINE_PLOTTING( TGraph, PlotGraph );
  DEFINE_PLOTTING( TEfficiency, PlotEff );
  DEFINE_PLOTTING_RET( TF1, TGraph&, PlotFunc );
  DEFINE_PLOTTING_RET( RooAbsData, TGraphAsymmErrors&, PlotData );
  DEFINE_PLOTTING_RET( RooAbsPdf, TGraph&, PlotPdf );

#undef DEFINE_PLOTTING
#undef DEFINE_PLOTTING_RET

  /** @} */

#define PASSTHROUGH_VOID( FNAME ) \
  template<typename ... Args>     \
  inline void FNAME( Args ... args ){ Pad().FNAME( args ... ); }
#define PASSTHROUGH_RET( FNAME, RTYPE ) \
  template<typename ... Args>           \
  inline RTYPE FNAME( Args ... args ){ return Pad().FNAME( args ... ); }

  /**
   * @{
   * @brief passing through addition Pad1D functions. Using variadic template to
   * future proof interface.
   */
  PASSTHROUGH_VOID( DrawCMSLabel );
  PASSTHROUGH_VOID( DrawLuminosity );
  PASSTHROUGH_RET( DrawHLine, TLine& );
  PASSTHROUGH_RET( DrawVLine, TLine& );
  PASSTHROUGH_VOID( SetLogy );
  PASSTHROUGH_RET( WriteLine, PadBase& );
  PASSTHROUGH_VOID( SetHistAxisTitles );

  /** @} */

  /**
   * @{
   * @brief Pass through for axis access
   */
  inline TAxis& Xaxis(){ return Pad().Xaxis(); }
  inline TAxis& Yaxis(){ return Pad().Yaxis(); }
  inline const TAxis&
  Xaxis() const { return Pad().Xaxis(); }
  inline const TAxis&
  Yaxis() const { return Pad().Yaxis(); }

  /** @} */

protected:
  void _init_margin();
};

}/* plt  */

}/* usr  */


#endif/* end of include guard: USERUTILS_PLOTUTILS_SIMPLE1DCANVAS_HPP  */
