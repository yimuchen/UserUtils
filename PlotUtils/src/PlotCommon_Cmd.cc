#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"
#include "UserUtils/PlotUtils/interface/PlotCommon.hpp"
#else
#include "UserUtils/Common/STLUtils/StringUtils.hpp"
#include "UserUtils/PlotUtils/PlotCommon.hpp"
#endif

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
 * This is not directly a copy constructor, but add additional detection
 * routines so that the command names are unique.
 */
RooArgContainer::RooArgContainer( const std::vector<RooCmdArg>& arglist )
{
  std::set<std::string> nameset;

  for( const auto& arg : arglist ){
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

// ---------------------------------------------------------------------------//

const std::string PlotUnder::CmdName = "PlotUnder";

PlotUnder::PlotUnder( const TObject& obj ):
  RooCmdArg( CmdName.c_str(),
    0,0, // int
    0,0, // double
    0,0, // char
    &obj ){}

PlotUnder::PlotUnder( const TObject* obj ):
  RooCmdArg( CmdName.c_str(),
    0,0, // int
    0,0, // double
    0,0, // char
    obj ){}

// ---------------------------------------------------------------------------//

/**
 * @brief string for defining the RooCmdArg name
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
  RooCmdArg( CmdName.c_str(), 0, 0, 0, 0, drawopt.c_str() ){}

// ---------------------------------------------------------------------------//

/**
 * @brief string for defining the RooCmdArg name
 */
const std::string ShowFitErr::CmdName = "ShowFitErr";

/**
 * @brief defining plot type via ROOT draw strings
 */
ShowFitErr::ShowFitErr( const TFitResultPtr& fit, const double z ) :
  RooCmdArg( CmdName.c_str(),
             0, 0,// int
             z, 0,// double
             0, 0,// c_string
             dynamic_cast<const TObject*>( &( *fit ) ) ){}

}

}
