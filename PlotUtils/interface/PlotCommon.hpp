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

#include "TEfficiency.h"
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


RooCmdArg        PlotUnder( const TObject* x );
inline RooCmdArg PlotUnder( const TObject& x ){ return PlotUnder( &x ); }

/**
 * @brief Plot argument to specify which edges of data to keep track of.
 */
enum tracky
{
  none, min, max, both, aut
};

RooCmdArg TrackY( int x );

RooCmdArg EntryText( const std::string&, const int place_last = 0 );

// ------------------------------------------------------------------------------
RooCmdArg TextColor( const int col, const float alpha = 1 );
RooCmdArg TextSize( const double size );
RooCmdArg TextAngle( const float x );
RooCmdArg TextAlign( const int a );

// ------------------------------------------------------------------------------

RooCmdArg LineColor( const int col, const float alpha = 1 );
RooCmdArg LineStyle( const short style );
RooCmdArg LineWidth( const short width );

// ------------------------------------------------------------------------------

RooCmdArg FillColor( const int col, const float alpha = 1.0 );
RooCmdArg FillStyle( const short style );

// ------------------------------------------------------------------------------

RooCmdArg MarkerColor( const int col, const float alpha = 1 );
RooCmdArg MarkerStyle( const short style );
RooCmdArg MarkerSize( const float style );

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

RooCmdArg PlotType( const int );
RooCmdArg PlotType( const std::string& );


RooCmdArg ScaleY( const double x );

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
  cont,// < Plotting only contour lines
  box,// < Plotting the box diagram (box size indicating fill value)
  density,// < Plotting scatter type historgrams
};

RooCmdArg Plot2DF( const int );
RooCmdArg Plot2DF( const std::string& );

RooCmdArg VisualizeError( const TFitResultPtr&, const double z = 1 );

template<typename ... Args>
inline RooCmdArg
VisualizeError( const RooFitResult& fit, Args... args )
{
  return RooFit::VisualizeError( fit, args ... );
}

RooCmdArg ExtrapolateInRatio( const int flag = true );

RooCmdArg ExtendXRange( const bool flat      = true );
/**  @} */

/**
 * @addtogroup PlotUtilsHelper
 * @{
 */

extern double GetYmax( const TH1D& );
extern double GetYmin( const TH1D& );
inline double GetYmax( const TH1D* x ){ return GetYmax( *x ); }
inline double GetYmin( const TH1D* x ){ return GetYmin( *x ); }

extern double GetYmax( const TGraph& );
extern double GetYmin( const TGraph& );
extern double GetXmax( const TGraph& );
extern double GetXmin( const TGraph& );
inline double GetYmax( const TGraph* x ){ return GetYmax( *x ); }
inline double GetYmin( const TGraph* x ){ return GetYmin( *x ); }
inline double GetXmax( const TGraph* x ){ return GetXmax( *x ); }
inline double GetXmin( const TGraph* x ){ return GetXmin( *x ); }

extern double GetYmax( const TEfficiency& );
extern double GetYmin( const TEfficiency& );
inline double GetYmax( const TEfficiency* x ){ return GetYmax( *x ); }
inline double GetYmin( const TEfficiency* x ){ return GetYmin( *x ); }

extern double GetYmin( const THStack& );
extern double GetYmax( const THStack& );
inline double GetYmin( const THStack* x ){ return GetYmin( *x ); }
inline double GetYmax( const THStack* x ){ return GetYmax( *x ); }

extern double EstimateLatexWidth( const std::string& text );
extern double EstimateLatexHeight( const std::string& text );

/** @} */

}// plt

}// usr

#endif
