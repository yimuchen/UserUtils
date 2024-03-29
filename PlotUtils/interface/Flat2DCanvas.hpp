#ifndef USERUTILS_PLOTUTILS_FLAT2DCANVAS_HPP
#define USERUTILS_PLOTUTILS_FLAT2DCANVAS_HPP

#ifdef CMSSW_GIT_HASH
#include "UserUtils/PlotUtils/interface/Canvas.hpp"
#include "UserUtils/PlotUtils/interface/Pad2DFlat.hpp"
#else
#include "UserUtils/PlotUtils/Canvas.hpp"
#include "UserUtils/PlotUtils/Pad2DFlat.hpp"
#endif

namespace usr
{

namespace plt
{

/**
 * @brief Canvas containing a single 2D pad
 */
class Flat2DCanvas : public Canvas
{
public:
  static length_t default_width;
  static length_t default_height;
  static FontSet  default_font;
  Flat2DCanvas( const length_t width  = default_width,
                const length_t height = default_height,
                const FontSet&        = default_font );
  virtual ~Flat2DCanvas ();

  /**
   * @brief Returning reference to the underlying Pad2DFlat instance
   */
  inline Pad2DFlat&Pad(){ return GetPad<Pad2DFlat>( 0 ); }

  inline const Pad2DFlat&
  Pad() const { return GetPad<Pad2DFlat>( 0 ); }

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
   * @brief passing through for Pad2DFlat plotting function so that user can
   * 'plot' on this canvas type.
   *
   * @details See Pad2DFlats plot functions for details. Require explicitly
   * defining two interfaces to avoid compile time reference errors.
   */
  DEFINE_PLOTTING( TH2D, PlotHist );
  DEFINE_PLOTTING_RET( TF2, TGraph2D&, PlotFunc );
  DEFINE_PLOTTING( TGraph2D, PlotGraph );
  DEFINE_PLOTTING( TGraph, Plot1DGraph );
  DEFINE_PLOTTING_RET( TGraph2D, TList&,  PlotColGraph );

  /** @} */

#undef DEFINE_PLOTTING
#undef DEFINE_PLOTTING_RET

  /**
   * @{
   * @brief Pass through for axis access
   */
  inline TAxis& Xaxis(){ return Pad().Xaxis(); }
  inline TAxis& Yaxis(){ return Pad().Yaxis(); }
  inline TAxis& Zaxis(){ return Pad().Zaxis(); }
  inline const TAxis&
  Xaxis() const { return Pad().Xaxis(); }
  inline const TAxis&
  Yaxis() const { return Pad().Yaxis(); }
  inline const TAxis&
  Zaxis() const { return Pad().Zaxis(); }

  /** @} */

  /**
   * @{
   * @brief passing through addition Pad2DFlat functions, using variadic
   * template to future proof interface.
   */
  template<typename ... Args>
  inline void DrawCMSLabel( Args ... args ){ Pad().DrawCMSLabel( args ... );}
  template<typename ... Args>
  inline void
  DrawLuminosity( Args ... args )
  {
    Pad().DrawLuminosity( args ... );
  }


  /** @} */
#undef CANVAS2D_VOID_PASSTHROUGH

  inline void SetLogx( int x = 1  ){ Pad().SetLogx( x ); }
  inline void SetLogy( int x = 1  ){ Pad().SetLogy( x ); }
  inline void SetLogz( int x = 1  ){ Pad().SetLogz( x ); }

protected:
  void init_margin();
};

}

}

#endif
