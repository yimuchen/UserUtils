/**
 * @file    parameter_approxfunc_test
 * @brief   testing the functionality of likelihood function approximations
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/STLUtils/OStreamUtils.hpp"
#include "UserUtils/MathUtils/interface/Measurement.hpp"
#include "UserUtils/MathUtils/interface/StatisticsUtil.hpp"
#else
#include "UserUtils/Common/STLUtils/OStreamUtils.hpp"
#include "UserUtils/MathUtils/Measurement.hpp"
#include "UserUtils/MathUtils/StatisticsUtil.hpp"
#endif

#include "Math/Derivator.h"
#include "Math/Functor.h"

int
main( int argc, char* argv[] )
{
  usr::Measurement p( 0, 0.01, 0.01 );

  auto F = [&p]( const double x ){
             return usr::LinearVarianceNLL( x, p );
           };

  ROOT::Math::Functor1D func( F );


  for( double x = -5; x < 5; x += 0.001 ){
    const double deriv = ROOT::Math::Derivator::Eval( func, x );
    printf( "%.6lf: %.6lf %.6lf\n", x, func( x ), deriv );
    assert( !std::isnan( deriv ) );
  }

  return 0;
}
