/**
 * @file RootMathTools_DefaultEngines.cc
 * @author Yi-Mu "Enoch" Chen
 * @brief Implementing the Default minimizer specialization
 *
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/MathUtils/interface/RootMathTools/DefaultEngines.hpp"
#else
#include "UserUtils/MathUtils/RootMathTools/DefaultEngines.hpp"
#endif

#include "TError.h"

#include <cmath>

namespace usr
{

/**
 * @brief pseudo machine epsilon to initialize iterations for next to boundary
 *        situations.
 * @details this is the difference between 1 and the next largest double
 *          smaller than 1 (\f$\sim10^{-50}\f$). Enough for most test cases to
 *          converge properly.
 */
extern const double eff_machine_epsilon = 1-std::nexttoward( 1, 0 );

// ------------------------------------------------------------------------------


DefaultMinimizer::DefaultMinimizer() :
  TMinuitMinimizer( ROOT::Minuit::kMigrad )
{
  // Default settings which should work for 99% of use cases
  SetMaxFunctionCalls( 1000000 );// for Minuit/Minuit2
  SetMaxIterations( 10000 );// for GSL
  SetTolerance( 0.00001 );
  SetPrintLevel( -1000 );// Suppressing all outputs
}


usr::Measurement
DefaultMinimizer::GetParamAsMeasurement( const unsigned index )
{
  const double cen = X()[index];

  // Getting minos error
  double up, lo;
  GetMinosError( index, lo, up );
  return usr::Measurement( cen, up, -lo );
}


// ------------------------------------------------------------------------------

DefaultSolver1D::DefaultSolver1D() :
  ROOT::Math::RootFinder( ROOT::Math::RootFinder::kBRENT )
{}

unsigned DefaultSolver1D::default_max_iteration = 1e6;
double   DefaultSolver1D::default_abs_tolerance = 1e-8;
double   DefaultSolver1D::default_rel_tolerance = 1e-10;

double
DefaultSolver1D::SolveF( const ROOT::Math::IGenFunction& f,
                         double                          xlow,
                         double                          xup )
{
  SetFunction( f, xlow, xup );
  Solve( (int)default_max_iteration,
         default_abs_tolerance,
         default_rel_tolerance );
  return Root();// Returning the root regardless of consituent;
}


double
DefaultSolver1D::SolveForY( const ROOT::Math::IGenFunction& f,
                            double                          y,
                            double                          xlow,
                            double                          xup )
{
  // Lambda function is the easiest way to express an augmented f.
  auto shifted_f = [&f, y]( const double x ){
                     return f( x )-y;
                   };

  ROOT::Math::Functor1D new_func( shifted_f );

  SetFunction( new_func, xlow, xup );
  Solve( (int)default_max_iteration,
         default_abs_tolerance,
         default_rel_tolerance );

  return Root();
}


// ------------------------------------------------------------------------------

DefaultMinimizer1D::DefaultMinimizer1D() :
  ROOT::Math::BrentMinimizer1D()
{}

unsigned DefaultMinimizer1D::default_max_iteration = 1e6;
double   DefaultMinimizer1D::default_abs_tolerance = 1e-8;
double   DefaultMinimizer1D::default_rel_tolerance = 1e-10;

void
DefaultMinimizer1D::Minimize( const ROOT::Math::IGenFunction& f,
                              double                          xlow,
                              double                          xup )
{
  SetFunction( f, xlow, xup );
  ROOT::Math::BrentMinimizer1D::Minimize( (int)default_max_iteration,
                                          default_abs_tolerance,
                                          default_rel_tolerance );
}


// ------------------------------------------------------------------------------


DefaultSolverND::DefaultSolverND() :
  ROOT::Math::GSLMultiRootFinder( ROOT::Math::GSLMultiRootFinder::kHybridS  )
{
  SetPrintLevel( 0 );// Suppress all outputs by default
  gErrorIgnoreLevel = kBreak;
}

}/*usr*/
