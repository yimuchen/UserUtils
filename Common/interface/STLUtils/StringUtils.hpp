/**
 * @file    StringUtils.hpp
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 * @brief   Functions for string operations
 */
#ifndef USERUTILS_COMMON_STLUTILS_STRINGPATHUTILS_HPP
#define USERUTILS_COMMON_STLUTILS_STRINGPATHUTILS_HPP

#include <string>
#include <vector>

namespace usr {

/**
 * @defgroup StringUtils String utilities
 * @brief    Helper string operation functions
 * @ingroup Common
 * @{
 */

/*-----------------------------------------------------------------------------
 * Getting the environment variable
   --------------------------------------------------------------------------*/
extern std::string GetEnv( const std::string& );

/*-----------------------------------------------------------------------------
 *  Globing related functions
   --------------------------------------------------------------------------*/
extern std::string GlobToRegex( const std::string& x );

/*-----------------------------------------------------------------------------
 *  Globbing in std::string format
   --------------------------------------------------------------------------*/
extern std::vector<std::string> GlobLocalStr( const std::string& pattern );

/*-----------------------------------------------------------------------------
 *  Generating random strings
   --------------------------------------------------------------------------*/
extern std::string RandomString( const unsigned length );

/*-----------------------------------------------------------------------------
 *  String operation functions
   --------------------------------------------------------------------------*/
extern void ToUpper( std::string& );
extern void StripSubstring( std::string&, const std::string& x );

extern size_t MatchBrace( const std::string&, const unsigned open_brace );
extern size_t NextOpenBrace( const std::string&, const unsigned start );

/** @} */

}/* usr */


#endif/* end of include guard: USERUTILS_COMMON_STLUTILS_STRINGPATHUTILS_HPP */
