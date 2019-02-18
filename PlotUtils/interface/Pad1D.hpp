/**
 * @file    Pad1D.hpp
 * @brief   A pad designed for 1D data plotting.
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifndef USERUTILS_PLOTUTILS_PAD1D_HPP
#define USERUTILS_PLOTUTILS_PAD1D_HPP

#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/STLUtils/VectorUtils.hpp"
#include "UserUtils/PlotUtils/interface/Canvas.hpp"
#include "UserUtils/PlotUtils/interface/Constants.hpp"
#else
#include "UserUtils/Common/STLUtils/VectorUtils.hpp"
#include "UserUtils/PlotUtils/Canvas.hpp"
#include "UserUtils/PlotUtils/Constants.hpp"
#endif

#include "RooAbsData.h"
#include "RooAbsPdf.h"
#include "RooCmdArg.h"
#include "RooFitResult.h"
#include "RooPlot.h"
#include "RooRealVar.h"

#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TH1.h"
#include "TH1D.h"
#include "THStack.h"
#include "TLegend.h"
#include "TLine.h"


#include <vector>

namespace usr  {

namespace plt {

// Forwards declaration of all classes in this header file
class Pad1D;// main thing
struct RangeByVar;// constructor helper class
class PlotType;// RooCmdArg augmentation class ;
class TrackY;// RooCmdArg augmentation class;
class EntryText;// RooCmdArg augmentation class;
class Roo1DFrame;// simple access for Roo1DFrame;
class RooArgContainer;// Helper class for simple RooCmdArg parsing.

/**
 * @brief Simple class for exposing additional variables and functions of the
 * @ROOT{RooPlot} object
 */
class Roo1DFrame : public RooPlot
{
  friend class Pad1D;

public:
  Roo1DFrame();
  Roo1DFrame( const RangeByVar& );
  Roo1DFrame( const double min, const double max );
  virtual
  ~Roo1DFrame();

  /**
   * @brief returning the internal histogram object used for axis plotting.
   */
  inline TH1*
  AxisHistPtr(){ return RooPlot::_hist; }

  /**
   * @brief returning reference to internal histogram object used for axis
   * plotting (no null protection)
   */
  inline TH1&
  AxisHist(){ return *AxisHistPtr(); }

  /**
   * Making a Root object under the ownership of the RooPlot object.
   * Using the variadic interface to allow for any sort of declaration type.
   * @tparam ObjType The type of object you which to create (must be explicitly
   *                 specified)
   * @tparam Args    the arguments could be of any type nessesary.
   * @param  args    any arguments required for a TObject inherited object.
   * @return         Reference to the newly created object.
   */
  template<typename ObjType, typename ... Args>
  ObjType&
  MakeObj( Args ... args )
  {
    ObjType* ptr = new ObjType( args ... );
    RooPlot::addObject( ptr );
    return *ptr;
  }

  /**
   * Returning the last object plotted on the RooPlot object. Typically a
   * child class of TGraph, casting will fail if wrong type is passed.
   */
  template<typename ObjType=TGraph>
  ObjType&
  LastPlot()
  {
    ObjType& ans = *( dynamic_cast<ObjType*>( getObject( numItems() -1 ) ) );
    return ans;
  }
};

/**
 * @brief Specialized class for 1D data plotting.
 */
class Pad1D : public PadBase
{
protected:
  Pad1D( const PadSize&, const RangeByVar& );
  Pad1D( const PadSize&, const double min, const double max );
  Pad1D( const PadSize& );

  virtual void InitDraw();
  virtual void Finalize();

  friend class Canvas;

public:
  /**
   * @{
   * @brief Getting the min/max y values of a plotting object
   */
  static double GetYmax( const TH1D* );
  static double GetYmin( const TH1D* );
  static double GetYmax( const TGraph* );
  static double GetYmin( const TGraph* );
  static double GetYmin( const THStack* );
  static double GetYmax( const THStack* );
  static double GetXmax( const TGraph* );
  static double GetXmin( const TGraph* );
  /** @} */

  /**
   * @{
   * @brief Getting the min/max y values of a plotting object
   */
  inline static double
  GetYmax( const TH1D& x ){ return GetYmax( &x ); }
  inline static double
  GetYmin( const TH1D& x ){ return GetYmin( &x ); }
  inline static double
  GetYmax( const TGraph& x ){ return GetYmax( &x ); }
  inline static double
  GetYmin( const TGraph& x ){ return GetYmin( &x ); }
  inline static double
  GetXmax( const TGraph& x ){ return GetXmax( &x ); }
  inline static double
  GetXmin( const TGraph& x  ){ return GetXmin( &x ); }
  /** @} */

public:
  virtual
  ~Pad1D ();
  Pad1D()               = delete;
  Pad1D( const Pad1D& ) = delete;

#define DECLARE_PLOT_FUNCTIONS( FUNC_NAME, TYPE, RET_TYPE )                   \
  RET_TYPE FUNC_NAME( TYPE&, const std::vector<RooCmdArg> & );                \
  inline RET_TYPE FUNC_NAME( TYPE* x, const std::vector<RooCmdArg> &list ){   \
    return FUNC_NAME( *x, list );                                             \
  }                                                                           \
  inline RET_TYPE FUNC_NAME( TYPE& x ){ return FUNC_NAME( x, {} ); }          \
  inline RET_TYPE FUNC_NAME( TYPE* x ){ return FUNC_NAME( x, {} ); }          \
  template<typename ... Args>                                                 \
  inline RET_TYPE FUNC_NAME( TYPE& x, const RooCmdArg & arg1, Args ... args ){ \
    return FUNC_NAME( x, MakeVector<RooCmdArg>( arg1, args ... ) );           \
  }                                                                           \
  template<typename ... Args>                                                 \
  inline RET_TYPE FUNC_NAME( TYPE* x, const RooCmdArg & arg1, Args ... args ){ \
    return FUNC_NAME( x, MakeVector<RooCmdArg>( arg1, args ... ) );           \
  }

  /** @{
   * @brief
   * Plotting 1D histogram objects
   */
  DECLARE_PLOT_FUNCTIONS( PlotHist, TH1D, TH1D& );
  /** @} */


  /** @{
   * @brief
   * Plotting x-y scatter graph objects
   */
  DECLARE_PLOT_FUNCTIONS( PlotGraph, TGraph, TGraph& );
  /** @} */

  /** @{
   * @brief
   * Plotting RooFit's @ROOT{RooAbsData}s
   */
  DECLARE_PLOT_FUNCTIONS( PlotData, RooAbsData, TGraphAsymmErrors& );
  /** @} */

  /** @{
   * @brief
   * Plotting RooFit's @ROOT{RooAbsPdf}s
   */
  DECLARE_PLOT_FUNCTIONS( PlotPdf, RooAbsPdf, TGraph& );
  /** @} */
#undef DECLARE_PLOT_FUNCTIONS

  TLine& DrawHLine( const double  y,
                    const Color_t c,
                    const Style_t s = 1,
                    const Width_t w = 1 );
  TLine& DrawVLine( const double  x,
                    const Color_t c,
                    const Style_t s          = 1,
                    const Width_t w          = 1 );
  void DrawCMSLabel( const std::string&      = cap::prelim,
                     const std::string& main = "CMS" );
  void DrawLuminosity( const double luminosity );
  void DrawLuminosity( const std::string& string );

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

  // Histogram axis title setting
  static const double autobinwidth;
  static const double forcevarbinwidth;
  void SetHistAxisTitles(
    const std::string& title,
    const std::string& unit          = "",
    const double       forcebinwidth = autobinwidth
    );

  /**
   * @brief enum for specifying y range adjustment type.
   */
  enum rangetype { hist, graph, ratio, pull, aut };
  void AutoSetYRange( const rangetype = aut );
  void SetLogy( int );
  void SetLogx( int );


  void MakeLegend();

  /** @brief returning reference to internal legend object. */
  inline TLegend&
  Legend(){ return _legend; }

  /** @brief returning reference to internal Roo1DFrame object. */
  inline Roo1DFrame&
  FrameObj(){ return _frame; }

  /** @brief returning the stored y range adjustment type */
  inline rangetype&
  RangeType(){ return _prevrangetype; }

protected:

  /**
   * @brief RooPlot object used for generating graphs for ROOFIT objects
   */
  Roo1DFrame _frame;

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

  /** @brief simple class for storing legend entry. */
  struct legentry
  {
    TObject*    obj;
    std::string entry;
    std::string legopt;
  };

  /** @brief legend entry stack */
  std::stack<legentry> _legstack;

  TGraphAsymmErrors& GenGraph( RooAbsData& data, RooLinkedList& arglist );
  TGraph&            GenGraph( RooAbsPdf& pdf, RooLinkedList& arglist );

  static
  void SetAxisTitle(
    TAxis&,
    const std::string& title,
    const std::string& unit = "" );

  void AutoSetYRangeHist();
  void AutoSetYRangeGraph();
  void AutoSetYRangeRatio();
  void AutoSetYRangePull();

  void _init_legend();
  void AddLegendEntry( TH1D&, const std::string&, const RooCmdArg& );
  void AddLegendEntry( TGraph&, const std::string&, const RooCmdArg& );
};

/**
 * @brief Constructor argument container for Pad1D
 */
struct RangeByVar
{
  const RooRealVar* var;
  double            xmin;
  double            xmax;
  int               nbin;
  RangeByVar(
    const RooRealVar& _var,
    const double      _xmin = find_default,
    const double      _xmax = find_default,
    const int         _nbin = -1
    );
  RangeByVar(
    const RooRealVar* _var,
    const double      _xmin = find_default,
    const double      _xmax = find_default,
    const int         _nbin = -1
    );

private:
  static const double find_default;
};

/**
 * @brief Enum for defining plot types. Putting this in global plt namespace
 * to reduce verbosity.
 */
enum plottype
{
  dummy_start = 10000,// < start at non-zero to avoid value collision.
  scatter,// < Plot both graphs and histograms as scatter points.
  hist,// < Plot histogram as boxes
  histerr,// <  error boxes  (both graph and histograms),
  histstack,// < Adding histogram to internal THStack, (histogram only)
  histnewstack,// < Forcing a new stack to be created.
  simplefunc,// < for graphs representing a function
  fittedfunc// < for graphs representing a function with fitting uncertainty
};

/**
 * @brief Plot argument to specify how data should be presented.
 */
class PlotType : public RooCmdArg
{
public:
  static const std::string CmdName;
  /** @brief Enum interface for plottype input */
  PlotType( const int i );

  /** @brief specifying raw ROOT style string */
  PlotType( const std::string& );
  virtual
  ~PlotType(){}
};

/**
 * @brief Plot argument to specify which edges of data to keep track of.
 */
class TrackY : public RooCmdArg
{
public:
  enum tracky
  {
    none, min, max, both, aut
  };
  static const std::string CmdName;
  TrackY( int x = aut );
  virtual
  ~TrackY(){}
};

/**
 * @brief Argument for defining text to place in legend.
 */
class EntryText : public RooCmdArg
{
public:
  static const std::string CmdName;
  EntryText( const std::string& );
  virtual ~EntryText(){}
};

/**
 * @brief Simple container for helping with RooCmdArg parsing.
 */
class RooArgContainer : public std::vector<RooCmdArg>
{
public:
  RooArgContainer( const std::vector<RooCmdArg>& );
  virtual
  ~RooArgContainer();

  const RooCmdArg& Get( const std::string& name ) const;
  bool             Has( const std::string& name ) const;
};

}/* plt */

}/* usr  */

#endif/* end of include guard: USERUTILS_PLOTUTILS_PADFRAME1D_HPP */
