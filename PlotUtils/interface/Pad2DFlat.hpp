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

namespace usr {

namespace plt {

/**
 * @brief Specialized class for 2D data object representation in x-y plane
 * projections
 */
class Pad2DFlat : public PadBase
{
protected:
  Pad2DFlat( Canvas* , const PadSize& );
  Pad2DFlat( Canvas* , const PadSize&,
             const double xmin, const double xmax,
             const double ymin, const double ymax );

  virtual void InitDraw();
  virtual void Finalize();

  friend class Canvas;

public:
  virtual ~Pad2DFlat();
  Pad2DFlat()                   = delete;
  Pad2DFlat( const Pad2DFlat& ) = delete;

  /**
   * @{
   * @brief Plotting 2D histogram object
   */
  TH2D& PlotHist( TH2D&, const std::vector<RooCmdArg>& );
  inline TH2D&
  PlotHist( TH2D* x, const std::vector<RooCmdArg>& list )
  {
    return PlotHist( *x, list );
  }
  inline TH2D& PlotHist( TH2D& x ){ return PlotHist( x, {} );}
  inline TH2D& PlotHist( TH2D* x ){ return PlotHist( *x, {} );}
  template<typename ... Args>
  inline TH2D&
  PlotHist( TH2D& x, const RooCmdArg& arg1, Args ... args )
  {
    return PlotHist( x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  template<typename ... Args>
  inline TH2D&
  PlotHist( TH2D* x, const RooCmdArg& arg1, Args ... args )
  {
    return PlotHist( *x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  // DECLARE_PAD2DFLAT_PLOT_FUNCTIONS( PlotHist, TH2D, TH2D& );
  /** @} */

  /**
   * @{
   * @brief Plotting 2D function object
   */
  TGraph2D& PlotFunc( TF2&, const std::vector<RooCmdArg>& );
  inline TGraph2D&
  PlotFunc( TF2* x, const std::vector<RooCmdArg>& list )
  {
    return PlotFunc( *x, list );
  }
  inline TGraph2D& PlotFunc( TF2& x ){ return PlotFunc( x, {} );}
  inline TGraph2D& PlotFunc( TF2* x ){ return PlotFunc( *x, {} );}
  template<typename ... Args>
  inline TGraph2D&
  PlotFunc( TF2& x, const RooCmdArg& arg1, Args ... args )
  {
    return PlotFunc( x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  template<typename ... Args>
  inline TGraph2D&
  PlotFunc( TF2* x, const RooCmdArg& arg1, Args ... args )
  {
    return PlotFunc( *x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  // DECLARE_PAD2DFLAT_PLOT_FUNCTIONS( PlotFunc, TF2, TGraph2D& );
  /** @} */

  /**
   * @{
   * @brief Plotting 2D graph object
   */
  TGraph2D& PlotGraph( TGraph2D&, const std::vector<RooCmdArg>& );
  inline TGraph2D&
  PlotGraph( TGraph2D* x, const std::vector<RooCmdArg>& list )
  {
    return PlotGraph( *x, list );
  }
  inline TGraph2D& PlotGraph( TGraph2D& x ){ return PlotGraph( x, {} );}
  inline TGraph2D& PlotGraph( TGraph2D* x ){ return PlotGraph( *x, {} );}
  template<typename ... Args>
  inline TGraph2D&
  PlotGraph( TGraph2D& x, const RooCmdArg& arg1, Args ... args )
  {
    return PlotGraph( x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  template<typename ... Args>
  inline TGraph2D&
  PlotGraph( TGraph2D* x, const RooCmdArg& arg1, Args ... args )
  {
    return PlotGraph( *x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  /** @} */

  /**
   * @{
   * @brief Plotting 1D graph object
   */
  TGraph& Plot1DGraph( TGraph&, const std::vector<RooCmdArg>& );
  inline TGraph&
  Plot1DGraph( TGraph* x, const std::vector<RooCmdArg>& list )
  {
    return Plot1DGraph( *x, list );
  }
  inline TGraph& Plot1DGraph( TGraph& x ){ return Plot1DGraph( x, {} );}
  inline TGraph& Plot1DGraph( TGraph* x ){ return Plot1DGraph( *x, {} );}
  template<typename ... Args>
  inline TGraph&
  Plot1DGraph( TGraph& x, const RooCmdArg& arg1, Args ... args )
  {
    return Plot1DGraph( x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  template<typename ... Args>
  inline TGraph&
  Plot1DGraph( TGraph* x, const RooCmdArg& arg1, Args ... args )
  {
    return Plot1DGraph( *x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  /** @} */

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
