/**
 * @file    Maths.cc
 * @brief   Simple concrete type math functions
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 * @details Detailed descriptions
 */
#include <cmath>

namespace usr {
/*----------------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------------*/

int
GetExponent( double x )
{
  return ReduceToMant(x);
}

/*----------------------------------------------------------------------------*/

double
IntPower( double base, int exp )
{
  if( exp < 0 ){ return IntPower( 1./base, exp ); }
  double ans = 1;

  for( int x = 1; x; x <<= 1, base *= base ){
    if( x & exp ){ ans *= base; }
  }

  return ans;
}

} /* usr */
