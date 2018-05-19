/**
 * @file    STLUtils_StringPathUtils.cc
 * @brief   Implementation of path utility functions in std::string format.
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */
#include "UserUtils/Common/interface/STLUtils/Filesystem.hpp"
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"

#include <algorithm>
#include <cstdlib>
#include <regex>
#include <string>

namespace usr  {
/*----------------------------------------------------------------------------*/
std::string
GetEnv( const std::string& x )
{
  return std::getenv( x.c_str() );
}

/*-----------------------------------------------------------------------------
 *  Globbing Related functions
   --------------------------------------------------------------------------*/
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

/*-----------------------------------------------------------------------------
 *  Globbing in std::string format
 *  Directly using results of std::filesystem
   --------------------------------------------------------------------------*/
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

/*-----------------------------------------------------------------------------
 *  Random string
   --------------------------------------------------------------------------*/
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


/*-----------------------------------------------------------------------------
 *  string operations functions
   --------------------------------------------------------------------------*/
void
ToUpper( std::string& x )
{
  std::for_each( x.begin(), x.end(), []( char& c ){
      c = std::toupper( c );
    } );
}

void
StripSubstring( std::string& x, const std::string& sub )
{
  size_t found;

  while( ( found = x.find( sub ) ) != std::string::npos ){
    x.erase( x.begin() +found, x.begin()+found + sub.size() );
  }
}



}/* usr  */
