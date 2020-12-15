/**
 * @file
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 * @brief   Defining functions for measurement arithmetics
 */
#ifndef USERUTILS_MATHUTILS_MEASUREMENT_ARITHMETIC_HPP
#define USERUTILS_MATHUTILS_MEASUREMENT_ARITHMETIC_HPP

#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/STLUtils/VectorUtils.hpp"
#include "UserUtils/MathUtils/interface/Measurement/Measurement.hpp"
#include "UserUtils/MathUtils/interface/StatisticsUtil.hpp"
#else
#include "UserUtils/Common/STLUtils/VectorUtils.hpp"
#include "UserUtils/MathUtils/Measurement/Measurement.hpp"
#include "UserUtils/MathUtils/StatisticsUtil.hpp"
#endif

#include <vector>

namespace usr {

/**
 * @addtogroup StatUtils
 * @{
 */

/*-----------------------------------------------------------------------------
 *  Container interface for  with MinosError functions
   --------------------------------------------------------------------------*/
extern Measurement MakeMinos(
  const ROOT::Math::IGenFunction& nll,
  const double                    min,
  const double                    max,
  const double                    confidencelevel = usr::stat::onesigma_level );

extern Measurement MakeMinos(
  const ROOT::Math::IMultiGenFunction& nllfunction,
  const ROOT::Math::IMultiGenFunction& varfunction,
  const double*                        initguess,
  const double                         confidencelevel = usr::stat::onesigma_level,
  const double*                        upperguess      = nullptr,
  const double*                        lowerguess      = nullptr  );

/*-----------------------------------------------------------------------------
 *  NLL Functions for measurement uncertainty propagation.
   --------------------------------------------------------------------------*/
double LinearVarianceNLL( double x, const Measurement& m );

/*-----------------------------------------------------------------------------
 *  Simple calculation of un-correlated measurements.
   --------------------------------------------------------------------------*/
extern Measurement EvaluateUncorrelated(
  const std::vector<Measurement>& m_list,
  const ROOT::Math::IMultiGenFunction& var_function,
  const double confidencelevel                 = usr::stat::onesigma_level,
  double (* nll)( double, const Measurement& ) = & LinearVarianceNLL );

extern Measurement SumUncorrelated(
  const std::vector<Measurement>& paramlist,
  const double confidencelevel                   = usr::stat::onesigma_level,
  double ( * nll )( double, const Measurement& ) = & LinearVarianceNLL
  );

extern Measurement ProdUncorrelated(
  const std::vector<Measurement>& paramlist,
  const double confidencelevel                   = usr::stat::onesigma_level,
  double ( * nll )( double, const Measurement& ) = & LinearVarianceNLL
  );

extern Measurement LazyEvaluateUncorrelated(
  const std::vector<Measurement>&      paramlist,
  const ROOT::Math::IMultiGenFunction& varfunction );

/**
 * @brief variadic interface to allow for the indefinite input Sum(x,y,z,....)
 *
 * It is worth noting that the output of (x+y+z) might not be the same as the
 * output of Sum(x,y,z) (operator+ is a two input call to Sum(a,b)). But for
 * all intents and purposes, the errors in uncertainties could be ignore for
 * the most part.
 */
template<typename ... Ts>
Measurement
Sum( const Measurement& x,  Ts ... args )
{
  return SumUncorrelated( MakeVector<Measurement>( x, args ... ) );
}

/**
 * @brief variadic interface to allow for the indefinte input Prod(x,y,z,....)
 *
 * It is worth noting that the output of (x*y*z) might not be the same as the
 * output of Prod(x,y,z) (operator* is a two input call to Prod(a,b)). But for
 * all intents and purposes, the errors in uncertainties could be ignore for
 * the most part.
 */
template<typename ... Ts>
Measurement
Prod( const Measurement& x, Ts ... args )
{
  return ProdUncorrelated( MakeVector<Measurement>( x, args ... ) );
}



/** @} */

}/* usr */

#endif/* end of include guard: USERUTILS_MATHUTILS_PARAMETERARITHMETIC_HPP */
