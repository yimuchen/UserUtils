/**
 * @file    Pad1D.hpp
 * @brief   A pad designed for 1D data plotting.
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifndef USERUTILS_PLOTUTILS_PAD1D_HPP
#define USERUTILS_PLOTUTILS_PAD1D_HPP

#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/RootUtils/RooArgContainer.hpp"
#include "UserUtils/Common/interface/STLUtils/VectorUtils.hpp"
#include "UserUtils/PlotUtils/interface/Canvas.hpp"
#include "UserUtils/PlotUtils/interface/Constants.hpp"
#include "UserUtils/PlotUtils/interface/PlotCommon.hpp"
#else
#include "UserUtils/Common/RootUtils/RooArgContainer.hpp"
#include "UserUtils/Common/STLUtils/VectorUtils.hpp"
#include "UserUtils/PlotUtils/Canvas.hpp"
#include "UserUtils/PlotUtils/Constants.hpp"
#include "UserUtils/PlotUtils/PlotCommon.hpp"
#endif

#include "RooAbsData.h"
#include "RooAbsPdf.h"
#include "RooCmdArg.h"
#include "RooFitResult.h"
#include "RooPlot.h"
#include "RooRealVar.h"

#include "TEfficiency.h"
#include "TF1.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"
#include "THStack.h"
#include "TLegend.h"
#include "TLine.h"
#include "TProfile.h"


#include <vector>

namespace usr  {

namespace plt {

/**
 * @brief Specialized class for 1D data plotting.
 */
class Pad1D : public PadBase
{
protected:
  Pad1D( Canvas*, const PadSize&, const RangeByVar& );
  Pad1D( Canvas*, const PadSize&, const double min, const double max );
  Pad1D( Canvas*, const PadSize& );

  virtual void InitDraw();
  virtual void Finalize();

  friend class Canvas;

public:
  virtual ~Pad1D ();
  Pad1D() = delete;
  // Pad1D(){};// Needed for reflex, should never be used!
  Pad1D( const Pad1D& ) = delete;

  /**
   * @{
   * @brief Plotting 1D histogram objects
   */
  TH1D& PlotHist( TH1D&, const std::vector<RooCmdArg>& );
  inline TH1D&
  PlotHist( TH1D* x, const std::vector<RooCmdArg>& list )
  {
    return PlotHist( *x, list );
  }
  inline TH1D& PlotHist( TH1D& x ){ return PlotHist( &x, {} ); }
  inline TH1D& PlotHist( TH1D* x ){ return PlotHist( x, {} ); }
  template<typename ... Args>
  inline TH1D&
  PlotHist( TH1D& x, const RooCmdArg & arg1, Args ... args )
  {
    return PlotHist( x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  template<typename ... Args>
  inline TH1D&
  PlotHist( TH1D* x, const RooCmdArg & arg1, Args ... args )
  {
    return PlotHist( x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }

  /** @} */

  /**
   * @{
   * @brief Special case for TProfile plotting
   */
  TH1D& PlotProfile( TProfile&, const std::vector<RooCmdArg>& );
  inline TH1D&
  PlotProfile( TProfile* x, const std::vector<RooCmdArg>& list )
  {
    return PlotProfile( *x, list );
  }
  inline TH1D& PlotProfile( TProfile& x ){ return PlotProfile( x, {} ); }
  inline TH1D& PlotProfile( TProfile* x ){ return PlotProfile( *x, {} );}
  template<typename ... Args>
  inline TH1D&
  PlotProfile( TProfile& x, const RooCmdArg& arg1, Args ... args )
  {
    return PlotProfile( x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  template<typename ... Args>
  inline TH1D&
  PlotProfile( TProfile* x, const RooCmdArg& arg1, Args ... args )
  {
    return PlotProfile( *x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  /** @} */

  /**
   * @{
   * @brief Plotting TEfficiency objects
   */
  TEfficiency& PlotEff( TEfficiency&, const std::vector<RooCmdArg>& );
  inline TEfficiency&
  PlotEff( TEfficiency* x, const std::vector<RooCmdArg>& list )
  {
    return PlotEff( *x, list );
  }
  inline TEfficiency& PlotEff( TEfficiency& x ){ return PlotEff( x, {} ); }
  inline TEfficiency& PlotEff( TEfficiency* x ){ return PlotEff( *x, {} );}
  template<typename ... Args>
  inline TEfficiency&
  PlotEff( TEfficiency& x, const RooCmdArg& arg1, Args ... args )
  {
    return PlotEff( x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  template<typename ... Args>
  inline TEfficiency&
  PlotEff( TEfficiency* x, const RooCmdArg& arg1, Args ... args )
  {
    return PlotEff( *x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  /** @} */

  /**
   * @{
   * @brief Plotting x-y scatter graph objects
   */
  TGraph& PlotGraph( TGraph&, const std::vector<RooCmdArg>& );
  inline TGraph&
  PlotGraph( TGraph* x, const std::vector<RooCmdArg>& list )
  {
    return PlotGraph( *x, list );
  }
  inline TGraph& PlotGraph( TGraph& x ){ return PlotGraph( x, {} ); }
  inline TGraph& PlotGraph( TGraph* x ){ return PlotGraph( x, {} );}
  template<typename ... Args>
  inline TGraph&
  PlotGraph( TGraph& x, const RooCmdArg& arg1, Args ... args )
  {
    return PlotGraph( x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  template<typename ... Args>
  inline TGraph&
  PlotGraph( TGraph* x, const RooCmdArg& arg1, Args ... args )
  {
    return PlotGraph( x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  /** @} */

  /**
   * @{
   * @brief Plotting ROOT flavoured 1D function
   */
  TGraph& PlotFunc( TF1&, const std::vector<RooCmdArg>& );
  inline TGraph&
  PlotFunc( TF1* x, const std::vector<RooCmdArg>& list )
  {
    return PlotFunc( *x, list );
  }
  inline TGraph& PlotFunc( TF1& x ){ return PlotFunc( x, {} ); }
  inline TGraph& PlotFunc( TF1* x ){ return PlotFunc( x, {} );}
  template<typename ... Args>
  inline TGraph&
  PlotFunc( TF1& x, const RooCmdArg& arg1, Args ... args )
  {
    return PlotFunc( x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  template<typename ... Args>
  inline TGraph&
  PlotFunc( TF1* x, const RooCmdArg& arg1, Args ... args )
  {
    return PlotFunc( x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  /** @} */

  /**
   * @{
   * @brief Plotting RooFit's @ROOT{RooAbsData}s
   */
  TGraphAsymmErrors& PlotData( RooAbsData&, const std::vector<RooCmdArg>& );
  inline TGraphAsymmErrors&
  PlotData( RooAbsData* x, const std::vector<RooCmdArg>& list )
  {
    return PlotData( *x, list );
  }
  inline TGraphAsymmErrors& PlotData( RooAbsData& x ){ return PlotData( x, {} ); }
  inline TGraphAsymmErrors& PlotData( RooAbsData* x ){ return PlotData( x, {} );}
  template<typename ... Args>
  inline TGraphAsymmErrors&
  PlotData( RooAbsData& x, const RooCmdArg& arg1, Args ... args )
  {
    return PlotData( x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  template<typename ... Args>
  inline TGraphAsymmErrors&
  PlotData( RooAbsData* x, const RooCmdArg& arg1, Args ... args )
  {
    return PlotData( x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  /** @} */

  /**
   * @{
   * @brief
   * Plotting RooFit's @ROOT{RooAbsPdf}s
   */
  TGraph& PlotPdf( RooAbsPdf&, const std::vector<RooCmdArg>& );
  inline TGraph&
  PlotPdf( RooAbsPdf* x, const std::vector<RooCmdArg>& list )
  {
    return PlotPdf( *x, list );
  }
  inline TGraph& PlotPdf( RooAbsPdf& x ){ return PlotPdf( x, {} ); }
  inline TGraph& PlotPdf( RooAbsPdf* x ){ return PlotPdf( x, {} );}
  template<typename ... Args>
  inline TGraph&
  PlotPdf( RooAbsPdf& x, const RooCmdArg& arg1, Args ... args )
  {
    return PlotPdf( x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  template<typename ... Args>
  inline TGraph&
  PlotPdf( RooAbsPdf* x, const RooCmdArg& arg1, Args ... args )
  {
    return PlotPdf( x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  /** @} */


  /**
   * @{
   *  @brief function for drawing a horizontal line across pad
   */
  TLine&        DrawHLine( const double&, const std::vector<RooCmdArg>& );
  inline TLine& DrawHLine( const double& x ){ return DrawHLine( x, {} ); }
  template<typename ... Args>
  inline TLine&
  DrawHLine( const double& x, const RooCmdArg& arg1, Args ... args )
  {
    return DrawHLine( x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  /** @} */

  /**
   * @{
   * @brief Function for drawing a vertical along in the pad
   */
  TLine&        DrawVLine( const double&, const std::vector<RooCmdArg>& );
  inline TLine& DrawVLine( const double& x ){ return DrawVLine( x, {} ); }
  template<typename ... Args>
  inline TLine&
  DrawVLine( const double& x, const RooCmdArg& arg1, Args ... args )
  {
    return DrawVLine( x, MakeVector<RooCmdArg>( arg1, args ... ) );
  }
  /** @} */

  void DrawCMSLabel( const std::string&      = cap::prelim,
                     const std::string& main = "CMS" );
  void DrawLuminosity( const double luminosity );
  void DrawLuminosity( const std::string& string );

  // Common text settings
  float InnerTextLeft() const;
  float InnerTextRight() const;
  float InnerTextHCenter() const;
  float InnerTextTop() const;
  float InnerTextBottom() const;
  float InnerTextVCenter() const;

  // Axis and Range setting function
  TObject*     GetAxisObject() const;
  virtual void SetAxisFont();

  TAxis&       Xaxis();
  TAxis&       Yaxis();
  const TAxis& Xaxis() const;
  const TAxis& Yaxis() const;
  TAxis*       GetXaxis() const;
  TAxis*       GetYaxis() const;

  double GetDataMax() const;
  double GetDataMin() const;
  double GetXaxisMin() const;
  double GetXaxisMax() const;
  double GetYaxisMax() const;
  double GetYaxisMin() const;
  void   SetYaxisMax( const double );
  void   SetYaxisMin( const double );
  void   SetDataMax( const double );
  void   SetDataMin( const double );

  // Histogram axis title setting
  static const double autobinwidth;
  static const double forcevarbinwidth;
  void SetHistAxisTitles(
    const std::string& title,
    const std::string& unit          = "",
    const std::string& ytitle        = "Events",
    const double       forcebinwidth = autobinwidth
    );

  /**
   * @brief enum for specifying y range adjustment type.
   */
  enum rangetype { hist, graph, ratio, pull, aut };
  void AutoSetYRange( const rangetype = aut );
  void SetLogy( int );
  void SetLogx( int );

  void AddLegendEntry( TObject&,
                       const std::string& title,
                       const std::string& format );

  void FinalizeLegend( const align x );

  /** @brief returning reference to internal legend object. */
  inline TLegend&
  Legend(){ return _legend; }

  /** @brief returning reference to internal Roo1DFrame object. */
  inline RooFrame&
  FrameObj(){ return _frame; }

  /** @brief returning reference to internal Roo1DFrame object. */
  inline const RooFrame&
  FrameObj() const { return _frame; }

  /** @brief returning the stored y range adjustment type */
  inline rangetype&
  RangeType(){ return _prevrangetype; }

protected:

  /**
   * @brief RooPlot object used for generating graphs for ROOFIT objects
   */
  RooFrame _frame;

  /**
   * @brief Internal @ROOT{THStack} object for assisting with stacked histogram
   * plots.
   */
  THStack* _workingstack;

  /** @{ @brief  tracking the min/max values of the data plotted on the pad  */
  double _datamax;
  double _datamin;
  /** @} */

  /** @brief
   * The range type used to adjust the Y axis range on the previous adjustment
   * call
   */
  rangetype _prevrangetype;

  /** @brief the Legend object */
  TLegend _legend;

  /** @brief position of the legend object in the pad */
  align _legendposition;

  /** @brief list of lines that need finalizing just before plot is finalized */
  std::vector<TLine*> _linelist;

  static
  void SetAxisTitle(
    TAxis&,
    const std::string& title,
    const std::string& unit = "" );

  void AutoSetYRangeHist();
  void AutoSetYRangeGraph();
  void AutoSetYRangeRatio();
  void AutoSetYRangePull();
  void FixVLines();

  void _init_legend();
  void AddLegendEntry( TH1D&, const RooCmdArg&, const RooCmdArg& );
  void AddLegendEntry( TGraph&, const RooCmdArg&, const RooCmdArg& );
  void AddLegendEntry( TEfficiency&, const RooCmdArg&, const RooCmdArg& );

  // Helper function for Plot<> Functions
  void    TrackObjectY( const TObject& obj, const int tracky );
  TGraph& MakePdfGraph( RooAbsPdf&             pdf,
                        const RooArgContainer& arglist );
  TGraph& MakeTF1Graph( TF1&,
                        const RooArgContainer& arglist );
  TGraphAsymmErrors& MakeDataGraph( RooAbsData&            data,
                                    const RooArgContainer& arglist );
  TGraphAsymmErrors& GenGraph( RooAbsData& data, RooLinkedList& arglist );
  TGraph&            GenGraph( RooAbsPdf& pdf, RooLinkedList& arglist );

  bool CheckLogy() const;
  bool CheckLogx() const;

  // Creating the histogram object used to handle the axis object
  // In Pad1D_Plot.cc
  void CreateAxisObject( const TH1D&, const RooArgContainer& );
  void CreateAxisObject( const TGraph&, const RooArgContainer&  );
  void CreateAxisObject( const TF1&, const RooArgContainer&  );
  void CreateAxisObject( const TEfficiency&, const RooArgContainer&  );
  // Common call function to be called by the CreateAxisObject calls
  void PlotCreatedAxisObject( TH1D& );
};

}/* plt */

}/* usr  */

#endif/* end of include guard: USERUTILS_PLOTUTILS_PADFRAME1D_HPP */
