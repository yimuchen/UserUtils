/*******************************************************************************
*
*  Filename    : StatisticsUtil.hpp
*  Description : General purpose Statistics related utility functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef USERUTILS_MATHUTILS_STATISTICUTIL_HPP
#define USERUTILS_MATHUTILS_STATISTICUTIL_HPP

#include "UserUtils/MathUtils/interface/GSLUtil.hpp"

namespace usr {

namespace stat {

/*******************************************************************************
*   Sigma interval -- Confidence level conversion.
*******************************************************************************/
extern double NormalCDF( const double x );
extern double GetConfidenceLevel( const double sigmainterval );
extern double GetSigmaInterval( const double confidencelevel );

extern const double onesigma_level;
extern const double twosigma_level;

/*******************************************************************************
*   General purpose Minos error computation
*******************************************************************************/
extern double DeltaNLLFromSigma( const double sigma );
extern double DeltaNLLFromConfidence( const double confidence );

// Single value Minos error evaluation
extern int MinosError(
  gsl_function* nllfunction,
  double&       guess,
  double&       min,
  double&       max,
  const double  confidencelevel = onesigma_level
  );


extern int MinosError(
  usr::gsl::gsl_multifunc* nllfunction,
  usr::gsl::gsl_multifunc* varfunction,
  gsl_vector*              initguess,
  double&                  central,
  double&                  min,
  double&                  max,
  const double             confidencelevel = onesigma_level,
  gsl_vector*              upperguess = nullptr,
  gsl_vector*              lowerguess = nullptr
  );

/*******************************************************************************
*   Common Distribution NLLs
*   Defined in standard gsl_function format
*******************************************************************************/

/*******************************************************************************
*   double params[0] - mean,
*   double params[1] - sigma
*******************************************************************************/
extern double GaussianNLL( double, void* params );

/*******************************************************************************
*   double param[0] - passed
*   double param[1] - total
*******************************************************************************/
extern double BinomialNLL( double, void* params );

/*******************************************************************************
*   double param[0] - observed
*******************************************************************************/
extern double PoissonNLL( double, void* params );

}/* stat */

}/* usr */

#endif/* end of include guard: USERUTILS_MATHUTILS_STATISTICUTIL_HPP */
