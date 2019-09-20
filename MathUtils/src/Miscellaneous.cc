/**
 * @file    Miscellaneous.cc
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 * @brief  Implementing miscellaneous mathematical functions
 */

#ifdef CMSSW_GIT_HASH
#include "UserUtils/MathUtils/interface/Miscellaneous.hpp"
#else
#include "UserUtils/MathUtils/Miscellaneous.hpp"
#endif

#include "TDecompChol.h"
#include "TError.h"
#include "TMath.h"

#include <cmath>

namespace usr {

/**
 * @brief returning the intersect points of two segments if exists.
 *
 * Given four points \f$A(p1x,p1y)\f$, \f$B(p2x,p2y)\f$, \f$C(p3x,p3y)\f$,
 * and \f$D(p4x,p4y)\f$. The function checks if segments AB and CD intersect.
 * If yes, store results in intx, and inty return true; return false otherwise.
 */
bool
Intersect(
  const double p1x, const double p1y,
  const double p2x, const double p2y,
  const double p3x, const double p3y,
  const double p4x, const double p4y,
  double& intx, double& inty
  )
{
  const double s1x = p2x - p1x;
  const double s1y = p2y - p1y;
  const double s2x = p4x - p3x;
  const double s2y = p4y - p3y;

  const double s = ( -s1y * ( p1x - p3x ) + s1x * ( p1y - p3y ) ) / ( -s2x*s1y + s1x*s2y );
  const double t = ( s2x * ( p1y - p3y ) - s2y * ( p1x - p3x ) ) / ( -s2x*s1y + s1x*s2y );

  if( s >= 0 && s <= 1 && t >= 0 && t <= 1 ){
    intx = p1x + ( t * s1x );
    inty = p1y + ( t * s1y );
    return true;
  } else {
    return false;
  }
}

/**
 * @brief Performing a Cholesky decomposing of a covariance matrix from fit
 *
 * In the case that the Cholesky decomposition fails, will returned matrix will
 * be computed from the intermediate result. (Such is the feature of the
 * decomposition class in ROOT)
 */
extern TMatrixD
DecompCorvariance( const TMatrixDSym& m )
{
  // Suppressing error warnings of failed decomposition.
  const int outputlevel_temp = gErrorIgnoreLevel;
  gErrorIgnoreLevel = kFatal;

  TDecompChol decomp = TDecompChol( m );
  decomp.Decompose();
  TMatrixD ans = decomp.GetU();
  ans.T();

  gErrorIgnoreLevel = outputlevel_temp;

  return ans;
}

}/* usr */
