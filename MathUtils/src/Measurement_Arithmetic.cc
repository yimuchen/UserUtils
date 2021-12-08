/**
 * @file    Measurement_Arithmetic.cc
 * @brief   Implementation of measurement arithetics methods
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/MathUtils/interface/Measurement.hpp"
#include "UserUtils/MathUtils/interface/StatisticsUtil.hpp"
#else
#include "UserUtils/MathUtils/Measurement.hpp"
#include "UserUtils/MathUtils/StatisticsUtil.hpp"
#endif

#include <numeric>

#include "Math/Derivator.h"
#include "Math/Functor.h"

using namespace std;

namespace usr
{

/**
 * @brief interface to the Single variable version of the MinosError() function.
 */
Measurement
MakeMinos( const ROOT::Math::IGenFunction& nll,
           const double                    min,
           const double                    max,
           const double                    confidencelevel )
{
  double central    = min+max / 2;
  double lowerbound = min;
  double upperbound = max;

  usr::stat::MinosError( nll,
                         central,
                         lowerbound,
                         upperbound,
                         confidencelevel );
  return Measurement( central, upperbound-central, central-lowerbound );
}


/**
 * @brief interface to the \f$R^{n}\rightarrow R\f$ version of the MinosError()
 *        function
 */
Measurement
MakeMinos( const ROOT::Math::IMultiGenFunction& nllfunction,
           const ROOT::Math::IMultiGenFunction& varfunction,
           const double*                        initguess,
           const double                         confidencelevel,
           const double*                        upperguess,
           const double*                        lowerguess )
{
  double central;
  double upperbound;
  double lowerbound;

  usr::stat::MinosError( nllfunction,
                         varfunction,
                         initguess,
                         central,
                         lowerbound,
                         upperbound,
                         confidencelevel,
                         upperguess,
                         lowerguess );

  return Measurement( central,
                      fabs( upperbound-central ),
                      fabs( central-lowerbound ) );
}


/**
 * @brief Approximate @NLL function for a measurements of
 *        \f$x^{+\sigma_+}_{-\sigma_i}\f$
 *
 * The approximation for an @NLL function is a variation of the one recommended
 * in [R. Barlow's "Asymmetric statistical
 * errors"](https://arxiv.org/abs/physics/0406120), In the form of:
 *
 * \f[\mathrm{NLL}(x) = \frac{x^{2}}{ 2V(1+Ax)} \f]
 *
 * with \f$V\f$ being the average variance \f$\frac{\sigma_+ + \sigma_-}{2}\f$,
 * and \f$A\f$ being the variance asymmetry \f$\frac{\sigma_{+} -
 * \sigma_{-}}{V}\f$.
 *
 * A few adjustments are made to ensure the stability of the calculations.
 * - The ratio between the two uncertainties would have a hard limit of 10. The
 *   smaller uncertainty would be inflated if extreme asymmetries are present.
 * - The average variance would have an minimum value of \f$10^{-16}\f$ which
 *   should cover most cases where uncertainty calculations are required. Test
 *   have shown that the calculation would not be affected by such limits if the
 *   there are some dominate uncertainties.
 * - To avoid the singularity at \f$x=-1/A\f$, an exponential function would be
 *   used instead for the linear function for the denominator in the @NLL
 *   function when x is past half-way from the central value to the original
 *   singular point, such that the approximation would be extended to an
 * infinite
 *   domain. The exponential function is designed such that the denominator and
 *   its derivative is continuous.
 */

double
LinearVarianceNLL( const double x, const Measurement& m )
{
  static const double maxrelerror = 10.;
  static double       (* AugmentedDenominator)( double,
                                                const double,
                                                const double )
    = []( double x, const double up, const double lo )->double {
        static const double minprod = 1e-12;

        const double V = std::max( up * lo, minprod );
        const double A = ( up-lo ) / V;

        double ans = V * ( 1+x * A );
        if( up > lo && A != 0 && x < ( -lo-1 / A ) / 2 ){
          const double s = ( -lo-1 / A ) / 2;
          const double b = A / ( 1+A * s );
          const double B = V * ( 1+A * s ) / exp( b * s );
          ans = B * exp( b * x );
        } else if( lo > up && A != 0 && x > ( up+1 / A ) / 2 ){
          ans = AugmentedDenominator( -x, lo, up );
        }
        return ans;
      };

  // Getting potentially inflating uncertainties
  const double central  = m.CentralValue();
  const double rawupper = m.AbsUpperError();
  const double rawlower = m.AbsLowerError();
  const double effupper = std::max( rawupper, rawlower / maxrelerror );
  const double efflower = std::max( rawlower, rawupper / maxrelerror );

  const double num = ( x-central ) * ( x-central );
  const double de  = AugmentedDenominator( x-central, effupper, efflower );
  const double ans = 0.5  * num / de;

  return ans;
}


/**
 * @brief given a list of measurements with uncertainties, return the effective
 *        sum of all measurements as if all measurements are uncorrelated.
 *
 * Given a list of parameters, a master @NLL function is generated assuming that
 * all variables are uncorrelated and uses the same underlying @NLL function
 * (specifed with the `nll` argument). The MinosErrors method for
 * multidimensional Minos uncertainty calculations is then invoked for the
 * calculation. This function also automatically estimates the initial guesses
 * for the Minos error calculations using the partial differentials of the
 * variable function. For details of this initial guess estimation see the
 * usr::LazyEvaluateUncorrelated method.
 */
extern Measurement
EvaluateUncorrelated( const std::vector<Measurement>&      m_list,
                      const ROOT::Math::IMultiGenFunction& varfunction,
                      const double                         confidencelevel,
                      double (*                            nll )( double,
                                       const Measurement& ) )
{
  auto masternll = [&m_list, nll]( const double*x )->double {
                     double ans = 0;

                     for( unsigned i = 0; i < m_list.size(); ++i ){
                       ans = ans+nll( x[i], m_list.at( i ) );
                     }

                     return ans;
                   };
  ROOT::Math::Functor nllfunction( masternll, m_list.size() );

  std::vector<double> init;
  std::vector<double> upperguess;
  std::vector<double> lowerguess;
  double              diff_sqsum = 0;

  for( const auto& m : m_list ){
    init.push_back( m.CentralValue() );
  }

  for( unsigned i = 0; i < m_list.size(); ++i  ){
    const double diff =
      ROOT::Math::Derivator::Eval( varfunction, init.data(), i );
    diff_sqsum += diff * diff;
  }

  diff_sqsum = std::sqrt( diff_sqsum );

  // Guess for the boundaries for the input required for getting the upper and
  // lower uncertainties based on the derivatives of the variable function.
  for( unsigned i = 0; i < m_list.size(); ++i ){
    const double diff =
      ROOT::Math::Derivator::Eval( varfunction, init.data(), i );
    const double w = fabs( diff / diff_sqsum );
    if( diff > 0 ){
      upperguess.push_back( init.at( i )+w * m_list.at( i ).AbsUpperError() );
      lowerguess.push_back( init.at( i )-w * m_list.at( i ).AbsLowerError() );
    } else {
      upperguess.push_back( init.at( i )-w * m_list.at( i ).AbsLowerError() );
      lowerguess.push_back( init.at( i )+w * m_list.at( i ).AbsUpperError() );
    }
  }

  return MakeMinos( nllfunction,
                    varfunction,
                    init.data(),
                    confidencelevel,
                    upperguess.data(),
                    lowerguess.data() );
}


/**
 * @brief given a list of measurements with uncertainties, return the effective
 *        sum of all measurements as if all measurements are uncorrelated.
 *
 * This function also allows the user to define the working confidence level
 * for the calculations and also the approximate @NLL function used for the
 * individual measurements (by default using the LinearVarianceNLL() function).
 */
Measurement
SumUncorrelated( const vector<Measurement>& m_list,
                 const double               confidencelevel,
                 double (*                  nll )( double,
                                  const Measurement& ) )
{
  const unsigned dim = m_list.size();
  auto           Sum = [dim]( const double*x ){
                         double ans = 0;

                         for( unsigned i = 0; i < dim; ++i ){
                           ans += x[i];
                         }

                         return ans;
                       };

  return EvaluateUncorrelated( m_list,
                               ROOT::Math::Functor( Sum, dim ),
                               confidencelevel,
                               nll );
}


/**
 * @brief given a list of measurements with uncertainties, return the effective
 *        sum of all measurements as if all measurements are uncorrelated.
 *
 * This function also allows the user to define the working confidence level
 * for the calculations and also the approximate @NLL function used for the
 * individual measurements (by default using the LinearVarianceNLL() function).
 */
Measurement
ProdUncorrelated( const std::vector<Measurement>& m_list,
                  const double                    confidencelevel,
                  double (*                       nll )( double,
                                   const Measurement& ) )
{
  const unsigned dim = m_list.size();
  auto           Prod = [dim]( const double*x ){
                          double ans = 1;

                          for( unsigned i = 0; i < dim; ++i ){
                            ans *= x[i];
                          }

                          return ans;
                        };

  double              prod = 1.;
  vector<Measurement> normlist;

  for( const auto& p : m_list ){
    prod *= p.CentralValue();
    normlist.push_back( p.NormParam() );
  }

  return prod * EvaluateUncorrelated( normlist,
                                      ROOT::Math::Functor( Prod, dim ),
                                      confidencelevel,
                                      nll );
}


/**
 * @brief Give a function of parameters, calculate the error propagation given a
 * various a list of symmetric error functions using a lazy method. Assuming all
 * parameters are uncorrelated
 *
 *
 * For a function given as:
 * \f[
 *    y = f(x_1 , x_2, x_3,\ldots )
 * \f]
 *
 * With a set of measurements \f$\hat{x}_i{}^{+\sigma^{+}_i}_{-\sigma^{-}_i}\f$,
 * The resulting measurement \f$\hat{y}^{+\sigma^{+}_y}_{-\sigma^{-}_y}\f$ is
 * evaluted as:
 *
 * \f[
 *    \hat{y} = f(\hat{x}_1, \hat{x}_2, \hat{x}_3\ldots)
 * \f]
 *
 * and:
 *
 * \f[
 *    y\pm\sigma^{\pm}_{y} = f(\hat{x}_i + \Delta x_i)
 * \f]
 *
 * with
 *
 * \f[
 *    \Delta x_i = \frac{ |\partial_i f| }{\sqrt{\sum_i (\partial_i f)^2}}
 * \sigma^{k_i}_{i}
 * \f]
 *
 * where \f$k_i = +\f$ if \f$\partial_i f\f$ is positive or else  \f$k_i = - \f$
 * otherwise.
 *
 **/
Measurement
LazyEvaluateUncorrelated( const std::vector<Measurement>&      paramlist,
                          const ROOT::Math::IMultiGenFunction& varfunction )
{
  std::vector<double> center;
  std::vector<double> upper;
  std::vector<double> lower;
  double              diff_sqsum = 0;

  for( const auto& p : paramlist ){
    center.push_back( p.CentralValue() );
  }

  for( unsigned i = 0; i < paramlist.size(); ++i ){
    const double diff = ROOT::Math::Derivator::Eval( varfunction,
                                                     center.data(),
                                                     i );
    diff_sqsum += diff * diff;
  }

  diff_sqsum = std::sqrt( diff_sqsum );

  for( unsigned i = 0; i < paramlist.size(); ++i ){
    const double diff = ROOT::Math::Derivator::Eval( varfunction,
                                                     center.data(),
                                                     i );
    const double w = fabs( diff / diff_sqsum );
    const auto&  p = paramlist.at( i );
    if( diff > 0 ){
      upper.push_back( p.CentralValue()+w * p.AbsUpperError() );
      lower.push_back( p.CentralValue()-w * p.AbsLowerError() );
    } else {
      upper.push_back( p.CentralValue()-w * p.AbsLowerError() );
      lower.push_back( p.CentralValue()+w * p.AbsUpperError() );
    }
  }

  const double cen = varfunction( center.data() );
  const double up  = varfunction( upper.data() );
  const double lo  = varfunction( lower.data() );

  return Measurement( cen, up-cen, cen-lo  );
}

}/* usr */
