/*******************************************************************************
*
*  Filename    : Measurement_Formatting.cc
*  Description : String Formatting functions for Measurement class
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*  Using boost::format and std::regex for proper styling
*
*******************************************************************************/
#include "UserUtils/Common/interface/Maths.hpp"
#include "UserUtils/MathUtils/interface/Measurement.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <regex>
#include <string>

namespace usr
{

namespace fmt {

/*-----------------------------------------------------------------------------
 *  Decimal representation implementation functions
   --------------------------------------------------------------------------*/
decimal::decimal( const double input, const int p ) :
  _central( input ),
  _upper( 0 ),
  _lower( 0 )
{
  precision( p );
}

decimal::decimal( const Measurement& input, const int p ) :
  _central( input.CentralValue() ),
  _upper( input.AbsUpperError() ),
  _lower( input.AbsLowerError() )
{
  if( p < 0 ){
    SetPrecision();
  } else {
    precision( p );
  }
}

/*----------------------------------------------------------------------------*/

std::string
decimal::str() const
{
  if( _upper == _lower && _upper == 0 ){
    return base::decimal( _central ).dupsetting( *this ).str();
  } else {
    const std::string cen = base::decimal( _central ).dupsetting( *this ).str();
    const std::string up  = base::decimal( _upper ).dupsetting( *this ).str();
    const std::string lo  = base::decimal( _lower ).dupsetting( *this ).str();

    boost::format symmfmt( "%s\\pm%s" );
    boost::format asymfmt( "%s^{+%s}_{-%s}" );

    if( up == lo ){// string-wise comparison!
      return boost::str( symmfmt % cen % up );
    } else {
      return boost::str( asymfmt % cen % up % lo );
    }
  }
}

/*----------------------------------------------------------------------------*/

decimal&
decimal::precision( const int p )
{
  if( p < 0 && ( _upper != 0 || _lower != 0 ) ){
    SetPrecision();
  } else {
    base::format::precision( abs( p ) );
  }
  return *this;
}

/*----------------------------------------------------------------------------*/

void
decimal::SetPrecision()
{
  const double op_unc = std::max( _upper, _lower );
  const int exp       = GetExponent( op_unc );
  if( exp > 0 ){
    _precision = 0;
  } else {
    _precision = -exp+1;
  }
}

/*-----------------------------------------------------------------------------
 *  Scientific notation implementation functions
   --------------------------------------------------------------------------*/
scientific::scientific( const double input, const unsigned p ) :
  _central( input ),
  _upper( 0 ),
  _lower( 0 ),
  _exp( 0 )
{
  precision( p );
  SetExponent();
}

/*----------------------------------------------------------------------------*/

scientific::scientific( const Measurement& input, const int p ) :
  _central( input.CentralValue() ),
  _upper( input.AbsUpperError() ),
  _lower( input.AbsLowerError() ),
  _exp( 0 )
{
  SetExponent();
  if( p < 0 ){
    SetPrecision();
  } else {
    precision( p );
  }
}

/*----------------------------------------------------------------------------*/
std::string
scientific::str() const
{
  boost::format symmbase( "%s\\pm%s" );
  boost::format asymbase( "%s^{+%s}_{-%s}" );

  boost::format symmexpfmt( "(%s)\\times10^{%d}" );
  boost::format asymexpfmt( "%s\\times10^{%d}" );

  const std::string cen  = base::decimal( _central ).dupsetting( *this ).str();
  const std::string up   = base::decimal( _upper ).dupsetting( *this ).str();
  const std::string lo   = decimal( _lower ).dupsetting( *this ).str();
  const std::string base =
    ( up == lo && _upper == 0 ) ? cen :
    ( up == lo )                ? boost::str( symmbase % cen % up ) :
    boost::str( asymbase % cen % up % lo );

  const std::string ans =
    ( _exp == 0 )               ? base :
    ( up == lo && _upper != 0 ) ? boost::str( symmexpfmt % base % _exp ) :
    boost::str( asymexpfmt % base % _exp );

  return ans;
}

/*----------------------------------------------------------------------------*/
scientific&
scientific::precision( const int i )
{
  if( i < 0 && ( _upper != 0 || _lower != 0 ) ){
    SetPrecision();
  } else {
    base::format::precision( abs( i ) );
  }
  return *this;
}


/*-----------------------------------------------------------------------------
 *  Scinetific notation, helper private functions
   --------------------------------------------------------------------------*/
void
scientific::SetExponent()
{
  if( _central != 0 ){
    _exp      = GetExponent( _central );
    _central /= IntPower( 10, _exp );
    _upper   /= IntPower( 10, _exp );
    _lower   /= IntPower( 10, _exp );
  } else if( _upper != 0 || _lower != 0  ){
    _exp      = std::max( GetExponent( _upper ), GetExponent( _lower ) );
    _central /= IntPower( 10, _exp );
    _upper   /= IntPower( 10, _exp );
    _lower   /= IntPower( 10, _exp );
  }
}
/*----------------------------------------------------------------------------*/
void
scientific::SetPrecision()
{
  double op_unc = std::max( _upper, _lower );
  if( op_unc > _central ){
    if( op_unc < 10 ){
      _precision = 1;
    } else {
      _precision = 0;
    }
  } else {
    _precision = 1;

    while( op_unc < 1 ){
      _precision++;
      op_unc *= 10;
    }
  }
}

}/* fmt */

/*-----------------------------------------------------------------------------
 *  Input specialization
 *  PTreeUtils template specialization
   --------------------------------------------------------------------------*/
template<>
Measurement
GetSingle<Measurement>(
  const boost::property_tree::pt::ptree& tree,
  const std::string& query )
{
  const std::vector<double> input = GetList<double>( tree, query );
  if( input.size() == 0 ){
    return Measurement(1,0,0);// Returning unit for empty input
  } else if( input.size() == 1 ){
    return Measurement(input.at(0),0,0);
  } else if( input.size() == 2 ){
    return Measurement(input.at(0),input.at(1),input.at(1));
  } else { // Ignoring all inputs after the 3 inputs
    return Measurement( input[0], input[1], input[2] );
  }
}

}/* usr */
