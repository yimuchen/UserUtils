/**
 * @file    PlotCommon_Cmd.cc
 * @brief   Implementation of RooArgCmd related custom classes.
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"
#include "UserUtils/PlotUtils/interface/PlotCommon.hpp"
#else
#include "UserUtils/Common/STLUtils/StringUtils.hpp"
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
 * @class RooArgContainer
 * @details We will not be using the @ROOT{RooLinkedList} class because
 * that has the issues of deleting the command objects after going out of
 * scope which causes unintended cansequences.
 */

/**
 * @brief constructing out argument container from a std::vector.
 *
 * This is not directly a copy constructor, but add additional detection routines
 * so that the command names in arglist are unique. The second list is a list of
 * default argument to be added if the corresponding arguments don't exist in the
 * `arglist` input (if not specified, no default arguments will be added.).
 */
RooArgContainer::RooArgContainer(
  const std::vector<RooCmdArg>& arglist,
  const std::vector<RooCmdArg>& default_list )
{
  std::set<std::string> nameset;

  for( const auto& arg : arglist ){
    if( nameset.insert( arg.GetName() ).second ){
      push_back( arg );
    }
  }

  for( const auto& arg : default_list ){
    if( nameset.insert( arg.GetName() ).second ){
      push_back( arg );
    }
  }
}

/** @brief Nothing to do... */
RooArgContainer::~RooArgContainer(){}

/**
 * @brief Checking if the argument list has a command of a given name.
 */
bool
RooArgContainer::Has( const std::string& name ) const
{
  auto iter = std::find_if( begin(), end(),
    [&name]( const RooCmdArg& item ){
        return item.GetName() == name;
      } );
  return iter != end();
}

/**
 * @brief Returning the argument in the list with a given name.
 *
 * @details No not-found protection would be performed.
 */
const RooCmdArg&
RooArgContainer::Get( const std::string& name ) const
{
  auto iter = std::find_if( begin(), end(),
    [&name]( const RooCmdArg& item ){
        return item.GetName() == name;
      } );
  return *iter;
}

/**
 * @brief Function for checking if an argument already exists in a list of
 * RooCmdArgs.
 *
 * Useful for when determining default plotting arguments.
 */
bool
RooArgContainer::CheckList( const std::vector<RooCmdArg>& arglist,
                            const std::string&            name )
{
  auto iter = std::find_if( arglist.begin(), arglist.end(),
    [&name]( const RooCmdArg& item ){
        return item.GetName() == name;
      } );
  return iter != arglist.end();
}

/**
 * @{
 * @addtogroup PlotUtilsArgument
 */
// ---------------------------------------------------------------------------//

/** @brief command name */
const std::string PlotUnder::CmdName = "PlotUnder";

/**
 * @{
 * @brief Request that the plotted object be placed underneath the target object
 **/
PlotUnder::PlotUnder( const TObject& obj ) :
  RooCmdArg( CmdName.c_str(),
             0, 0,// int
             0, 0,// double
             0, 0,// char
             &obj ){}

PlotUnder::PlotUnder( const TObject* obj ) :
  RooCmdArg( CmdName.c_str(),
             0, 0,// int
             0, 0,// double
             0, 0,// char
             obj ){}
/** @} */

// ---------------------------------------------------------------------------//

/**
 * @brief command name
 */
const std::string TrackY::CmdName = "TrackY";

/**
 * @brief define track y via enum
 */
TrackY::TrackY( const int i ) :
  RooCmdArg( CmdName.c_str(), i ){}

// ---------------------------------------------------------------------------//

/**
 * @brief strong for defining the RooCmdArg name
 */
const std::string EntryText::CmdName = "EntryText";

/**
 * @brief define entry text string.
 */
EntryText::EntryText( const std::string& str ) :
  RooCmdArg( CmdName.c_str(), 0, 0, 0, 0, str.c_str() ){}

// ---------------------------------------------------------------------------//

/**
 * @brief string for defining the RooCmdArg name
 */
const std::string PlotType::CmdName = "PlotType";

/**
 * @brief defining plot type via enum
 */
PlotType::PlotType( const int i ) :
  RooCmdArg( CmdName.c_str(), i ){}

/**
 * @brief defining plot type via ROOT draw strings
 */
PlotType::PlotType( const std::string& drawopt ) :
  RooCmdArg( CmdName.c_str(), plottype_dummy, 0, 0, 0, drawopt.c_str() ){}

// ---------------------------------------------------------------------------//

/**
 * @brief string for defining the RooCmdArg name
 */
const std::string Plot2DF::CmdName = "Plot2DF";

/**
 * @brief defining plot type via enum
 */
Plot2DF::Plot2DF( const int i ) :
  RooCmdArg( CmdName.c_str(), i ){}

/**
 * @brief defining plot type via ROOT draw strings
 */
Plot2DF::Plot2DF( const std::string& drawopt ) :
  RooCmdArg( CmdName.c_str(), plot2df_dummy, 0, 0, 0, drawopt.c_str() ){}

// ---------------------------------------------------------------------------//

/**
 * @brief string for defining the RooCmdArg name
 */
const std::string TextColor::CmdName = "TextColor";

/**
 * @class TextColor
 * @brief Defining text color via the color index + alpha value convention.
 */
TextColor::TextColor( const int color, const float alpha ) :
  RooCmdArg( CmdName.c_str(), color, 0, alpha ){}

// ---------------------------------------------------------------------------//

/**
 * @brief string for defining the RooCmdArg name
 */
const std::string TextSize::CmdName = "TextSize";

/**
 * @class TextSize
 * @brief Defining text size using the standard double notation (>1 is in
 * pixels), (<1 is in ratio to the pad width)
 */
TextSize::TextSize( const double size ) :
  RooCmdArg( CmdName.c_str(), 0, 0, size ){}


/**
 * @brief string for defining the RooCmdArg name
 */
const std::string LineColor::CmdName = "LineColor";

/**
 * @class LineColor
 * @brief Defining Line color via the color index + alpha value convention.
 */
LineColor::LineColor( const int color, const float alpha ) :
  RooCmdArg( CmdName.c_str(), color, 0, alpha ){}

/**
 * @brief string for defining the RooCmdArg name
 */
const std::string LineStyle::CmdName = "LineStyle";

/**
 * @class LineStyle
 * @brief Defining Line style via the style index (see namespace usr::plt::sty)
 */
LineStyle::LineStyle( const short style ) :
  RooCmdArg( CmdName.c_str(), style ){}

/**
 * @brief string for defining the RooCmdArg name
 */
const std::string LineWidth::CmdName = "LineWidth";

/**
 * @class LineWidth
 * @brief Defining line width via number of pixels
 */
LineWidth::LineWidth( const short width ) :
  RooCmdArg( CmdName.c_str(), width ){}


/**
 * @brief string for defining the RooCmdArg name
 */
const std::string FillColor::CmdName = "FillColor";

/**
 * @class FillColor
 * @brief Defining Fill color via color index and alpha value
 */
FillColor::FillColor( const int color, const float alpha ) :
  RooCmdArg( CmdName.c_str(), color, 0, alpha ){}

/**
 * @brief string for defining the RooCmdArg name
 */
const std::string FillStyle::CmdName = "FillStyle";

/**
 * @class LineStyle
 * @brief Defining fill style via the style index (see namespace usr::plt::sty)
 */
FillStyle::FillStyle( const short style ) :
  RooCmdArg( RooFit::FillStyle( style ) ){}


/**
 * @brief string for defining the RooCmdArg name
 */
const std::string MarkerColor::CmdName = "MarkerColor";

/**
 * @class MarkerColor
 * @brief Defining Marker color via color index and alpha value
 */
MarkerColor::MarkerColor( const int color, const float alpha ) :
  RooCmdArg( CmdName.c_str(), color, 0, alpha ){}

/**
 * @brief string for defining the RooCmdArg name
 */
const std::string MarkerStyle::CmdName = "MarkerStyle";

/**
 * @class MarkerStyle
 * @brief Defining marker style via the style index (see namespace usr::plt::sty)
 */
MarkerStyle::MarkerStyle( const short style ) :
  RooCmdArg( RooFit::MarkerStyle( style ) ){}

/**
 * @brief string for defining the RooCmdArg name
 */
const std::string MarkerSize::CmdName = "MarkerSize";


/**
 * @class MarkerSize
 * @brief Defining MarkerSize  via number of pixels/8.
 */
MarkerSize::MarkerSize( const float size ) :
  RooCmdArg( RooFit::MarkerSize( size ) ){}


/**
 * @brief string for defining the RooCmdArg name
 */
const std::string VisualizeError::CmdName = "VisualizeError";

/**
 * @brief defining plot type via ROOT draw strings
 */
VisualizeError::VisualizeError( const TFitResultPtr& fit, const double z ) :
  RooCmdArg( CmdName.c_str(),
             0, 0,// int
             z, 0,// double
             0, 0,// c_string
             dynamic_cast<const TObject*>( &( *fit ) ) ){}

VisualizeError::VisualizeError( const RooFitResult& fit,
                                const double        z,
                                const bool          linearmethod ) :
  RooCmdArg( RooFit::VisualizeError( fit, z, linearmethod ) ){}

VisualizeError::VisualizeError( const RooFitResult& fit,
                                const RooArgSet&    param,
                                const double        z,
                                const bool          linearmethod ) :
  RooCmdArg( RooFit::VisualizeError( fit, param, z, linearmethod ) ){}

/** @} */

}

}
