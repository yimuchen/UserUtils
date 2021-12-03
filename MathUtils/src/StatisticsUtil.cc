/**
 * @file    StatisticsUtil.cc
 * @brief   Implementation of common statistical number and functions.
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/MathUtils/interface/RootMathTools/DefaultEngines.hpp"
#include "UserUtils/MathUtils/interface/StatisticsUtil.hpp"
#else
#include "UserUtils/MathUtils/RootMathTools/DefaultEngines.hpp"
#include "UserUtils/MathUtils/StatisticsUtil.hpp"
#endif

#include <cassert>
#include <iostream>

#include "Math/Derivator.h"
#include "Math/ProbFuncMathCore.h"

namespace usr
{

namespace stat
{

/**
 * @brief CDF of the normal distribution.
 *
 * Implemented in the ROOT Math library.
 */
double
NormalCDF( const double x )
{
  return ROOT::Math::normal_cdf( x );
}


/**
 * @brief Translating the informal but common phrase of "x sigmas" into
 *        actual confidence levels.
 */
double
GetConfidenceLevel( const double sigmainterval )
{
  return NormalCDF( sigmainterval )-NormalCDF( -sigmainterval );
}


/**
 * @brief Translatign the more formal term of "a confidence level of x", into a
 *        more immediately understandable "x sigma interval"
 */
double
GetSigmaInterval( const double confidencelevel )
{
  // Trying to get a larger than 10 sigma is impractical
  static const double   maxinterval = 10;
  DefaultSolver1D       solver;
  ROOT::Math::Functor1D f( &GetConfidenceLevel );
  return solver.SolveForY( f, confidencelevel, 0, maxinterval );
}


extern const double onesigma_level = GetConfidenceLevel( 1 );

extern const double twosigma_level = GetConfidenceLevel( 2 );


/**
 * @brief getting the difference in NLL required for a given sigma interval.
 * @details Essentially calculating the difference in NLL from the normal
 * distribution: for a sigma interval of \f$x\f$, the return value would be
 *  \f$x^{2}/2\f$
 */
double
DeltaNLLFromSigma( const double sigma )
{
  return sigma * sigma / 2;
}


/**
 * @brief getting the difference in NLL required for a certain confidence level.
 * @details First convert the confidence interval into a sigma interval, then
 * returning the answer.
 */
double
DeltaNLLFromConfidence( const double confidence )
{
  return DeltaNLLFromSigma( GetSigmaInterval( confidence ) );
}


/**
 * @brief Generic routine for calculating minos uncertainties for 1D functions.
 *
 * Given a 1D function acting as the @NLL function for some parameter, the
 * MinosError will use the min/max brackets to return the central value (where
 * the @NLL function was minimized) and the upper and lower uncertainty edges,
 * a.k.a  where the @NLL function deviates from the minimum value by some number
 * K. The number K is determined by the working "confidence level" of the
 * function.
 */
int
MinosError( const ROOT::Math::IGenFunction& nllfunction,
            double&                         guess,
            double&                         min,
            double&                         max,
            const double                    confidencelevel )
{
  DefaultMinimizer1D minimizer;
  DefaultSolver1D    solver;

  minimizer.Minimize( nllfunction, min, max );
  guess = minimizer.XMinimum();

  const double nlltarget = minimizer.FValMinimum()
                           +DeltaNLLFromConfidence( confidencelevel );

  double max_temp = solver.SolveForY( nllfunction, nlltarget, guess, max );
  double min_temp = solver.SolveForY( nllfunction, nlltarget, min, guess );

  min = std::min( min_temp, max_temp );
  max = std::max( min_temp, max_temp );

  return 0;
}


/**
 * @brief Generic routine for calculating the minos uncertainty of a N parameter
 *        system.
 *
 * Given an N dimensional parameter system governed by a @NLL function
 * `nllfunction`, the MinosError routine will attempt to calculate the minos
 * uncertainty of the extended parameter defined by the `varfunction`.
 *
 * The `initguess` is mandatory for initializing the parameter guessing, while
 * the `central`, `min`, `max` parameters are no longer used for initializing or
 * bracketing, just for storing the return result. The use can provide an
 *initial
 * guess for parameter inputs that will yield the higher or lower edges if
 * needed, otherwise the guess is simply stepping the minimum value by a value
 *of
 * 0.01.
 */
extern int
MinosError( const ROOT::Math::IMultiGenFunction& nllfunction,
            const ROOT::Math::IMultiGenFunction& varfunction,
            const double*                        initguess,
            double&                              central,
            double&                              min,
            double&                              max,
            const double                         confidencelevel,
            const double*                        upperguess,
            const double*                        lowerguess  )
{
  // Static variables and other common declarations
  static const double defaultstep = 0.01;

  // Asserts for input functions
  assert( nllfunction.NDim() == varfunction.NDim() );

  // Common variables.
  const size_t dim = nllfunction.NDim();

  // Step 1: Finding the minimum value
  DefaultMinimizer minimizer;
  minimizer.SetFunction( nllfunction );

  for( size_t i = 0; i < dim; ++i ){// Setting up the initial values
    const double x        = initguess[i];
    double       stepsize = defaultstep;
    if( upperguess ){
      stepsize = std::min( defaultstep * fabs( upperguess[i]-x ), stepsize );
    }
    if( lowerguess ){
      stepsize = std::min( defaultstep * fabs( lowerguess[i]-x ), stepsize );
    }

    minimizer.SetVariable( i, "", x, stepsize );
  }

  minimizer.Minimize();

  const double nllmin    = minimizer.MinValue();
  const double targetnll = nllmin+DeltaNLLFromConfidence( confidencelevel );
  central = varfunction( minimizer.X() );

  // Step 2: Setting up the function for root finding
  // The original variables take up the first 'dim' variables in the solver
  // The final variable is the Lagrange multiplier lambda
  DefaultSolverND                  solver;
  std::vector<ROOT::Math::Functor> functor_list;

  for( size_t i = 0; i < dim; ++i ){
    auto div = [&nllfunction, &varfunction, i, dim]
               ( const double* x )->double {
                 const double nlldiv = ROOT::Math::Derivator::Eval( nllfunction
                                                                    ,
                                                                    x,
                                                                    i );
                 const double vardiv = ROOT::Math::Derivator::Eval( varfunction
                                                                    ,
                                                                    x,
                                                                    i );
                 return nlldiv-x[dim] * vardiv;
               };
    functor_list.push_back( ROOT::Math::Functor( div, dim+1 ) );
    solver.AddFunction( functor_list.back() );
  }

  auto nll_check = [&nllfunction, targetnll]
                   ( const double* x )->double {
                     return nllfunction( x )-targetnll;
                   };
  functor_list.push_back( ROOT::Math::Functor( nll_check, dim+1 ) );
  solver.AddFunction( functor_list.back() );

  // Step 3. Begin the root finding from initial guess
  std::vector<double> guess;

  for( size_t i = 0; i < dim; ++i ){
    guess.push_back( upperguess ? upperguess[i] :
                     minimizer.X()[i]+defaultstep );
  }

  guess.push_back( 1 );
  solver.Solve( guess.data(), 1e8 );

  const double boundary1 = varfunction( solver.X() );

  //  Step 3.5 Getting the second root
  guess.clear();

  for( size_t i = 0; i < dim; ++i ){
    const double best = minimizer.X()[i];
    const double diff = solver.X()[i]-best;
    guess.push_back( lowerguess ? lowerguess[i] :  ( best-diff * 0.01 ) );
  }

  guess.push_back( -solver.X()[dim] );// New lambda is the negative for first
  solver.Solve( guess.data(), 1e8 );

  const double boundary2 = varfunction( solver.X() );

  // Cleaning up - Storing outputs
  min = std::min( boundary1, boundary2 );
  max = std::max( boundary1, boundary2 );

  return 0;
}


/**
 * @brief ROOT::Math compatible interface for the @NLL of a Gaussian function.
 */
double
GaussianNLL::DoEval( const double x ) const
{
  return ( x-mean ) * ( x-mean ) / ( 2 * sigma * sigma );
}


/**
 * @brief ROOT::Math compatible interface for the @NLL of a binomial
 *measurement.
 */
double
BinomialNLL::DoEval( const double x ) const
{
  return -passed* log( x )-( total-passed ) * log( 1-x );
}


/**
 * @brief ROOT::Math compatible inferface for the @NLL of a poisson measurement.
 */
double
PoissonNLL::DoEval( const double x ) const
{
  return x-obs * log( x );
}

}/* stat */

}/* usr */
