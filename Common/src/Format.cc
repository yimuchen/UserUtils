/**
 * @file    Format.cc
 * @brief   Implementatin of double formatting strings
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */
#include "UserUtils/Common/interface/Format.hpp"
#include "UserUtils/Common/interface/Maths.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <regex>
#include <string>
#include <cmath>

namespace usr {

namespace fmt {

namespace base {

/*-----------------------------------------------------------------------------
 *  Default settings control variables
   --------------------------------------------------------------------------*/
int precision_default        = 8;
unsigned spacesep_default    = 3;
std::string spacestr_default = "";
const unsigned max_precision = 27;

/*-----------------------------------------------------------------------------
 *  Decimal represenation functions
   --------------------------------------------------------------------------*/
std::string
decimal::str() const
{
  const unsigned op_precision = std::min( abs(_precision) , abs(max_precision) );
  std::string retstr =
    boost::    str( boost::format(
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

/*-----------------------------------------------------------------------------
 *  scientific represenation of a double
   --------------------------------------------------------------------------*/
scientific::scientific( const double x , const unsigned p )
{
  precision(p);
  _mant = x ;
  _exp  = ReduceToMant(_mant) ;
}

/*----------------------------------------------------------------------------*/

std::string
scientific::str() const
{
  const std::string base = decimal( _mant, _precision ).dupsetting(*this).str();
  // largest exponent should be around ~300
  // no need of additional formatting.
  const std::string ans = ( _exp == 0) ?  base :
    boost::str( boost::format( "%s \\times 10^{%d}") % base % _exp );
  return ans;
}

} /* base */

} /* fmt */

}/* usr */
