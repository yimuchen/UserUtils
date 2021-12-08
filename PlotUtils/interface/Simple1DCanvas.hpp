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

  /**
   * @{
   * @brief passing through for Pad1D plotting function so that user can 'plot'
   * on this canvas type. (Require explicitly defining the interface to avoid
   * compile time type ambiguities of reference type.)
   */
  template<typename ... Args>
  inline TH1D&
  PlotHist( TH1D& x, Args ... args )
  {
    return Pad().PlotHist( x, args ... );
  }
  template<typename ... Args>
  inline TH1D&
  PlotHist( TH1D*x, Args ... args )
  {
    return Pad().PlotHist( x, args ... );
  }

  template<typename ... Args>
  inline TGraph&
  PlotFunc( TF1& x, Args ... args )
  {
    return Pad().PlotFunc( x, args ... );
  }
  template<typename ... Args>
  inline TGraph&
  PlotFunc( TF1*x, Args ... args )
  {
    return Pad().PlotFunc( x, args ... );
  }

  template<typename ... Args>
  inline TGraph&
  PlotGraph( TGraph& x, Args ... args )
  {
    return Pad().PlotGraph( x, args ... );
  }


  template<typename ... Args>
  inline TGraph&
  PlotGraph( TGraph*x, Args ... args )
  {
    return Pad().PlotGraph( x, args ... );
  }


  template<typename ... Args>
  inline TEfficiency&
  PlotEff( TEfficiency& x, Args ... args )
  {
    return Pad().PlotEff( x, args ... );
  }


  template<typename ... Args>
  inline TEfficiency&
  PlotEff( TEfficiency*x, Args ... args )
  {
    return Pad().PlotEff( x, args ... );
  }


  template<typename ... Args>
  inline TGraphAsymmErrors&
  PlotData( RooAbsData& x, Args ... args )
  {
    return Pad().PlotData( x, args ... );
  }


  template<typename ... Args>
  inline TGraphAsymmErrors&
  PlotData( RooAbsData*x, Args ... args )
  {
    return Pad().PlotData( x, args ... );
  }


  template<typename ... Args>
  inline TGraph&
  PlotPdf( RooAbsPdf& x, Args ... args )
  {
    return Pad().PlotPdf( x, args ... );
  }
  template<typename ... Args>
  inline TGraph&
  PlotPdf( RooAbsPdf*x, Args ... args )
  {
    return Pad().PlotPdf( x, args ... );
  }

  /** @} */

  /**
   * @{
   * @brief passing through addition Pad1D functions. Using variadic template to
   * future proof interface.
   */
  template<typename ... Args>
  inline void DrawCMSLabel( Args ... args ){ Pad().DrawCMSLabel( args ... ); }
  template<typename ... Args>
  inline void
  DrawLuminosity( Args ... args )
  {
    Pad().DrawLuminosity( args ... );
  }


  template<typename ... Args>
  inline TLine&
  DrawHLine( Args ... args )
  {
    return Pad().DrawHLine( args ... );
  }


  template<typename ... Args>
  inline TLine&
  DrawVLine( Args ... args )
  {
    return Pad().DrawVLine( args ... );
  }


  template<typename ... Args>
  inline void SetLogy( Args ... args ){ Pad().SetLogy( args ... ); }
  template<typename ... Args>
  inline PadBase&
  WriteLine( Args ... args )
  {
    return Pad().WriteLine( args ... );
  }


  template<typename ... Args>
  inline void SetHistAxisTitles( Args ... args )
  { Pad().SetHistAxisTitles( args ... ); }

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
