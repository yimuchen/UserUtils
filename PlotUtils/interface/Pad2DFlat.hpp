/**
 * @file Pad2DFlat.hpp
 * @author Yi-Mu "Enoch" Chen
 */
#ifndef USERUTILS_PLOTUTILS_PAD2DFLAT_HPP
#define USERUTILS_PLOTUTILS_PAD2DFLAT_HPP

#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/STLUtils/VectorUtils.hpp"
#include "UserUtils/PlotUtils/interface/Canvas.hpp"
#include "UserUtils/PlotUtils/interface/Constants.hpp"
#include "UserUtils/PlotUtils/interface/PlotCommon.hpp"
#else
#include "UserUtils/Common/STLUtils/VectorUtils.hpp"
#include "UserUtils/PlotUtils/Canvas.hpp"
#include "UserUtils/PlotUtils/Constants.hpp"
#include "UserUtils/PlotUtils/PlotCommon.hpp"
#endif

#include "RooPlot.h"

#include "TF2.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TH2D.h"
#include "TLegend.h"

namespace usr
{

namespace plt
{

/**
 * @brief Specialized class for 2D data object representation in x-y plane
 * projections
 */
class Pad2DFlat : public PadBase
{
protected:
  Pad2DFlat( Canvas*, const PadSize& );
  Pad2DFlat( Canvas*,
             const PadSize&,
             const double xmin,
             const double xmax,
             const double ymin,
             const double ymax );

  virtual void InitDraw();
  virtual void Finalize();

  friend class Canvas;

public:
  virtual ~Pad2DFlat();
  Pad2DFlat() = delete;
  // Pad2DFlat() {} // Needed for reflex should never be used by
  Pad2DFlat( const Pad2DFlat& ) = delete;

#define DEFINE_PLOTTING( OTYPE, FNAME )                                   \
  OTYPE & FNAME( OTYPE&, const std::vector<RooCmdArg>& );                 \
  inline OTYPE& FNAME( OTYPE * x, const std::vector<RooCmdArg>& args )    \
  { return FNAME( *x, args );                                             \
  }                                                                       \
  inline OTYPE& FNAME( OTYPE & x ){ return FNAME( x, {} ); }              \
  inline OTYPE& FNAME( OTYPE * x ){ return FNAME( x, {} ); }              \
  template<typename ... Args>                                             \
  inline OTYPE& FNAME( OTYPE & x, const RooCmdArg & arg1, Args ... args ) \
  { return FNAME( x, MakeVector<RooCmdArg>( arg1, args ... ) ); }         \
  template<typename ... Args>                                             \
  inline OTYPE& FNAME( OTYPE * x, const RooCmdArg & arg1, Args ... args ) \
  { return FNAME( x, MakeVector<RooCmdArg>( arg1, args ... ) ); }

#define DEFINE_PLOTTING_RET( OTYPE, RTYPE, FNAME )                       \
  RTYPE FNAME( OTYPE&, const std::vector<RooCmdArg>& );                  \
  inline RTYPE FNAME( OTYPE * x, const std::vector<RooCmdArg>& args )    \
  { return FNAME( *x, args );                                            \
  }                                                                      \
  inline RTYPE FNAME( OTYPE & x ){ return FNAME( x, {} ); }              \
  inline RTYPE FNAME( OTYPE * x ){ return FNAME( x, {} ); }              \
  template<typename ... Args>                                            \
  inline RTYPE FNAME( OTYPE & x, const RooCmdArg & arg1, Args ... args ) \
  { return FNAME( x, MakeVector<RooCmdArg>( arg1, args ... ) ); }        \
  template<typename ... Args>                                            \
  inline RTYPE FNAME( OTYPE * x, const RooCmdArg & arg1, Args ... args ) \
  { return FNAME( x, MakeVector<RooCmdArg>( arg1, args ... ) ); }

  /**
   * @{
   * @brief Plotting 2D histogram object
   */
  DEFINE_PLOTTING( TH2D, PlotHist );

  // DECLARE_PAD2DFLAT_PLOT_FUNCTIONS( PlotHist, TH2D, TH2D& );

  /** @} */

  /**
   * @{
   * @brief Plotting 2D function object
   */
  DEFINE_PLOTTING_RET( TF2, TGraph2D&,  PlotFunc );

  // DECLARE_PAD2DFLAT_PLOT_FUNCTIONS( PlotFunc, TF2, TGraph2D& );

  /** @} */

  /**
   * @{
   * @brief Plotting 2D graph object
   */
  DEFINE_PLOTTING( TGraph2D, PlotGraph );


  /** @} */

  /**
   * @{
   * @brief Plotting 1D graph object
   */
  DEFINE_PLOTTING( TGraph, Plot1DGraph );

  /** @} */

  /**
   * @{
   * @brief Plotting 2D with z values as colors
   */
  DEFINE_PLOTTING_RET( TGraph2D, TList&, PlotColGraph );

  /** @} */

#undef DEFINE_PLOTTING
#undef DEFINE_PLOTTING_RET

  TObject*     GetAxisObject() const;
  TAxis&       Xaxis();
  TAxis&       Yaxis();
  TAxis&       Zaxis();
  const TAxis& Xaxis() const;
  const TAxis& Yaxis() const;
  const TAxis& Zaxis() const;
  TAxis*       GetXaxis() const;
  TAxis*       GetYaxis() const;
  TAxis*       GetZaxis() const;

  void SetAxisFont();

  /**
   * @brief returning reference to internal legend object.
   */
  inline TLegend& Legend(){ return _legend; }


  void DrawCMSLabel( const std::string&      = cap::prelim,
                     const std::string& main = "CMS" );
  void DrawLuminosity( const double luminosity );
  void DrawLuminosity( const std::string& string );

  inline void SetLogx( int x = 1  ){ TPad_().SetLogx( x ); }
  inline void SetLogy( int x = 1  ){ TPad_().SetLogy( x ); }
  inline void SetLogz( int x = 1  ){ TPad_().SetLogz( x ); }

private:
  TLegend _legend;
  RooFrame _frame;
  void init_legend();
  void MakeLegend();
};

}/* plt */

}/* usr */


#endif
