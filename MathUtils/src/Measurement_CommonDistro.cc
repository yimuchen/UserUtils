/**
 * @file
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 * @brief   Implementing common measurement constructions
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/MathUtils/interface/Measurement/Arithmetic.hpp"
#include "UserUtils/MathUtils/interface/Measurement/CommonDistro.hpp"
#include "UserUtils/MathUtils/interface/RootMathTools/DefaultEngines.hpp"
#include "UserUtils/MathUtils/interface/StatisticsUtil.hpp"
#else
#include "UserUtils/MathUtils/Measurement/Arithmetic.hpp"
#include "UserUtils/MathUtils/Measurement/CommonDistro.hpp"
#include "UserUtils/MathUtils/RootMathTools/DefaultEngines.hpp"
#include "UserUtils/MathUtils/StatisticsUtil.hpp"
#endif

#include "Math/Functor.h"
#include "Math/QuantFuncMathCore.h"
#include "TEfficiency.h"
#include <limits>

namespace usr
{

namespace Efficiency
{

/**
 * @brief simpler interface for creating Efficiency (binomial) measurements
 *         with minos uncertainties.
 * @ingroup StatUtils
 *
 * @details Calls the GSL functions for generating, known to fail for the
 * extreme case of all pass and all fail.
 */
Measurement
Minos( const double passed, const double total, const double confidencelevel )
{
  return MakeMinos( usr::stat::BinomialNLL( passed, total ),
                    usr::eff_machine_epsilon,
                    1-usr::eff_machine_epsilon,
                    confidencelevel  );
}


const bool shortest_interval = true;
const bool central_interval  = false;

/**
 * @brief Creating a efficiency (binomial) measurement with Bayesian
 *        uncertainties.
 * @ingroup StatUtils
 *
 * @details Using TEfficiency methods, see those for detailed documentation.
 */
Measurement
Bayesian( double passed,
          double total,
          double confidencelevel,
          bool   confidencemethod,
          double alpha,
          double beta )
{
  const double central = ( passed+alpha-1. ) / ( total+alpha+beta-2. );
  const double err_up  = TEfficiency::Bayesian(
    total,
    passed,
    confidencelevel,
    alpha,
    beta,
    true,// For upper boundary
    confidencemethod )-central;
  const double err_down = central-TEfficiency::Bayesian(
    total,
    passed,
    confidencelevel,
    alpha,
    beta,
    false,// For lower boundary
    confidencemethod );
  return Measurement( central, err_up, err_down );
}


/**
 * @brief The default method used for efficiency display used in TEfficiency to
 * avoid under coverage.
 * @ingroup StatUtils
 */
Measurement
ClopperPearson( const double passed,
                const double total,
                const double confidencelevel )
{
  const double central = passed / total;
  const double err_up  = TEfficiency::ClopperPearson( total,
                                                      passed,
                                                      confidencelevel,
                                                      true )-central;
  const double err_down = central-TEfficiency::ClopperPearson( total,
                                                               passed,
                                                               confidencelevel,
                                                               false );
  return Measurement( central, err_up, err_down );
}


/**
 * @brief Lazy method which just uses the method taught in high-school
 *
 * Essentially:
 * \f[
 *  \sigma = \sqrt{\frac{\epsilon(1-\epsilon)}/N}
 * \f]
 *
 * Suffers from excessive under coverage in extreme values of \f$\epsilon\f$
 */
Measurement
Lazy( const double passed, const double total, const double confidencelevel )
{
  const double sigmainterval = usr::stat::GetSigmaInterval( confidencelevel );
  const double central       = passed / total;
  const double error         = std::sqrt( central * ( 1-central ) / total  );
  return Measurement( central, sigmainterval * error, sigmainterval * error );
}

}/* Efficiency */

/*----------------------------------------------------------------------------*/

namespace Poisson
{

/**
 * @brief simple interface for creating poisson measurements with minos
 *        uncertainties.
 * @ingroup StatUtils
 */
Measurement
Minos( const double obs, const double confidencelevel )
{
  if( obs == 0 ){
    return Measurement( 0, 0, 0 );
  }

  return MakeMinos( usr::stat::PoissonNLL( obs ),
                    usr::eff_machine_epsilon,
                    obs+obs * obs+1,
                    confidencelevel );
}


/**
 * @brief Lazy method for a Poisson measurement: just using the square root.
 * @ingroup StatUtils
 */
Measurement
Lazy( const double obs, const double confidencelevel )
{
  const double sigmainterval = usr::stat::GetSigmaInterval( confidencelevel );
  const double error         = std::sqrt( obs );
  return Measurement( obs, sigmainterval * error, sigmainterval * error );
}


/**
 * @brief Least undercoverage Interval as recommended by CMS Statistics
 * committee
 * @ingroup StatUtils
 *
 * The Complete algorithm can be found here:
 * https://twiki.cern.ch/twiki/bin/view/CMS/PoissonErrorBars
 *
 * We are using the recipe for the older root version.
 */
Measurement
CMSStatCom( const double obs, const double confidencelevel )
{
  const double alpha = 1-confidencelevel;
  const double lower = ( obs == 0.0 ) ?
                       0.0 :
                       ( ROOT::Math::gamma_quantile( alpha / 2, obs, 1. ) );
  const double upper = ROOT::Math::gamma_quantile_c( alpha / 2, obs+1, 1 );

  return Measurement( obs, upper-obs,  obs-lower );
}

}/* Poisson */

}/* usr */
