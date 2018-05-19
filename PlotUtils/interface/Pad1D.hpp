/**
 * @file    Pad1D.hpp
 * @brief   A pad designed for 1D data plotting.
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */

#ifndef USERUTILS_PLOTUTILS_PAD1D_HPP
#define USERUTILS_PLOTUTILS_PAD1D_HPP

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

#include "UserUtils/Common/interface/STLUtils/VectorUtils.hpp"
#include "UserUtils/PlotUtils/interface/Canvas.hpp"
#include "UserUtils/PlotUtils/interface/Constants.hpp"

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
 * An simple wrapper object designed to expose additional features of the
 * RooPlot class (the internal TH1 used for axis plotting) and a feature for
 * allow the creation objects directly under the ownership of a RooPlot instance
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

  inline TH1*
  AxisHistPtr(){ return RooPlot::_hist; }
  inline TH1&
  AxisHist(){ return *AxisHistPtr(); }

  template<typename ObjType, typename ... Args>
  ObjType& MakeObj( Args ... );
  template<typename ObjType>
  ObjType& MakeClone( ObjType& );
};


/**
 * Specialised pad for plotting 1 dimensional ROOT and ROOFIT objects.
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
  /** @addtogroup
   *  Helper function for calculating the maximum and minimum y values for a
   *  plotting entity (including the error bars.)
   *  @{
   */
  static double GetYmax( const TH1D* );
  static double GetYmin( const TH1D* );
  static double GetYmax( const TGraph* );
  static double GetYmin( const TGraph* );
  static double GetYmin( const THStack* );
  static double GetYmax( const THStack* );
  inline static double
  GetYmax( const TH1D& x ){ return GetYmax( &x ); }
  inline static double
  GetYmin( const TH1D& x ){ return GetYmin( &x ); }
  inline static double
  GetYmax( const TGraph& x ){ return GetYmax( &x ); }
  inline static double
  GetYmin( const TGraph& x ){ return GetYmin( &x ); }
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
  inline RET_TYPE FUNC_NAME( TYPE& x, const RooCmdArg &arg1, Args ... args ){ \
    return FUNC_NAME( x, MakeVector<RooCmdArg>( arg1, args ... ) );            \
  }                                                                           \
  template<typename ... Args>                                                 \
  inline RET_TYPE FUNC_NAME( TYPE* x, const RooCmdArg &arg1, Args ... args ){ \
    return FUNC_NAME( x, MakeVector<RooCmdArg>( arg1, args ... ) );           \
  }

  DECLARE_PLOT_FUNCTIONS( PlotHist,  TH1D,       TH1D& );
  DECLARE_PLOT_FUNCTIONS( PlotGraph, TGraph,     TGraph& );
  DECLARE_PLOT_FUNCTIONS( PlotData,  RooAbsData, TGraphAsymmErrors& );
  DECLARE_PLOT_FUNCTIONS( PlotPdf,   RooAbsPdf,  TGraph& );
#undef DECLARE_PLOT_FUNCTIONS

  // Additional plotting utilities
  // - Line drawing, note that the lines will be destroyed when the Pad is.
  //   Passing back reference to allow for additional styling;
  TLine& DrawHLine( const double  y,
                    const Color_t c,
                    const Style_t s = 1,
                    const Width_t w = 1 );
  TLine& DrawVLine( const double  x,
                    const Color_t c,
                    const Style_t s = 1,
                    const Width_t w = 1 );

  // Axis and Range setting function
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

  // Automatic adjustment of Y range, with various different types:
  // - histogram will assume a positive definite input, and the y axis minimum
  //   value will be directly set at the data minimum value (if only histograms
  //   are used to generate the plot, the data mimimum value would be clipped
  //   at 0.3)
  // - graph will actively move the y axis minimum away from the data minimum
  // - ratio would keep the axis range centered at 1 extending outwards by
  //   to a maximum of 0-2;
  // - pull will keep the axis ragne centered at 0, extending outwards to a
  //   maximum of -5 -- 5;
  // - aut will automatically determine the range based on the objects in the
  //   pad, If the pad has be adjusted previously by and options, it will use
  //   that options. Otherwise, if any histogram type object exist in the pad,
  //   it will use the histogram options, otherwise it would use the graph
  //   options.
  enum rangetype { hist, graph, ratio, pull, aut };
  void AutoSetYRange( const rangetype = aut );

  // Additional Latex drawing functions
  // Drawing CMS label at top left of the pad (interia of frame)
  // Defaulting to the preliminary tag.
  void DrawCMSLabel( const std::string& = cap::prelim,
                     const std::string& main = "CMS" );
  // Drawing the luminosity on the top right. Explicit string could
  // be added,otherwise assuming 13TeV format. Expected luminosity units
  // is fb-1
  void DrawLuminosity( const double luminosity );
  void DrawLuminosity( const std::string& string );

  // Opening function for setting log scale on y axis and re-adjusting
  // scale accordingly
  void SetLogy( int );

  // Get axis object.
  // This will either be the axis object in the RooPlot instance
  // or the first plotted axis object.
  TObject*     GetAxisObject() const;
  virtual void SetAxisFont();


  // Flushing the contents of the Legend stack and adjusting Legend box size
  void MakeLegend();
  inline TLegend&
  Legend(){ return _legend; }

  // Direct access functions
  inline Roo1DFrame&
  FrameObj(){ return _frame; }
  inline rangetype&
  RangeType(){ return _prevrangetype; }

protected:
  Roo1DFrame _frame;

  // temporary storage for TStack plotting
  THStack* _workingstack;

  // Tracking of the maximum and minimum value of plotted data.
  // and helper functions used for adjusting the weights
  double _datamax;
  double _datamin;
  rangetype _prevrangetype;
  void AutoSetYRangeHist();
  void AutoSetYRangeGraph();
  void AutoSetYRangeRatio();
  void AutoSetYRangePull();

  // TLegend object.
  // An issue with ROOT object drawing is that the first object to be
  // drawn has a smaller z index (placed towards the back), when usually the
  // plotted towards the foreground are more 'important' (should be higher on
  // the Legend entries). So we will be using a stack to temporary store
  // the legend entry and flushing it's content when a save function is called.
  // When the flushing is called, additional method would also be used to
  // determine the size of the legend, and the size of the legend would be
  // adjusted accordlingly.
  TLegend _legend;
  struct legentry
  {
    TObject*    obj;
    std::string entry;
    std::string legopt;
  };
  std::stack<legentry> _legstack;
  void _init_legend();
  void AddLegendEntry( TH1D&, const std::string&, const RooCmdArg& );
  void AddLegendEntry( TGraph&, const std::string&, const RooCmdArg& );

  // Miscelleneous helper functions

  // Helper function for RooFit plotOn generated  objects
  template<typename Ret = TGraph, typename Arg>
  Ret& GenRoofitGraph( Arg&, RooLinkedList& );

  // Static axis title setting function.
  static
  void SetAxisTitle(
    TAxis&,
    const std::string& title,
    const std::string& unit = "" );
};

/*-----------------------------------------------------------------------------
 *  Container for constructor arguments
   --------------------------------------------------------------------------*/
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

/*-----------------------------------------------------------------------------
 *  New RooArgCmd options for unified plotting interface, along with
 *  enum for defining the options. Here we are using inheritence for
 *  better namespace control
   --------------------------------------------------------------------------*/
enum plottype// Putting this type in global plt namespace to reduce
{// verbosity (you probably wouldn't have a variabe called
 // plt::scatter)
  dummy_start = 10000,// Choosing an arbitrary starting point to void 0
                      // integer collision
  scatter,// for both graphs and histograms
  hist,// hist boxes
  histerr,// error boxes  (both graph and histograms),
  histstack,// Adding histogram to internal THStack, (histogram only)
  histnewstack,// Forcing a new stack to be created.
  simplefunc,// for graphs representing a function
  fittedfunc// for graphs representing a function with fitting uncertainty
};

class PlotType : public RooCmdArg// only overloading the consturctor
{
public:
  static const std::string CmdName;
  PlotType( const int i );
  // Enum interface.
  PlotType( const std::string& );
  // passing raw ROOT style string.
  // Addition option parsing (or axis/same... etc) would be peformed.
  virtual
  ~PlotType(){}
};


/*-----------------------------------------------------------------------------
 *  Y range tracking options:
 *  if set to aut, the functions in the Pad1D will override the flag to
 *  something suitable.
   --------------------------------------------------------------------------*/
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

/*-----------------------------------------------------------------------------
 *  EntryText :
 *  Flag for setting the object to be added to the Legend in the pad
   --------------------------------------------------------------------------*/
class EntryText : public RooCmdArg
{
public:
  static const std::string CmdName;
  EntryText( const std::string& );
  virtual ~EntryText(){}
};


/*-----------------------------------------------------------------------------
 *  Class for simple argument parsing (Guaranteeing uniquess and simple getting
 *  and existence detection.
   --------------------------------------------------------------------------*/
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

#include "UserUtils/PlotUtils/src/Pad1D.ipp"

#endif/* end of include guard: USERUTILS_PLOTUTILS_PADFRAME1D_HPP */
