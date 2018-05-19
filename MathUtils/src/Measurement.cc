/*******************************************************************************
*
*  Filename    : Measurement.cc
*  Description : Implmentation of arithmetics for Measurement class
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "UserUtils/MathUtils/interface/Measurement.hpp"

#include <iostream>

using namespace std;

namespace usr {

/*******************************************************************************
*   Constructor And Destructor
*******************************************************************************/
Measurement::Measurement()
{
  _central_value = 0;
  _error_up     = 0;
  _error_down   = 0;
}

/******************************************************************************/

Measurement::Measurement(
  const double c,
  const double error_up,
  const double error_down ) :
  _central_value( c ),
  _error_up( error_up ),
  _error_down( error_down )
{
  if( _error_up < 0 ){
    cerr << "Warning! Upper error is small than zero! Assuming flipped sign: "
         << c << " " << error_up << " " << error_down
         << endl;
    _error_up = -_error_up;
  }
  if( _error_down < 0 ){
    cerr << "Warning! Lower error is small than zero! Assuming flipped sign: "
         << c << " " << error_up << " " << error_down
         << endl;
    _error_down = -_error_down;
  }
}

/******************************************************************************/

Measurement::Measurement( const Measurement& x )
{
  *this = x;
}

/******************************************************************************/

Measurement::~Measurement(){}


/*******************************************************************************
*   Assignment operator
*******************************************************************************/
Measurement&
Measurement::operator=( const Measurement& x )
{
  _central_value = x._central_value;
  _error_up     = x._error_up;
  _error_down   = x._error_down;
  return *this;
}

/*******************************************************************************
*   Arithmetics operators, call functions defined in MeasurementArithmetic.hpp
*******************************************************************************/
Measurement
Measurement::NormParam() const
{
  return Measurement( 1,
    RelUpperError(),
    RelLowerError()
    );
}

/*******************************************************************************
*   Measurement - Measurement arithmetics
*   Call functions defined in MeasurementArithmetic.hpp
*******************************************************************************/
Measurement&
Measurement::operator+=( const Measurement& x )
{
  *this = Sum( *this, x );
  return *this;
}

/******************************************************************************/

Measurement&
Measurement::operator*=( const Measurement& x )
{
  *this = Prod( *this, x );
  return *this;
}

/******************************************************************************/

Measurement
Measurement::operator+( const Measurement& x ) const
{
  return Sum( *this, x );
}

/******************************************************************************/

Measurement
Measurement::operator*( const Measurement& x ) const
{
  return Prod( *this, x );
}

/*******************************************************************************
*   Measurement - double arithmetics
*   Required for Measurement Arithmetics
*******************************************************************************/
Measurement&
Measurement::operator+=( const double x )
{
  _central_value += x;
  return *this;
}

/*----------------------------------------------------------------------------*/

Measurement&
Measurement::operator-=( const double x )
{
  _central_value -= x ;
  return *this;
}

/*----------------------------------------------------------------------------*/

Measurement&
Measurement::operator*=( const double x )
{
  _central_value *= x;
  _error_up     *= fabs( x );
  _error_down   *= fabs( x );
  return *this;
}

/*----------------------------------------------------------------------------*/

Measurement&
Measurement::operator/=( const double x )
{
  _central_value /= x;
  _error_up     /= fabs( x );
  _error_down   /= fabs( x );
  return *this;
}

/*----------------------------------------------------------------------------*/

Measurement
Measurement::operator+( const double x ) const
{
  Measurement ans( *this );
  ans += x;
  return ans;
}
/*----------------------------------------------------------------------------*/

Measurement
Measurement::operator-( const double x ) const
{
  Measurement ans( *this );
  ans -= x ;
  return ans;
}

/*----------------------------------------------------------------------------*/

Measurement
Measurement::operator*( const double x ) const
{
  Measurement ans( *this );
  ans *= x;
  return ans;
}

/*----------------------------------------------------------------------------*/

Measurement
Measurement::operator/( const double x ) const
{
  Measurement ans( *this );
  ans /= x;
  return ans;
}

/*----------------------------------------------------------------------------*/

Measurement
operator+( const double y, const Measurement& x )
{
  return x+y;
}

/*----------------------------------------------------------------------------*/

Measurement
operator-( const double y, const Measurement& x )
{
  return Measurement(
    y-x.CentralValue(),
    x.AbsLowerError(),
    x.AbsUpperError()
  );
}

/*----------------------------------------------------------------------------*/

Measurement
operator*( const double y, const Measurement& x )
{
  return x * y;
}

/*----------------------------------------------------------------------------*/


Measurement
operator/( const double y, const Measurement& x )
{
  const double centralValue = y / x._central_value;
  const double err_up       = centralValue * x.RelUpperError();
  const double err_dw       = centralValue * x.RelLowerError();
  return Measurement( centralValue, err_up, err_dw );
}
}/* usr */
