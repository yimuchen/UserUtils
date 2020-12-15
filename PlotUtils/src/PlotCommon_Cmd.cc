/**
 * @file    PlotCommon_Cmd.cc
 * @brief   Implementation of RooArgCmd related custom classes.
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/RootUtils/RooArgContainer.hpp"
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"
#include "UserUtils/Common/interface/STLUtils/VectorUtils.hpp"
#include "UserUtils/PlotUtils/interface/PlotCommon.hpp"
#else
#include "UserUtils/Common/RootUtils/RooArgContainer.hpp"
#include "UserUtils/Common/STLUtils/StringUtils.hpp"
#include "UserUtils/Common/STLUtils/VectorUtils.hpp"
#include "UserUtils/PlotUtils/PlotCommon.hpp"
#endif

#include "RooFitResult.h"
#include "TFitResult.h"

#include <algorithm>
#include <set>
#include <string>

namespace usr {

namespace plt {

/**
 * @{
 * @addtogroup PlotUtilsArgument
 */
// ---------------------------------------------------------------------------//


/**
 * @brief Request that the plotted object be placed underneath the target object
 **/
RooCmdArg
PlotUnder( const TObject* obj )
{
  return RooCmdArg( "PlotUnder",
    0, 0,/*int*/ 0, 0,/* double*/ 0, 0,/*char*/ obj );
}

USERUTILS_COMMON_REGISTERCMD( PlotUnder );

// ---------------------------------------------------------------------------//
/**
 * @brief define track y via enum
 */
RooCmdArg
TrackY( const int i )
{ return RooCmdArg( "TrackY", i ); }

USERUTILS_COMMON_REGISTERCMD( TrackY );

/**
 * @brief define entry text string.
 */
RooCmdArg EntryText( const std::string& str, const int pl )
{ return RooCmdArg( "EntryText", pl, 0, 0, 0, str.c_str() ); }

USERUTILS_COMMON_REGISTERCMD( EntryText );

/**
 * @brief defining plot type via enum
 */
RooCmdArg PlotType( const int i ){ return RooCmdArg( "PlotType", i ); }


/**
 * @brief defining plot type via ROOT draw strings
 */
RooCmdArg
PlotType( const std::string& opt )
{
  return RooCmdArg( "PlotType"
                  , usr::plt::plottype_dummy, 0, 0, 0, opt.c_str() );
}

USERUTILS_COMMON_REGISTERCMD( PlotType );

/**
 * @brief defining plot type via enum
 */
RooCmdArg Plot2DF( const int i ){ return RooCmdArg( "Plot2DF", i ); }

/**
 * @brief defining plot type via ROOT draw strings
 */
RooCmdArg
Plot2DF( const std::string& opt )
{ return RooCmdArg( "Plot2DF", plot2df_dummy, 0, 0, 0, opt.c_str() );}

USERUTILS_COMMON_REGISTERCMD( Plot2DF );

// ---------------------------------------------------------------------------//

/**
 * @brief Defining text color via the color index + alpha value convention.
 */
RooCmdArg TextColor( const int color, const float alpha )
{ return RooCmdArg( "TextColor", color, 0, alpha ); }

USERUTILS_COMMON_REGISTERCMD( TextColor );

/**
 * @brief Construct a new usrutils common registercmd object
 */
RooCmdArg TextAngle( const float angle )
{ return RooCmdArg( "TextAngle", 0, 0, angle ); }

USERUTILS_COMMON_REGISTERCMD( TextAngle );

/**
 * @brief Defining text size using the standard double notation (>1 is in
 * pixels), (<1 is in ratio to the pad width)
 */
RooCmdArg TextSize( const double size )
{ return RooCmdArg( "TextSize", 0, 0, size ); }

USERUTILS_COMMON_REGISTERCMD( TextSize );

/**
 * @brief Construct a new usrutils common registercmd object
 */
RooCmdArg TextAlign( const int a )
{ return RooCmdArg( "TextAlign", a ); }

USERUTILS_COMMON_REGISTERCMD( TextAlign );

// ---------------------------------------------------------------------------//

/**
 * @brief Defining Line color via the color index + alpha value convention.
 */
RooCmdArg LineColor( const int color, const float alpha )
{ return RooCmdArg( "LineColor", color, 0, alpha ); }

USERUTILS_COMMON_REGISTERCMD( LineColor );

/**
 * @brief Defining Line style via the style index (see namespace usr::plt::sty)
 */
RooCmdArg LineStyle( const short style )
{ return RooCmdArg( "LineStyle", style ); }

USERUTILS_COMMON_REGISTERCMD( LineStyle );

/**
 * @brief Defining line width via number of pixels
 */
RooCmdArg LineWidth( const short width )
{ return RooCmdArg( "LineWidth", width ); }

USERUTILS_COMMON_REGISTERCMD( LineWidth );

// ---------------------------------------------------------------------------//
/**
 * @brief Defining Fill color via color index and alpha value
 */
RooCmdArg FillColor( const int color, const float alpha )
{ return RooCmdArg( "FillColor", color, 0, alpha ); }

USERUTILS_COMMON_REGISTERCMD( FillColor );

/**
 * @brief Defining fill style via the style index (see namespace usr::plt::sty)
 */
RooCmdArg FillStyle( const short style )
{ return RooFit::FillStyle( style ); }

USERUTILS_COMMON_REGISTERCMD( FillStyle );

// ---------------------------------------------------------------------------//
/**
 * @brief Defining Marker color via color index and alpha value
 */
RooCmdArg MarkerColor( const int color, const float alpha )
{ return RooCmdArg( "MarkerColor", color, 0, alpha ); }

USERUTILS_COMMON_REGISTERCMD( MarkerColor );

/**
 * @brief Defining marker style via the style index (see namespace usr::plt::sty)
 */
RooCmdArg MarkerStyle( const short style )
{ return RooFit::MarkerStyle( style ); }

USERUTILS_COMMON_REGISTERCMD( MarkerStyle );

/**
 * @brief Defining MarkerSize  via number of pixels/8.
 */
RooCmdArg MarkerSize( const float size )
{ return RooFit::MarkerSize( size ); }

USERUTILS_COMMON_REGISTERCMD( MarkerSize );

// ---------------------------------------------------------------------------//

RooCmdArg ScaleY( const double s )
{ return RooCmdArg( "ScaleY", 0, 0, s ); }

USERUTILS_COMMON_REGISTERCMD( ScaleY );

// ---------------------------------------------------------------------------//
/**
 * @brief Switch for presenting fit uncertainty in fit.
 */
RooCmdArg
VisualizeError( const TFitResultPtr& fit, const double z )
{
  return RooCmdArg( "VisualizeError",
    0, 0,// int
    z, 0,// double
    0, 0,// c_string
    dynamic_cast<const TObject*>( &( *fit ) ) );
}

// ---------------------------------------------------------------------------//
RooCmdArg ExtrapolateInRatio( const int flag )
{ return RooCmdArg( "ExtrapolateInRatio", flag ); }

USERUTILS_COMMON_REGISTERCMD( ExtrapolateInRatio );


/**
 * @brief Whether or not to extend the x-range of plotting based on the data.
 *
 * Useful is the plot object is just a data container, and we want to leave some
 * margin at the ends for aesthetic reasons.
 */
RooCmdArg ExtendXRange( const bool flag  )
{ return RooCmdArg( "ExtendXRange", flag ); }

USERUTILS_COMMON_REGISTERCMD( ExtendXRange );



/** @} */

}

}

// Class implementation macro needs to be mapped outside of namespace
