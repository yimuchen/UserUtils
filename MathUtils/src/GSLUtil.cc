/*******************************************************************************
*
*  Filename    : GSLUtil.cc
*  Description : Implementation of GSL enhancing functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "UserUtils/MathUtils/interface/GSLUtil.hpp"

#include <cassert>
#include <cmath>
#include <gsl/gsl_deriv.h>
#include <iostream>

namespace usr {

namespace gsl {

/*-----------------------------------------------------------------------------
 *  GSL settings constants
   --------------------------------------------------------------------------*/
extern const double abs_epsilon     = 1e-8;
extern const double rel_epsilon     = 1e-6;
extern const double mch_epsilon     = 1-std::nexttoward( 1, 0 );
extern const unsigned max_iteration = 1e+4;

/*-----------------------------------------------------------------------------
 *  GSL solver iteration
   --------------------------------------------------------------------------*/
void
IterateSolver( gsl_multiroot_fsolver* solver )
{
  const unsigned dim                = solver->f->size;
  const unsigned this_max_iteration = max_iteration * dim;
  int status;
  size_t iteration = 0;

  do {
    iteration++;
    status = gsl_multiroot_fsolver_iterate( solver );
    // if( status ){ break; }
    status = gsl_multiroot_test_residual( solver->f, abs_epsilon );
  } while( status == GSL_CONTINUE && iteration < this_max_iteration );
}

/*----------------------------------------------------------------------------*/

void
IterateSolver( gsl_root_fsolver* solver )
{
  int status;
  size_t iter = 0;
  double lo;
  double hi;

  do {
    iter++;
    status = gsl_root_fsolver_iterate( solver );
    lo     = gsl_root_fsolver_x_lower( solver );
    hi     = gsl_root_fsolver_x_upper( solver );
    status = gsl_root_test_interval( lo, hi, 0, rel_epsilon );
    if( status == GSL_SUCCESS ){
      break;
    }
  } while( status == GSL_CONTINUE && iter < max_iteration );
}

/*----------------------------------------------------------------------------*/

void
IterateSolver( gsl_min_fminimizer* solver )
{
  int status;
  size_t iter = 0;
  double lo;
  double hi;

  do {
    iter++;
    status = gsl_min_fminimizer_iterate( solver );
    lo     = gsl_min_fminimizer_x_lower( solver );
    hi     = gsl_min_fminimizer_x_upper( solver );

    status = gsl_min_test_interval( lo, hi, 0, rel_epsilon );

  } while( status == GSL_CONTINUE && iter < max_iteration );

}

/*----------------------------------------------------------------------------*/

void
IterateSolver( gsl_multimin_fminimizer* solver )
{
  int status;
  unsigned iter = 0;
  double size;// size of stepping vector

  do {
    iter++;
    status = gsl_multimin_fminimizer_iterate( solver );

    if( status ){ break; }

    size   = gsl_multimin_fminimizer_size( solver );
    status = gsl_multimin_test_size( size, abs_epsilon );

  } while( status == GSL_CONTINUE && iter < max_iteration * solver->x->size );
}

/*******************************************************************************
*   1D function solving at y = dedicated value
*******************************************************************************/
double
Solve1D(
  gsl_function* func,
  double        yval,
  double        xmin,
  double        xmax
  )
{
  struct argparam
  {
    gsl_function* original_func;
    double        yval;
  };
  argparam newparams = { func, yval };

  gsl_function augfunc;
  augfunc.function = []( double x, void* param ){
                       argparam* p = (argparam*)param;
                       return p->original_func->function( x, p->original_func->params ) - p->yval;
                     };
  augfunc.params = &newparams;

  gsl_root_fsolver* solver = gsl_root_fsolver_alloc( gsl_root_fsolver_brent );
  gsl_root_fsolver_set( solver, &augfunc, xmin, xmax );

  IterateSolver( solver );

  double ans = gsl_root_fsolver_root( solver );

  gsl_root_fsolver_free( solver );

  return ans;
}

/*******************************************************************************
*   Handy parameter-less gsl_multifunc
*******************************************************************************/
double
sum( const gsl_vector* x, void* )
{
  double ans = 0;

  for( size_t i = 0; i < x->size; ++i ){
    ans += gsl_vector_get( x, i );
  }

  return ans;
}

/*----------------------------------------------------------------------------*/

double
product( const gsl_vector* x, void* )
{
  double ans = 1;

  for( size_t i = 0; i < x->size; ++i ){
    ans *= gsl_vector_get( x, i );
  }

  return ans;
}

/*******************************************************************************
*   Partial derivative function
*******************************************************************************/
int
partial_deriv(
  gsl_multifunc* function,
  gsl_vector*    x,
  size_t         varidx,
  double         h,
  double&        result,
  double&        error
  )
{
  struct projected_param
  {
    gsl_multifunc* original_func;
    gsl_vector*    original_x;
    size_t         varidx;
  };

  gsl_function f;

  f.function =
    []( double x, void* p )
    {
      projected_param* param = (projected_param*)p;

      gsl_vector* newvec = gsl_vector_alloc( param->original_x->size );

      for( size_t idx = 0; idx < param->original_x->size; ++idx ){
        const double newx = idx==param->varidx ?  x:
                            gsl_vector_get( param->original_x, idx );
        gsl_vector_set( newvec, idx, newx );
      }

      double ans = param->original_func->f(
        newvec, param->original_func->params
      );

      gsl_vector_free( newvec );

      return ans;
    };

  projected_param p = { function, x, varidx };

  f.params = &p;

  double projx = gsl_vector_get( x, varidx );
  int retans   = gsl_deriv_central( &f, projx, h, &result, &error );
  return retans;
}

/******************************************************************************/
// Simpler interfacting functions
double
deriv( gsl_function* F, double x )
{
  double stepsize = sqrt(abs_epsilon);
  double result = 0, error = 1;

  do {
    gsl_deriv_central( F, x, stepsize, &result, &error );
    stepsize /= 2;
  } while(
    fabs( error ) > std::max( abs_epsilon, result*rel_epsilon ) &&
    stepsize > abs_epsilon
    );

  return result;
}

double
partial_deriv(
  gsl_multifunc* function,
  gsl_vector*    x,
  size_t         varidx
  )
{
  double stepsize = sqrt(abs_epsilon);
  double result = 0, error = 1;

  do {
    partial_deriv( function, x, varidx, stepsize, result, error );
    stepsize /= 2;
  } while(
    fabs( error ) > std::max( abs_epsilon, result*rel_epsilon ) &&
    stepsize > abs_epsilon
    );

  return result;
}

};/* namespace gsl  */

};/* namespace usr */
