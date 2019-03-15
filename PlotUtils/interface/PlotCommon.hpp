/**
 * @file PlotCommon.hpp
 * @author Yi-Mu "Enoch" Chen
 * @brief  Common classes for plotting arguments
 * @date 2019-02-19
 */
#ifndef USERUTILS_PLOTUTILS_PLOTARG_HPP
#define USERUTILS_PLOTUTILS_PLOTARG_HPP

#include "RooCmdArg.h"
#include "RooPlot.h"
#include "RooRealVar.h"

#include "TGraph.h"
#include "TH1D.h"
#include "THStack.h"

#include <vector>

namespace usr {

namespace plt {


// Forward declaration of required classes
struct RangeByVar;// constructor helper class

/**
 * @brief Simple class for exposing additional variables and functions of the
 * @ROOT{RooPlot} object
 */
class RooFrame : public RooPlot
{
public:
  RooFrame();
  RooFrame( const RangeByVar& );
  RooFrame( const double min, const double max );
  RooFrame( const double xmin, const double xmax,
            const double ymin, const double ymax );
  virtual
  ~RooFrame();

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
  template<typename ObjType>
  ObjType&
  LastPlot()
  {
    ObjType& ans = *( dynamic_cast<ObjType*>( getObject( numItems() -1 ) ) );
    return ans;
  }
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
 * @addtogroup PlotUtilsArgument
 * @{
 */


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
 * @brief Enum for defining plot types. Putting this in global plt namespace
 * to reduce verbosity.
 */
enum plottype
{
  plottype_dummy = 10000,// < start at non-zero to avoid value collision.
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
  PlotType( const int );

  /** @brief specifying raw ROOT style string */
  PlotType( const std::string& );
  virtual
  ~PlotType(){}
};



/**
 * @brief Enum for defining plot for 2D plot types. Putting this in global plt
 * namespace to reduce verbosity.
 */
enum plot2df
{
  plot2df_dummy = 10000,// < start at non-zero to avoid value collision.
  heat,// < Plot objects has head maps
  heatcont,// < Plot object as heat maps with contours lines
  cont// < Plotting only contour lines
};

/**
 * @brief Plot argument to specify how data should be presented.
 */
class Plot2DF : public RooCmdArg
{
public:
  static const std::string CmdName;
  /** @brief Enum interface for plottype input */
  Plot2DF( const int );

  /** @brief specifying raw ROOT style string */
  Plot2DF( const std::string& );
  virtual
  ~Plot2DF(){}
};

/**
 * @brief Plot argument to plot a fitted TF1 with fit uncertainties.
 */
class ShowFitErr : public RooCmdArg
{
public:
  static const std::string CmdName;
  ShowFitErr( const TFitResultPtr&, const double z = 1 );
  virtual ~ShowFitErr(){}
};

/** @} */


/**
 * @{
 * @brief Getting the min/max x-y values of a plotting object
 */

extern double GetYmax( const TH1D* );
extern double GetYmin( const TH1D* );
extern double GetYmax( const TGraph* );
extern double GetYmin( const TGraph* );
extern double GetYmin( const THStack* );
extern double GetYmax( const THStack* );
extern double GetXmax( const TGraph* );
extern double GetXmin( const TGraph* );

inline double
GetYmax( const TH1D& x ){ return GetYmax( &x ); }

inline double
GetYmin( const TH1D& x ){ return GetYmin( &x ); }

inline double
GetYmax( const TGraph& x ){ return GetYmax( &x ); }

inline double
GetYmin( const TGraph& x ){ return GetYmin( &x ); }

inline double
GetXmax( const TGraph& x ){ return GetXmax( &x ); }

inline double
GetXmin( const TGraph& x  ){ return GetXmin( &x ); }
/** @} */

extern double EstimateLatexWidth( const std::string& text );

extern double EstimateLatexHeight( const std::string& text );


}// plt

}// usr



#endif
