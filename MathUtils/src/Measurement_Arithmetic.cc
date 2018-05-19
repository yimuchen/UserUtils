/*******************************************************************************
*
*  Filename    : MeasurementArithmetic.cc
*  Description : One Line descrption of file contents
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "UserUtils/MathUtils/interface/Measurement.hpp"

#include "UserUtils/MathUtils/interface/GSLUtil.hpp"
#include "UserUtils/MathUtils/interface/StatisticsUtil.hpp"

#include <cassert>
#include <gsl/gsl_vector.h>
#include <iostream>
#include <vector>

using namespace std;

namespace usr {

/*******************************************************************************
*   General Interfacing functions with MakeMinos functions
*******************************************************************************/
Measurement
MakeMinos(
  gsl_function* nll,
  const double  initguess,
  const double  min,
  const double  max,
  const double  confidencelevel
  )
{
  double central    = initguess;
  double lowerbound = min;
  double upperbound = max;

  usr::stat::MinosError( nll, central, lowerbound, upperbound, confidencelevel );
  return Measurement( central, upperbound-central, central-lowerbound );
}

/******************************************************************************/

Measurement
MakeMinos(
  usr::gsl::gsl_multifunc* nll,
  usr::gsl::gsl_multifunc* varfunction,
  gsl_vector*              initguess,
  const double             confidencelevel,
  gsl_vector*              upperguess,
  gsl_vector*              lowerguess
  )
{
  double central;
  double upperbound;
  double lowerbound;

  usr::stat::MinosError(
    nll,
    varfunction,
    initguess,
    central,
    lowerbound,
    upperbound,
    confidencelevel,
    upperguess,
    lowerguess
    );

  return Measurement( central, upperbound-central, central - lowerbound );
}


/*******************************************************************************
*   Approximate Loglikelihood --- Linear Variance
*   Following the arguments from R. Barlow's "Asymmetric statistical errors"
*   The loglikelihood is defined as:
*      > L = - (x*x) / ( 2 * V * (1 + x* A ))
*   *  x is the distance from the central value.
*   *  V is the average variance: (sigma1 * sigma2)
*   *  A is the asymmetry: (sigma1-sigma2)/V
*
*   Small adjustments are made such that this approximation could be used to
*   carry out numerically stalbe calculations:
*
*   * The ratio between the two uncertainties are capped at 10.
*   * V has a minimal value of ~1e-16
*   * A the function is singular at x = -1/A, a exponential is used to extend
*     the positive domain of the function to infinity
*     The singularity will always occur on the side with the smaller
*     uncertainty and will always be outside said uncertainty. An exponential
*     denominator is used to instead of the linear denominator for when x is
*     outside of the arithmetic average of said uncertainty and location of
*     singularity. The exponential function is designed such that the
*     denominator and it's derivative is continuous.
*
*******************************************************************************/
static double
AugmentedDeminator( double x, const double up, const double lo )
{
  // Minimum product of both uncertainties.
  // Here we are taking the cut off absolute uncertainty used in the GLS
  // functions.
  static const double minprod = gsl::rel_epsilon* gsl::rel_epsilon;

  const double V = std::max( up * lo, minprod );
  const double A = ( up-lo ) / V;

  double ans = V * ( 1 + x * A );
  if( up > lo && A != 0 && x < ( -lo-1/A )/ 2 ){
    const double s = ( -lo-1/A )/2;
    const double b = A / ( 1+A*s );
    const double B = V * ( 1+A*s ) / exp( b*s );
    ans = B * exp( b * x );
  } else if( lo > up && A != 0 && x > ( up+1/A )/2 ){
    ans = AugmentedDeminator( -x, lo, up );
  }

  return ans;
}

double
LinearVarianceNLL( double x, const Measurement& parm )
{
  // Can only treat asymmetry up to a cap, inflating the smaller uncertainty
  // if needed
  static const double maxrelerror = 10.;


  // Getting potentially inflating uncertainties
  const double central  = parm.CentralValue();
  const double rawupper = parm.AbsUpperError();
  const double rawlower = parm.AbsLowerError();
  const double effupper = std::max( rawupper, rawlower / maxrelerror );
  const double efflower = std::max( rawlower, rawupper / maxrelerror );

  const double num = ( x- central ) * ( x - central );
  const double de  = AugmentedDeminator( x-central, effupper, efflower );
  const double ans = 0.5  * num / de;

  return ans;
}

/*******************************************************************************
*   Hidden method for Making a master uncorrelated member functions
*******************************************************************************/
struct uncorrelatedparam
{
  const vector<Measurement>* listptr;
  double                     (* nll)( double, const Measurement& );
};

static double
UncorrelatedNLL( const gsl_vector* x, void* param )
{
  uncorrelatedparam* p = (uncorrelatedparam*)param;
  double ans           = 0;

  for( size_t i = 0; i < p->listptr->size(); ++i ){
    const Measurement& thisp = p->listptr->at( i );
    const double thisx       = gsl_vector_get( x, i );
    ans += p->nll( thisx, thisp );
  }

  return ans;
}

/*******************************************************************************
*   Main function for interfacing with user
*******************************************************************************/
Measurement
SumUncorrelated(
  const vector<Measurement>& paramlist,
  const double confidencelevel,
  double ( *nll )( double, const Measurement& )
  )
{
  const unsigned dim = paramlist.size();

  usr::gsl::gsl_multifunc masternll;
  uncorrelatedparam param = { &paramlist, nll };

  masternll.f      = &UncorrelatedNLL;
  masternll.n      = dim;
  masternll.params = &param;

  usr::gsl::gsl_multifunc varfunction;
  varfunction.f      = &usr::gsl::sum;
  varfunction.n      = dim;
  varfunction.params = nullptr;// parameter-less

  gsl_vector* initguess  = gsl_vector_alloc( dim );
  gsl_vector* upperguess = gsl_vector_alloc( dim );
  gsl_vector* lowerguess = gsl_vector_alloc( dim );

  double sqrt_sumsq_up = 0;
  double sqrt_sumsq_lo = 0;

  for( size_t i = 0; i < paramlist.size(); ++i ){
    const Measurement& p = paramlist.at( i );
    gsl_vector_set( initguess, i, p.CentralValue() );
    sqrt_sumsq_up += p.AbsUpperError() * p.AbsUpperError();
    sqrt_sumsq_lo += p.AbsLowerError() * p.AbsLowerError();
  }

  // Using symmetric approximation to get initial guess
  sqrt_sumsq_lo = sqrt( sqrt_sumsq_lo );
  sqrt_sumsq_up = sqrt( sqrt_sumsq_up );

  for( size_t i = 0; i < paramlist.size(); ++i ){
    const Measurement& p = paramlist.at( i );
    const double shift_up = sqrt_sumsq_up / p.AbsUpperError();
    const double shift_lo = sqrt_sumsq_lo / p.AbsLowerError();
    gsl_vector_set( upperguess, i,
      p.CentralValue() + p.AbsUpperError()/shift_up );
    gsl_vector_set( lowerguess, i,
      p.CentralValue() - p.AbsLowerError()/shift_lo );
  }

  // Calling MinosError function for acutal computation
  Measurement ans = MakeMinos(
    &masternll,
    &varfunction,
    initguess,
    confidencelevel,
    upperguess,
    lowerguess
    );

  // Releaing gsl vector calls
  gsl_vector_free( initguess );
  gsl_vector_free( upperguess );
  gsl_vector_free( lowerguess );

  return ans;
}

/******************************************************************************/

Measurement
ProdUncorrelated(
  const std::vector<Measurement>& paramlist,
  const double confidencelevel,
  double ( *nll )( double, const Measurement& )
  )
{
  const unsigned dim = paramlist.size();

  // Getting normalized version of list
  double prod = 1.;
  vector<Measurement> normlist;

  for( const auto& p : paramlist ){
    prod *= p.CentralValue();
    normlist.push_back( p.NormParam() );
  }

  usr::gsl::gsl_multifunc masternll;
  uncorrelatedparam param = { &normlist, nll };

  masternll.f      = &UncorrelatedNLL;
  masternll.n      = dim;
  masternll.params = &param;

  usr::gsl::gsl_multifunc varfunction;
  varfunction.f      = &usr::gsl::product;
  varfunction.n      = dim;
  varfunction.params = nullptr;// parameter-less

  gsl_vector* initguess  = gsl_vector_alloc( dim );
  gsl_vector* upperguess = gsl_vector_alloc( dim );
  gsl_vector* lowerguess = gsl_vector_alloc( dim );

  for( size_t i = 0; i < normlist.size(); ++i ){
   const Measurement& p = normlist.at( i );
   gsl_vector_set( initguess,  i, p.CentralValue() );
   // Don't attempt to scale the guess, works better for products
   gsl_vector_set( upperguess, i, p.CentralValue()+p.AbsUpperError() );
   gsl_vector_set( lowerguess, i, p.CentralValue()-p.AbsLowerError() );
  }

  // Calling MinosError function for acutal computation
  Measurement ans = MakeMinos(
    &masternll,
    &varfunction,
    initguess,
    confidencelevel,
    upperguess,
    lowerguess
    );

  // Releaing gsl vector calls
  gsl_vector_free( initguess );
  gsl_vector_free( upperguess );
  gsl_vector_free( lowerguess );

  return ans * prod;
}

}/* usr */
