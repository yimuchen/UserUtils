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

namespace usr {

/**
 * @brief interface to the Single variable version of the MinosError() function.
 */
Measurement
MakeMinos(
  const ROOT::Math::IGenFunction& nll,
  const double                    min,
  const double                    max,
  const double                    confidencelevel )
{
  double central    = min + max / 2;
  double lowerbound = min;
  double upperbound = max;

  usr::stat::MinosError( nll, central, lowerbound, upperbound, confidencelevel );
  return Measurement( central, upperbound-central, central-lowerbound );
}

/**
 * @brief interface to the \f$R^{n}\rightarrow R\f$ version of the MinosError()
 *        function
 */
Measurement
MakeMinos(
  const ROOT::Math::IMultiGenFunction& nllfunction,
  const ROOT::Math::IMultiGenFunction& varfunction,
  const double*                        initguess,
  const double                         confidencelevel,
  const double*                        upperguess,
  const double*                        lowerguess )
{
  double central;
  double upperbound;
  double lowerbound;

  usr::stat::MinosError( nllfunction
                       , varfunction
                       , initguess
                       , central
                       , lowerbound
                       , upperbound
                       , confidencelevel
                       , upperguess
                       , lowerguess );

  return Measurement( central
                    , fabs( upperbound-central )
                    , fabs( central - lowerbound ) );
}

/**
 * @brief Approximate @NLL function for a measurements of
 *        \f$x^{+\sigma_+}_{-\sigma_i}\f$
 *
 * The approximation for an @NLL function is a variation of the one recommended
 * in
 * [R. Barlow's "Asymmetric statistical errors"](https://arxiv.org/abs/physics/0406120),
 * In the form of:
 *
 * \f[
 *  \mathrm{NLL}(x) = \frac{x^{2}}{ 2V(1+Ax)}
 * \f]
 *
 * with \f$V\f$ being the average variance \f$\sigma_+ \sigma_-\f$, and \f$A\f$
 * being the variance asymmetry \f$ (\sigma_{+} - \sigma_{-})/V \f$.
 *
 * A few adjustments are made to ensure the stability of the calculations.
 * - The ratio between the two uncertainties would have a hard limit of 10. The
 *   smaller uncertainty would be inflated if extreme asymmetries are present.
 * - The average variance would have an minimum value of $10^{-16}$ which
 *   should cover most cases where uncertainty calculations are required. Test
 *   have shown that the calculation would not be affected by such limits if
 *   the there are some dominate uncertainties.
 * - To avoid the singularity at \f$x=-1/A\f$, an exponential function would be
 *   used instead for the linear function for the denominator in the @NLL
 *   function when x is past half-way from the central value to the original
 *   singular point, such that the approximation would be extended to an
 *   infinite domain. The exponential function is designed such that the
 *   denominator and its derivative is continuous.
 */

double
LinearVarianceNLL( const double x, const Measurement& m )
{
  static const double maxrelerror = 10.;
  static double (* AugmentedDenominator)( double, const double, const double )
    = []( double x, const double up, const double lo )->double {
        static const double minprod = 1e-12;

        const double V = std::max( up * lo, minprod );
        const double A = ( up-lo ) / V;

        double ans = V * ( 1 + x * A );
        if( up > lo && A != 0 && x < ( -lo-1/A )/ 2 ){
          const double s = ( -lo-1/A )/2;
          const double b = A / ( 1+A*s );
          const double B = V * ( 1+A*s ) / exp( b*s );
          ans = B * exp( b * x );
        } else if( lo > up && A != 0 && x > ( up+1/A )/2 ){
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

  const double num = ( x- central ) * ( x - central );
  const double de  = AugmentedDenominator( x-central, effupper, efflower );
  const double ans = 0.5  * num / de;

  return ans;
}


extern Measurement
EvaluateUncorrelated(
  const std::vector<Measurement>& m_list,
  const ROOT::Math::IMultiGenFunction& varfunction,
  const double confidencelevel,
  double (* nll )( double, const Measurement& ) )
{
  auto masternll = [&m_list, nll]( const double* x )->double {
                     double ans = 0;

                     for( unsigned i = 0; i < m_list.size(); ++i ){
                       ans = ans + nll( x[i], m_list.at( i ) );
                     }

                     return ans;
                   };
  ROOT::Math::Functor nllfunction( masternll, m_list.size() );

  std::vector<double> init;
  std::vector<double> upperguess;
  std::vector<double> lowerguess;
  double diff_sqsum = 0;

  for( const auto m : m_list ){
    init.push_back( m.CentralValue() );
  }

  for( unsigned i = 0; i < m_list.size(); ++i  ){
    const double diff = ROOT::Math::Derivator::Eval( varfunction
                                                   , init.data(), i );
    diff_sqsum += diff *diff;
  }

  diff_sqsum = std::sqrt( diff_sqsum );

  // Guess for the boundaries for the input required for getting the upper and
  // lower uncertainties based on the derivatives of the variable function.
  for( unsigned i = 0; i < m_list.size(); ++i ){
    const double diff = ROOT::Math::Derivator::Eval( varfunction
                                                   , init.data(), i );
    const double w = fabs( diff / diff_sqsum );
    if( diff > 0 ){
      upperguess.push_back( init.at( i ) + w*m_list.at( i ).AbsUpperError() );
      lowerguess.push_back( init.at( i ) - w*m_list.at( i ).AbsLowerError() );
    } else {
      upperguess.push_back( init.at( i ) - w*m_list.at( i ).AbsLowerError() );
      lowerguess.push_back( init.at( i ) + w*m_list.at( i ).AbsUpperError() );
    }
  }

  return MakeMinos( nllfunction
                  , varfunction
                  , init.data()
                  , confidencelevel
                  , upperguess.data()
                  , lowerguess.data() );
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
SumUncorrelated(
  const vector<Measurement>& m_list,
  const double confidencelevel,
  double (* nll )( double, const Measurement& ) )
{
  const unsigned dim = m_list.size();
  auto Sum = [dim]( const double* x ){
               double ans = 0;

               for( unsigned i = 0; i < dim; ++i ){
                 ans += x[i];
               }

               return ans;
             };

  return EvaluateUncorrelated( m_list
                             , ROOT::Math::Functor( Sum, dim )
                             , confidencelevel
                             , nll );
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
ProdUncorrelated(
  const std::vector<Measurement>& m_list,
  const double confidencelevel,
  double (* nll )( double, const Measurement& ) )
{
  const unsigned dim = m_list.size();
  auto Prod = [dim]( const double* x ){
                double ans = 1;

                for( unsigned i = 0; i < dim; ++i ){
                  ans *= x[i];
                }

                return ans;
              };

  double prod = 1.;
  vector<Measurement> normlist;

  for( const auto& p : m_list ){
    prod *= p.CentralValue();
    normlist.push_back( p.NormParam() );
  }

  return prod*EvaluateUncorrelated( normlist
                                  , ROOT::Math::Functor( Prod, dim )
                                  , confidencelevel
                                  , nll );
}


/*
   Measurement
   LazyMeasurementFunction(
   const std::vector<Measurement>& paramlist,
   double (*                       function )( const std::vector<double>& ) )
   {
   static double (* static_pointer)( const std::vector<double>& );

   // Restructing to GSL compilent data teyp
   static_pointer = function;

   auto gsl_wrap = []( const gsl_vector* x, void* )->double {
                    std::vector<double> input;

                    for( unsigned i = 0; i  < x->size; ++i ){
                      input.push_back( gsl_vector_get( x, i ) );
                    }

                    return static_pointer( input );
                  };

   usr::gsl::gsl_multifunc gsl_min;
   gsl_min.n      = paramlist.size();
   gsl_min.params = nullptr;
   gsl_min.f      = gsl_wrap;


   // Resetting up the calculation center
   gsl_vector* input_center = gsl_vector_alloc( paramlist.size() );

   for( unsigned i = 0; i < paramlist.size(); ++i ){
    gsl_vector_set( input_center, i, paramlist.at( i ).CentralValue() );
   }


   // Summing over the  relavent uncertainties
   const double central = gsl_wrap( input_center, nullptr );
   double err_up        = 0;
   double err_down      = 0;

   for( unsigned i = 0; i < paramlist.size(); ++i ){
    const double diff = usr::gsl::partial_deriv( &gsl_min, input_center, i );
    if( diff > 0 ){
      err_up += diff * diff * paramlist.at( i ).AbsUpperError()
 *  paramlist.at( i ).AbsUpperError();
      err_down += diff * diff * paramlist.at( i ).AbsLowerError()
 *  paramlist.at( i ).AbsLowerError();
    } else {
      err_up += diff * diff * paramlist.at( i ).AbsLowerError()
 *  paramlist.at( i ).AbsLowerError();
      err_down += diff * diff * paramlist.at( i ).AbsUpperError()
 *  paramlist.at( i ).AbsUpperError();
    }
   }

   err_up   = std::sqrt( err_up );
   err_down = std::sqrt( err_down );

   // Tidying up
   gsl_vector_free( input_center );

   return Measurement( central, err_up, err_down );

   }
 */

}/* usr */
