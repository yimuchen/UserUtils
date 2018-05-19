/*******************************************************************************
*
*  Filename    : Efficiency.cc
*  Description : Implementation for efficiency wrappers
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TEfficiency.h"
#include "UserUtils/MathUtils/interface/Measurement/Arithmetic.hpp"
#include "UserUtils/MathUtils/interface/Measurement/CommonDistro.hpp"
#include "UserUtils/MathUtils/interface/StatisticsUtil.hpp"
#include <limits>

namespace usr {

/*******************************************************************************
*   Minos error from calling StatisticsUtil defined functions
*******************************************************************************/
namespace Efficiency {

Measurement
Minos(
  const double passed,
  const double total,
  const double confidencelevel )
{
  double params[] = {passed, total};
  gsl_function binomial;
  binomial.function = &usr::stat::BinomialNLL;
  binomial.params   = params;

  const double est    = passed/total;
  const double min    = usr::gsl::mch_epsilon ;

  return MakeMinos(
    &binomial,
    est,
    min, 1-min,
    confidencelevel
    );
}

/*******************************************************************************
*   General Wrapper functions for TEfficiency Bayesian method
*******************************************************************************/
const bool shortest_interval = true;
const bool central_interval  = false;

Measurement
Bayesian(
  double passed,
  double total,
  double confidencelevel,
  bool   confidencemethod,
  double alpha,
  double beta
  )
{
  const double central = ( passed + alpha - 1. )/( total + alpha + beta - 2. );
  const double err_up  = TEfficiency::Bayesian(
    total, passed,
    confidencelevel,
    alpha, beta,
    true,// For upper boundary
    confidencemethod
    ) - central;
  const double err_down = central - TEfficiency::Bayesian(
    total, passed,
    confidencelevel,
    alpha, beta,
    false,// For lower boundary
    confidencemethod
    );
  return Measurement( central, err_up, err_down );
}

}/* Efficiency */


/*-----------------------------------------------------------------------------
 *  Wrapper for Poisson related variables
   --------------------------------------------------------------------------*/
namespace Poisson {

Measurement
Minos( const double obs, const double confidencelevel )
{
  gsl_function poisson;
  poisson.function = &usr::stat::PoissonNLL;
  poisson.params   = const_cast<double*>( &obs );

  const double min = usr::gsl::mch_epsilon ;
  const double max = obs + obs*obs +1 ;

  return MakeMinos(
    &poisson,
    obs,
    min,
    max,
    confidencelevel );
}

} /* Poisson */

}/* usr */
