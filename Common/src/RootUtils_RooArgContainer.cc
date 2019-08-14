#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/RootUtils.hpp"
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"
#include "UserUtils/Common/interface/STLUtils/VectorUtils.hpp"
#else
#include "UserUtils/Common/RootUtils.hpp"
#include "UserUtils/Common/STLUtils/StringUtils.hpp"
#include "UserUtils/Common/STLUtils/VectorUtils.hpp"
#endif

namespace usr {
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
 * @brief Generating a RooLinked list object from our own container
 *
 * Excluding our custom defined classes, the user also has the ability to
 * add additional exclusion via a list strings.
 */
RooLinkedList
RooArgContainer::MakeRooList( const std::vector<std::string>& exclude ) const
{
  RooLinkedList ans;

  for( const auto& arg : *this ){
    // Ignoring custom arguments
    if( usr::FindValue( CustomCommandList, std::string( arg.GetName() ) ) ||
        usr::FindValue( exclude, std::string( arg.GetName() ) ) ){
      continue;
    }

    ans.Add( arg.Clone() );
  }

  return ans;
}

// Static variable.
std::vector<std::string> RooArgContainer::CustomCommandList;

unsigned
RooArgContainer::RegistorCommand( const std::string& cmd )
{
  assert( usr::FindValue(CustomCommandList, cmd ) == false );
  CustomCommandList.push_back( cmd );
  return CustomCommandList.size();
}

}// namespace usr
