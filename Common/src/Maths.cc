/**
 * @file    Maths.cc
 * @brief   Simple concrete type math functions
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#include <cmath>

namespace usr {

/**
 * @brief Reducing the double x to it's mantissa component in it's scientific
 *        notation, returns the exponent.
 */
int ReduceToMant( double& x )
{
  int ans = 0;
  if( x == 0 ){ return 0; }

  while( fabs( x ) > 10 ){
    ans++;
    x /= 10;
  }

  while( fabs( x ) < 0.1 ){
    ans--;
    x *= 10;
  }

  return ans;
}

/**
 * @brief returning the exponent for the scientific notation of a double x
 */
int
GetExponent( double x )
{
  return ReduceToMant(x);
}

/**
 * @brief faster implementation of the power function with integers
 *
 * This actually has minimal uses in analysis, I just thought it was a nice
 * excercise for writting a [square-and-multiply algorithm](https://en.wikipedia.org/wiki/Exponentiation_by_squaring) :/
 */
double
IntPower( double base, int exp )
{
  if( exp < 0 ){ return IntPower( 1./base, -exp ); }
  double ans = 1;

  for( int x = 1; x; x <<= 1, base *= base ){
    if( x & exp ){ ans *= base; }
  }

  return ans;
}

} /* usr */
