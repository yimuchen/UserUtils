/**
 * @file    STLUtils_StringUtils.cc
 * @brief   Implementation of path utility functions in std::string format.
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/STLUtils/Filesystem.hpp"
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"
#else
#include "UserUtils/Common/STLUtils/Filesystem.hpp"
#include "UserUtils/Common/STLUtils/StringUtils.hpp"
#endif

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

namespace usr
{

/**
 * @brief C++ string version of getting system environment variables.
 */
std::string
GetEnv( const std::string& x )
{
  return std::getenv( x.c_str() );
}


/**
 * @brief Converting the glob notation to regex comparaison strings.
 */
std::string
GlobToRegex( const std::string& query )
{
  static const std::regex  starmatch( "\\*" );
  static const std::string starrep( ".*" );

  static const std::regex  qmmatch( "\\?" );
  static const std::string qmrep( "." );

  std::string ans = query;
  ans = std::regex_replace( ans, starmatch, starrep );
  ans = std::regex_replace( ans, qmmatch, qmrep );
  return ans;
}


/**
 * @brief Globbing with the return type being a list of string rather than a
 *        the std::filesystem, useful for ROOT and CMSSW interfaces.
 */
std::vector<std::string>
GlobLocalStr( const std::string& x )
{
  const auto               orig = GlobLocal( x );
  std::vector<std::string> ans( orig.size() );
  std::transform( orig.begin(),
                  orig.end(),
                  ans.begin(), []( const fs::path& x )->std::string {
      return x.string();
    } );
  return ans;
}


/**
 * @brief generating an random alpha-numerical string of length n .
 *
 * Currently the random number generator used is that std::rand function
 * provided in the cstdlib.h file. No way of customizing the generator,
 * but allows for setting the random seed outside the function.
 */
std::string
RandomString( const unsigned n )
{
  static const std::string alphanum =
    "0123456789" "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz";

  std::string ans = "";

  for( unsigned i = 0; i < n; ++i ){
    ans.push_back( alphanum[rand() % alphanum.length()] );
  }

  return ans;
}


/**
 * @brief Converting entire string to uppercase
 */
std::string
ToUpper( const std::string& x )
{
  return boost::algorithm::to_upper_copy( x );
}


/**
 * @brief Converting entire string to lower case
 */
std::string
ToLower( const std::string& x )
{
  return boost::algorithm::to_lower_copy( x );
}


/**
 * @brief removing all instances of a certain substring in a given string.
 */
std::string
StripSubstring( const std::string& x, const std::string& sub )
{
  std::string ans( x );
  boost::algorithm::erase_all( ans, sub );
  return ans;
}


/**
 * @brief Remove all characters that is in string list.
 */
std::string
StripCharacters( const std::string& x, const std::string& sub )
{
  std::string ans( x );
  boost::range::remove_erase_if( ans, boost::is_any_of( sub ) );
  return ans;
}


std::string
StripToNaming( const std::string& x )
{
  std::string ans( x );
  ans = ToLower( ans );
  ans = StripCharacters( ans, " \n\t" );
  return ans;
}


/**
 * @brief Returing the position of a matching brace in a string.
 *
 * For a given input string, and the position to the opening brace: either '[',
 * '(' or '{', the function returns the matching brace position of the
 * corresponding character, taking into account nest braces. For examples the
 * function call:
 *
 * ```
 * MatchBrace( "((()))", 0 ) == 5
 * MatchBrace( "((()))", 1 ) == 4
 * MatchBrace( "((()))", 2 ) == 3
 * ```
 *
 * Note that this function assumes normal tree-like structures between braces of
 * different flavour, while only taking into account the braces of the same
 * flavour.  So the function call:
 * ```
 * MatchBrace("([)]", 0 ) == 2
 * MatchBrace("([)]", 1 ) == 3
 * ```
 * "Makes sense", in that it indeed returns the expected matching brace for '(',
 * and '[' respectively, thought the actual syntax of the full string in
 * ambiguous. In the case that the matching brace cannot be found (for example
 * in
 * the string "((())"), this function will return the end of the string.
 */
size_t
MatchBrace( const std::string& x, const unsigned open_brace )
{
  size_t   ans     = open_brace;
  unsigned counter = 1;

  const char addcount = x.at( open_brace );
  const char mincount = addcount == '(' ?
                        ')' :
                        addcount == '{' ?
                        '}' :
                        addcount == '[' ?
                        ']' :
                        ')';

  while( counter > 0 && ++ans < x.length() ){
    const char c = x.at( ans );
    if( c == addcount ){
      ++counter;
    } else if( c == mincount ){
      --counter;
    }
  }

  return ans;
}


/**
 * @brief Finding the next opening brace character after the given position.
 *
 * Finds the fist instance of '[', '(' or '{' are the given index position.
 */
size_t
NextOpenBrace( const std::string& x, const unsigned start )
{
  return x.find_first_of( "([{", start );
}


/**
 * @brief Checking if master string starts with some target string.
 *
 * Using the std::equal algorithm with the forwards iterator to complete.
 */
bool
starts_with( const std::string& master, const std::string& target )
{
  if( target.size() > master.size() ){ return false; }
  return std::equal( target.begin(), target.end(), master.begin() );
}


/**
 * @brief Checking if a masters string ends with some target string.
 *
 * Using std::equal with the reverse iterator to complete.
 */
bool
ends_with( const std::string& master, const std::string& target )
{
  if( target.size() > master.size() ){ return false; }
  return std::equal( target.rbegin(), target.rend(), master.rbegin() );
}

}/* usr  */
