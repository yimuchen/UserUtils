/*******************************************************************************
*
*  Filename    : MeasurementArithmetic.hpp
*  Description : Arithmetic opertion with good error propagation estimates
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*  Main reference from here:
*
*******************************************************************************/
#ifndef USERUTILS_MATHUTILS_MEASUREMENT_ARITHMETIC_HPP
#define USERUTILS_MATHUTILS_MEASUREMENT_ARITHMETIC_HPP

#include "UserUtils/Common/interface/STLUtils/VectorUtils.hpp"
#include "UserUtils/MathUtils/interface/Measurement/Measurement.hpp"
#include "UserUtils/MathUtils/interface/StatisticsUtil.hpp"
#include <vector>

namespace usr {

/*******************************************************************************
*   Common interfacing with MinosError functions
*******************************************************************************/
extern Measurement MakeMinos(
  gsl_function* nll,
  const double  initguess,
  const double  min,
  const double  max,
  const double  confidencelevel = usr::stat::onesigma_level
  );

extern Measurement MakeMinos(
  usr::gsl::gsl_multifunc* nll,
  usr::gsl::gsl_multifunc* varfunction,
  gsl_vector*              initguess,
  const double             confidencelevel = usr::stat::onesigma_level,
  gsl_vector*              upperguess      = nullptr,
  gsl_vector*              lowerguess      = nullptr
  );

/*******************************************************************************
*   Defining Default approximation NLL function for a given parameter
*******************************************************************************/
double LinearVarianceNLL( double, const Measurement& );

/*******************************************************************************
*   Main calculation functions, do mot call from outside
*   - Implemented in ParamterArithmethic.cc
*******************************************************************************/
Measurement SumUncorrelated(
  const std::vector<Measurement>& paramlist,
  const double confidencelevel = usr::stat::onesigma_level,
  double ( *nll )( double, const Measurement& ) = & LinearVarianceNLL
  );

Measurement ProdUncorrelated(
  const std::vector<Measurement>& paramlist,
  const double confidencelevel = usr::stat::onesigma_level,
  double ( *nll )( double, const Measurement& ) = & LinearVarianceNLL
  );

/*******************************************************************************
*   Template variadic functions for better interfacing
*   ex. Sum( a, b, c , d );
*******************************************************************************/
template<typename ... Ts>
Measurement
Sum( const Measurement& x,  Ts ... args )
{
  return SumUncorrelated( MakeVector<Measurement>( x, args ... ) );
}

/*----------------------------------------------------------------------------*/

template<typename ... Ts>
Measurement
Prod( const Measurement& x, Ts ... args )
{
  return ProdUncorrelated( MakeVector<Measurement>( x, args ... ) );
}

}/* usr */

#endif/* end of include guard: USERUTILS_MATHUTILS_PARAMETERARITHMETIC_HPP */
