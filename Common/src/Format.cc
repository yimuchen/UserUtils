/**
 * @file    Format.cc
 * @brief   Implementation of double formatting strings
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */

#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/Format.hpp"
#include "UserUtils/Common/interface/Maths.hpp"
#else
#include "UserUtils/Common/Format.hpp"
#include "UserUtils/Common/Maths.hpp"
#endif

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <cmath>
#include <regex>
#include <string>

namespace usr {

namespace fmt {

namespace base {

/*-----------------------------------------------------------------------------
 *  Default settings control variables
   --------------------------------------------------------------------------*/

/**
 * @details default precision is set such that loss of precision due to IO would
 * not be of too much concern, while avoiding artifacts in decimal--binary
 * conversions (ex. 1.7=1.6999999999999999556 )
 */
int precision_default = 8;

/**
 * @details spacing between digits should be universal (i.e. three digits), but
 * can be changed if publishing to journal using another number system.
 */
unsigned spacesep_default = 3;

/**
 * @details Defaults to none, as it is easier on the eyes when testing code with
 * screen I/O, but you might want to set it to the latex "\," string when
 * mass producing numbers for publication.
 */
std::string spacestr_default = "";

/**
 * @details library specifically doesn't allow more digits then 27 to be
 * printed after the decimal point. This is pretty close to the recommended
 * precision limit of doubles anyway.
 */
const unsigned max_precision = 27;

/**
 * @brief generating string to represent double as a string in decimal.
 */
std::string
decimal::str() const
{
  const unsigned op_precision = std::min( abs( _precision ), abs( max_precision ) );
  std::string retstr          =
    boost::str( boost::format(
      boost::str( boost::format( "%%.%df" )% op_precision )
      )% _input );

  // stripping trailing zero after decimal point
  if( _precision < 0 && retstr.find( '.' ) != std::string::npos ){
    boost::trim_right_if( retstr, boost::is_any_of( "0" ) );
    boost::trim_right_if( retstr, boost::is_any_of( "." ) );
  }

  // Adding spacing string every _spacesep digits away from decimal point
  // Largest double is around e308
  if( _spacesep != 0 && _spacestr != "" ){
    int space = ( ( (int)( retstr.length()/_spacesep ) ) + 1 ) * _spacesep;

    while( space > 0 ){
      if( retstr.find( '.' ) != std::string::npos ){
        // If decimal point exists, expand around decimal point
        boost::format bfexp( "(.*\\d)(\\d{%d}\\..*)" );
        boost::format afexp( "(.*\\.\\d{%d})(\\d.*)" );
        const std::regex beforedec( boost::str( bfexp % space ) );
        const std::regex afterdec( boost::str( afexp % space ) );
        retstr = std::regex_replace( retstr, beforedec, "$1"+_spacestr+"$2" );
        retstr = std::regex_replace( retstr, afterdec,  "$1"+_spacestr+"$2" );
      } else {
        // If decimal point doesnt exist, expand around right most side
        boost::format bfexp( "(.*\\d)(\\d{%d})" );
        const std::regex beforedec( boost::str( bfexp % space ) );
        retstr = std::regex_replace( retstr, beforedec, "$1"+_spacestr+"$2" );
      }
      space -= _spacesep;
    }
  }
  return retstr;
}


/**
 * @brief Requires explicit specification of precision (cannot be negative)
 */
scientific::scientific( const double x, const unsigned p )
{
  precision( p );
  _mant = x;
  _exp  = ReduceToMant( _mant );
}

/**
 * @brief Generating string to represent double as string in scientific
 *  notations.
 */
std::string
scientific::str() const
{
  const std::string base = decimal( _mant, _precision ).dupsetting( *this ).str();
  // largest exponent should be around ~300
  // no need of additional formatting.
  const std::string ans
    = ( _exp == 0 ) ?  base :
      boost::str( boost::format( "%s \\times 10^{%d}" ) % base % _exp );
  return ans;
}

}/* base */

}/* fmt */

}/* usr */
