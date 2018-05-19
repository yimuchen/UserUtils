/*******************************************************************************
*
*  Filename    : StatisticsUtil.cc
*  Description : Implementation of statistics utility functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "UserUtils/MathUtils/interface/GSLUtil.hpp"

#include <gsl/gsl_min.h>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_sf_erf.h>

#include <cassert>
#include <iostream>

namespace usr {

namespace stat {

/*-----------------------------------------------------------------------------
 *  Sigma interval -- confidenc level conversion functions
 *  The definition of the GSL error function:
 *   > https://www.gnu.org/software/gsl/manual/html_node/
 *   > Error-Function.html#Error-Function
 *       erf(x) = (2/\sqrt(\pi)) \int_0^x dt \exp(-t^2)
 *  Conversion to CDF of Normal distribution:
 *   > https://en.wikipedia.org/wiki/Normal_distribution
 *       CDF(x) = 1/2 * ( 1 + erf(x/sqrt(2)))
   --------------------------------------------------------------------------*/
double
NormalCDF( const double x )
{
  return 0.5 * ( 1 + gsl_sf_erf( x/sqrt( 2. ) ) );
}
/*----------------------------------------------------------------------------*/
double
GetConfidenceLevel( const double sigmainterval )
{
  return NormalCDF( sigmainterval ) - NormalCDF( -sigmainterval );
}
/*----------------------------------------------------------------------------*/
double
GetSigmaInterval( const double confidencelevel )
{
  // Trying to get a larger than 10 sigma is impractical
  static const double maxinterval = 10;

  gsl_root_fsolver* solver = gsl_root_fsolver_alloc( gsl_root_fsolver_brent );

  gsl_function func;
  func.function = []( double x, void* y ){
                    return GetConfidenceLevel( x ) - *( (const double*)y );
                  };
  func.params = const_cast<double*>( &confidencelevel );
  gsl_root_fsolver_set( solver, &func, 0, maxinterval );
  gsl::IterateSolver( solver );

  const double ans = gsl_root_fsolver_root( solver );
  gsl_root_fsolver_free( solver );

  return ans;
}

extern const double onesigma_level = GetConfidenceLevel( 1 );

extern const double twosigma_level = GetConfidenceLevel( 2 );


/*******************************************************************************
*   General MinosError computing functions
*******************************************************************************/
double
DeltaNLLFromSigma( const double sigma )
{
  return sigma*sigma/2;
}

double
DeltaNLLFromConfidence( const double confidence )
{
  return DeltaNLLFromSigma( GetSigmaInterval( confidence ) );
}

/******************************************************************************/

int
MinosError(
  gsl_function* nllfunction,
  double&       guess,
  double&       min,
  double&       max,
  const double  confidencelevel
  )
{
  // Getting central value via minimization
  gsl_min_fminimizer* solver = gsl_min_fminimizer_alloc( gsl_min_fminimizer_brent );
  gsl_min_fminimizer_set( solver, nllfunction, guess, min, max );

  usr::gsl::IterateSolver( solver );

  // saving central value to initial guess variable
  guess = gsl_min_fminimizer_x_minimum( solver );

  gsl_min_fminimizer_free( solver );

  // Getting error via solving nll(x) = nll(x_min)+ sigmainterval^2 / 2
  double deltanll  = DeltaNLLFromConfidence( confidencelevel );
  double nllmin    = nllfunction->function( guess, nllfunction->params );
  double nlltarget = nllmin + deltanll;

  double max_temp = usr::gsl::Solve1D( nllfunction, nlltarget, guess, max );
  double min_temp = usr::gsl::Solve1D( nllfunction, nlltarget, min, guess );

  max = std::max( max_temp, min_temp );
  min = std::min( max_temp, min_temp );

  return 0;
}

/******************************************************************************/

int
MinosError(
  usr::gsl::gsl_multifunc* nllfunction,
  usr::gsl::gsl_multifunc* varfunction,
  gsl_vector*              initguess,
  double&                  central,
  double&                  min,
  double&                  max,
  const double             confidencelevel,
  gsl_vector*              upperguess,
  gsl_vector*              lowerguess
  )
{
  // Static constant setups
  static const double defaultstep = 0.01;

  // Step 1: Finding the minimum value
  const size_t dim                = nllfunction->n;
  gsl_multimin_fminimizer* solver = gsl_multimin_fminimizer_alloc( gsl_multimin_fminimizer_nmsimplex2, dim );
  gsl_vector* step                = gsl_vector_alloc( dim );
  gsl_vector* bestinput           = gsl_vector_alloc( dim );

  for( size_t i = 0; i < dim; ++i ){
    double stepsize = defaultstep;// Default step size
    const double x  = gsl_vector_get( initguess, i );
    if( upperguess ){
      const double y = gsl_vector_get( upperguess, i );
      stepsize = std::min( stepsize, fabs( y-x )*defaultstep );
    } else if( lowerguess ){
      const double y = gsl_vector_get( upperguess, i );
      stepsize = std::min( stepsize, fabs( y-x )*defaultstep );
    }
    gsl_vector_set( step, i, stepsize );
  }

  gsl_multimin_fminimizer_set( solver, nllfunction, initguess, step );
  usr::gsl::IterateSolver( solver );
  gsl_vector_memcpy( bestinput, solver->x );
  central = varfunction->f( bestinput, varfunction->params );
  double nllmin = nllfunction->f( bestinput, nllfunction->params );

  gsl_vector_free( step );
  gsl_multimin_fminimizer_free( solver );

  // Step 2: Setting up the function for root finding
  // The original variables take up the first 'dim' variables in the solver
  // The final variable is the Lagrange multiplier lamda
  struct minosparam
  {
    usr::gsl::gsl_multifunc* nllfunc;
    usr::gsl::gsl_multifunc* varfunc;
    double                   yval;
  };
  gsl_multiroot_fsolver* errsolver
    = gsl_multiroot_fsolver_alloc( gsl_multiroot_fsolver_hybrids, dim + 1 );

  gsl_multiroot_function minosfunc;
  minosfunc.f
    = []( const gsl_vector* input, void* p, gsl_vector* output ){
        // Getting the original parameters (without the lagrange multipler)
        gsl_vector* reduced = gsl_vector_alloc( input->size - 1 );

        for( size_t i = 0; i < reduced->size; ++i ){
          gsl_vector_set( reduced, i, gsl_vector_get( input, i ) );
        }

        // The lagrange multiplier is the last variable
        double lambda = gsl_vector_get( input, input->size - 1 );

        const minosparam* param = (minosparam*)p;

        // First dim outputs are the partial derivative equations
        for( size_t i = 0; i < reduced->size; ++i ){
          double nlldiv = usr::gsl::partial_deriv( param->nllfunc, reduced, i );
          double vardiv = usr::gsl::partial_deriv( param->varfunc, reduced, i );
          gsl_vector_set( output, i, nlldiv - lambda*vardiv );
        }

        // Final is the NNL value equation
        double nllval = param->nllfunc->f( reduced, param->nllfunc->params );
        gsl_vector_set( output, input->size-1, nllval - param->yval );

        return (int)GSL_SUCCESS;
      };

  minosfunc.n = dim+1;
  minosparam tmp = {
    nllfunction,
    varfunction,
    nllmin + DeltaNLLFromConfidence( confidencelevel )
  };
  minosfunc.params = &tmp;

  // Step 3. Begin the root finding from initial guess
  gsl_vector* errinitguess = gsl_vector_alloc( dim+1 );
  gsl_vector* finalinput   = gsl_vector_alloc( dim );

  for( size_t i = 0; i < dim; ++i ){
    const double setval = upperguess ? gsl_vector_get( upperguess, i ) :
                          gsl_vector_get( bestinput, i )+defaultstep;
    gsl_vector_set( errinitguess, i,  setval );
  }

  gsl_vector_set( errinitguess, dim, 1 );// Initial guess for lambda is 1
  gsl_multiroot_fsolver_set( errsolver, &minosfunc, errinitguess );
  usr::gsl::IterateSolver( errsolver );

  for( size_t i = 0; i < finalinput->size; ++i ){
    gsl_vector_set( finalinput, i, gsl_vector_get( errsolver->x, i ) );
  }

  const double boundary1 = varfunction->f( finalinput, varfunction->params );

  //  Step 3.5 Getting the second root
  for( size_t i = 0; i < dim; ++i ){
    const double best   = gsl_vector_get( bestinput, i );
    const double diff   = gsl_vector_get( finalinput, i ) - best;
    const double setval = lowerguess ? gsl_vector_get( lowerguess, i ) :
                          ( best - diff * 0.01 );
    gsl_vector_set( errinitguess, i, setval );
  }

  // New lambda is the negative of the first lambda
  gsl_vector_set( errinitguess, dim, -gsl_vector_get( errinitguess, dim) );
  gsl_multiroot_fsolver_set( errsolver, &minosfunc, errinitguess );
  usr::gsl::IterateSolver( errsolver );

  for( size_t i = 0; i < finalinput->size; ++i ){
    gsl_vector_set( finalinput, i, gsl_vector_get( errsolver->x, i ) );
  }

  const double boundary2 = varfunction->f( finalinput, varfunction->params );

  // Cleaning up - Storing outputs
  min = std::min( boundary1, boundary2 );
  max = std::max( boundary1, boundary2 );

  gsl_vector_free( errinitguess );
  gsl_vector_free( finalinput   );
  gsl_multiroot_fsolver_free( errsolver );

  return 0;
}


/*******************************************************************************
*   Common NLL functions
*******************************************************************************/
double
GaussianNLL( double x, void* params )
{
  double mean  = ( (double*)params )[0];
  double sigma = ( (double*)params )[1];
  return ( x-mean )*( x-mean )/( 2*sigma*sigma );
}

/******************************************************************************/

double
BinomialNLL( double x, void* params )
{
  double passed = ( (double*)params )[0];
  double total  = ( (double*)params )[1];
  return -passed* log( x ) - ( total-passed ) * log( 1 - x );
}

/******************************************************************************/

double
PoissonNLL( double x, void* params )
{
  double obs = ( (double*)params )[0];
  return x - obs*log( x );
}

}/* stat */

}/* usr */
