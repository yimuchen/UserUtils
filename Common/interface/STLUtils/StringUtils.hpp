/**
 * @file    StringUtils.hpp
 * @brief   String utility functions
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 * @details The majority of the path utility functions.
 */
#ifndef USERUTILS_COMMON_STLUTILS_STRINGPATHUTILS_HPP
#define USERUTILS_COMMON_STLUTILS_STRINGPATHUTILS_HPP

#include <string>
#include <vector>

namespace usr {
/*-----------------------------------------------------------------------------
 * Getting the environment variable
   --------------------------------------------------------------------------*/
extern std::string GetEnv( const std::string& );

/*-----------------------------------------------------------------------------
 *  Globing related functions
 *  - Including common glob syntax to regex syntax conversion
 *  - string interface for globbing (local and remote) and others
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

}/* usr */


#endif/* end of include guard: USERUTILS_COMMON_STLUTILS_STRINGPATHUTILS_HPP */
