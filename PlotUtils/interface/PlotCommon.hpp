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
  virtual ~RooFrame();

  /**
   * @brief returning the internal histogram object used for axis plotting.
   */
  inline TH1*
  AxisHistPtr(){ return RooPlot::_hist; }

  /**
   * @brief returning the internal histogram object used for axis plotting.
   */
  inline const TH1*
  AxisHistPtr() const { return RooPlot::_hist; }

  /**
   * @brief returning reference to internal histogram object used for axis
   * plotting (no null protection)
   */
  inline TH1&
  AxisHist(){ return *AxisHistPtr(); }

  /**
   * @brief returning reference to internal histogram object used for axis
   * plotting (no null protection)
   */
  inline const TH1&
  AxisHist() const { return *AxisHistPtr(); }

  /**
   * Returning the last object plotted on the RooPlot object. Typically a
   * child class of TGraph, casting will fail if wrong type is passed.
   */
  template<typename ObjType>
  ObjType&
  LastPlot()
  { return *( dynamic_cast<ObjType*>( getObject( numItems() -1 ) ) ); }
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
  RooArgContainer( const std::vector<RooCmdArg>& arg_list,
                   const std::vector<RooCmdArg>& default_list = {} );
  virtual ~RooArgContainer();

  const RooCmdArg& Get( const std::string& name ) const;
  bool             Has( const std::string& name ) const;
  static bool      CheckList( const std::vector<RooCmdArg>&,
                              const std::string& name );
};

/**
 * @brief Specifying the plot object to be placed under another.
 */
class PlotUnder : public RooCmdArg
{
public:
  static const std::string CmdName;
  PlotUnder( const TObject& );
  PlotUnder( const TObject* );
  virtual ~PlotUnder(){}
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

// ------------------------------------------------------------------------------

class TextColor : public RooCmdArg
{
public:
  static const std::string CmdName;
  TextColor( const int color, const float alpha = 1 );
  virtual ~TextColor(){}
};

class TextSize : public RooCmdArg
{
public:
  static const std::string CmdName;
  TextSize( const double size );
  virtual ~TextSize(){}
};

// ------------------------------------------------------------------------------

class LineColor : public RooCmdArg
{
public:
  static const std::string CmdName;
  LineColor( const int color, const float alpha = 1 );
  virtual ~LineColor(){}
};

class LineStyle : public RooCmdArg
{
public:
  static const std::string CmdName;
  LineStyle( const short style );
  virtual ~LineStyle(){};
};

class LineWidth : public RooCmdArg
{
public:
  static const std::string CmdName;
  LineWidth( const short style );
  virtual ~LineWidth(){};
};

// ------------------------------------------------------------------------------

class FillColor : public RooCmdArg
{
public:
  static const std::string CmdName;
  FillColor( const int color, const float alpha = 1 );
  virtual ~FillColor(){}
};

class FillStyle : public RooCmdArg
{
public:
  static const std::string CmdName;
  FillStyle( const short style );
  virtual ~FillStyle(){};
};

// ------------------------------------------------------------------------------

class MarkerColor : public RooCmdArg
{
public:
  static const std::string CmdName;
  MarkerColor( const int color, const float alpha = 1 );
  virtual ~MarkerColor(){}
};

class MarkerStyle : public RooCmdArg
{
public:
  static const std::string CmdName;
  MarkerStyle( const short style );
  virtual ~MarkerStyle(){};
};

class MarkerSize : public RooCmdArg
{
public:
  static const std::string CmdName;
  MarkerSize( const float style );
  virtual ~MarkerSize(){};
};

// ------------------------------------------------------------------------------

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
  inline int
  get() const { return getInt( 0 ); }
};



/**
 * @brief Enum for defining plot for 2D plot types. Putting this in global plt
 * namespace to reduce verbosity.
 */
enum plot2df
{
  plot2df_dummy = 10000,// < start at non-zero to avoid value collision.
  heat,// < Plot objects has head maps
  heattext,// < Plot objects as heat maps with text in bin box
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
 * Intentionally using the same interface as the VisualizeError for
 * @ROOT{RooAbsPdf}.
 */
class VisualizeError : public RooCmdArg
{
public:
  static const std::string CmdName;
  VisualizeError( const TFitResultPtr&,
                  const double z = 1 );
  VisualizeError( const RooFitResult&,
                  const double z = 1,
                  const bool   linearmethod = true  );
  VisualizeError( const RooFitResult&,
                  const RooArgSet& param,
                  const double     z = 1,
                  const bool       linearmethod = true  );
  virtual ~VisualizeError(){}
};

/** @} */


/**
 * @addtogroup PlotUtilsHelper
 * @{
 */

/**
 * @{
 * @brief Getting the min-max value for a histogram object
 */
extern double GetYmax( const TH1D* );
extern double GetYmin( const TH1D* );
inline double
GetYmax( const TH1D& x ){ return GetYmax( &x ); }
inline double
GetYmin( const TH1D& x ){ return GetYmin( &x ); }
/** @} */

/**
 * @{
 * @brief getting the min-max x/y value for a TGraph object
 */
extern double GetYmax( const TGraph* );
extern double GetYmin( const TGraph* );
extern double GetXmax( const TGraph* );
extern double GetXmin( const TGraph* );
inline double
GetYmax( const TGraph& x ){ return GetYmax( &x ); }
inline double
GetYmin( const TGraph& x ){ return GetYmin( &x ); }
inline double
GetXmax( const TGraph& x ){ return GetXmax( &x ); }
inline double
GetXmin( const TGraph& x  ){ return GetXmin( &x ); }
/** @} */

/**
 * @{
 * @brief Gettin the min-max value for a THStack object.
 **/
extern double GetYmin( const THStack* );
extern double GetYmax( const THStack* );
/** @} */

/**
 * @{
 * @brief Estimating dimension of a ROOT flavoured latex string
 * } */
extern double EstimateLatexWidth( const std::string& text );
extern double EstimateLatexHeight( const std::string& text );
/** @} */

/** @} */

}// plt

}// usr



#endif
