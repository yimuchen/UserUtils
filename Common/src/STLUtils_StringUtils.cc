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
#include <regex>
#include <string>

namespace usr  {

/**
 * @brief C++ string version of getting system environment variables.
 */
std::string
GetEnv( const std::string& x )
{
  return std::getenv( x.c_str() );
}

/**
 * @brief Converting the glob notation to regex comparation strings.
 */
std::string
GlobToRegex( const std::string& query )
{
  static const std::regex starmatch( "\\*" );
  static const std::string starrep( ".*" );

  static const std::regex qmmatch( "\\?" );
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
  const auto orig = GlobLocal( x );
  std::vector<std::string> ans( orig.size() );
  std::transform( orig.begin(), orig.end(),
    ans.begin(),
    []( const fs::path& x ) -> std::string {
      return x.string();
    } );
  return ans;
}

/**
 * @brief generating an random alpha-numerical string with length n .
 *
 * Currently the random number generator used is that std::rand function
 * provided in the cstdlib.h file. No way of customizing the generator,
 * but allows for setting the random seed outisde the function.
 */
std::string
RandomString( const unsigned n )
{
  static const std::string alphanum = "0123456789"
                                      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                      "abcdefghijklmnopqrstuvwxyz";

  std::string ans = "";

  for( unsigned i = 0; i < n; ++i ){
    ans.push_back( alphanum[rand()%alphanum.length()] );
  }

  return ans;
}


/**
 * @brief Converting entire string to uppercase
 */
void
ToUpper( std::string& x )
{
  std::for_each( x.begin(), x.end(), []( char& c ){
      c = std::toupper( c );
    } );
}

/**
 * @brief removing all instances of a certain substring in a given string.
 */
void
StripSubstring( std::string& x, const std::string& sub )
{
  size_t found;

  while( ( found = x.find( sub ) ) != std::string::npos ){
    x.erase( x.begin() +found, x.begin()+found + sub.size() );
  }
}

}/* usr  */
