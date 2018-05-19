/*******************************************************************************
*
*  Filename    : GSLUtil.hpp
*  Description : Helper Utility functions using GSL
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef USERUTILS_MATHUTILS_GSLSETTINGS_HPP
#define USERUTILS_MATHUTILS_GSLSETTINGS_HPP

#include <gsl/gsl_multimin.h>
#include <gsl/gsl_multiroots.h>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_min.h>

namespace usr {

namespace gsl {

/*-----------------------------------------------------------------------------
 *  GSL setting constants
   --------------------------------------------------------------------------*/
extern const double abs_epsilon;
extern const double rel_epsilon;
extern const double mch_epsilon;
extern const unsigned max_iteration;

/*******************************************************************************
*   GSL iterator solver
*******************************************************************************/
extern void IterateSolver( gsl_multiroot_fsolver* solver );
extern void IterateSolver( gsl_root_fsolver* solver );
extern void IterateSolver( gsl_min_fminimizer* solver );
extern void IterateSolver( gsl_multimin_fminimizer* solver );

/*******************************************************************************
*   Solving a one dimension gsl_function func at func(x) = a
*******************************************************************************/
extern double Solve1D(
  gsl_function* function,
  double        yval,
  double        xmin,
  double        ymax
);

/*******************************************************************************
*   Multi variable functions
*   Using same wrapper as gsl minimizer function:
*    - double (* f) (const gsl_vector * x, void * params)
*    - size_t n
*    - void* params
*******************************************************************************/
typedef gsl_multimin_function gsl_multifunc;

/*******************************************************************************
*   Handy parameter-less gsl_multi_function function pointers
*******************************************************************************/
double sum( const gsl_vector* x, void* params);
double product( const gsl_vector* x, void* params);

/*******************************************************************************
*   Partial derivative - Wrapper for gsl_deriv function
*******************************************************************************/
extern int partial_deriv(
  gsl_multifunc* function,
  gsl_vector*    x,
  size_t         varidx,
  double         h,
  double&        result,
  double&        error
  );

// Simpler interface for self improving derivative intervals
extern double deriv(
  gsl_function* function,
  double        x
);

extern double partial_deriv(
  gsl_multifunc* function,
  gsl_vector*    x,
  size_t         varidx
);

};/* gsl */


}/* usr */

#endif/* end of include guard: USERUTILS_MATHUTILS_GSLSETTINGS_HPP */
