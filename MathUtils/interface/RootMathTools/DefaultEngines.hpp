/**
 * @file DefaultEngines.hpp
 * @author Yi-Mu "Enoch" Chen
 * @brief Declaring the Default Minimization Class
 */

#ifndef USERUTILS_MATHUTILS_ROOTMATHTOOLS_DEFAULTENGINES_HPP
#define USERUTILS_MATHUTILS_ROOTMATHTOOLS_DEFAULTENGINES_HPP

#ifdef CMSSW_GIT_HASH
#include "UserUtils/MathUtils/interface/Measurement/Measurement.hpp"
#else
#include "UserUtils/MathUtils/Measurement/Measurement.hpp"
#endif

#include "Math/BrentMinimizer1D.h"
#include "Math/GSLMultiRootFinder.h"
#include "Math/RootFinder.h"
#include "TMinuitMinimizer.h"

namespace usr {

/**
 * @brief
 *
 */

extern const double eff_machine_epsilon;

/**
 * @brief Default 1D minimizer engine with decent defaults and interfaces for
 * quick and dirty minimization.
 */
class DefaultMinimizer1D : public ROOT::Math::BrentMinimizer1D
{
public:
  DefaultMinimizer1D();
  ~DefaultMinimizer1D(){}

  // Quick and dirty minimization
  void Minimize( const ROOT::Math::IGenFunction& f, double low, double up );

  static unsigned default_max_iteration;
  static double default_abs_tolerance;
  static double default_rel_tolerance;
};

/**
 * @brief Default 1D function solver with decent defaults and interfaces for
 * quick and dirty function solving.
 *
 */
class DefaultSolver1D : public ROOT::Math::RootFinder
{
public:
  DefaultSolver1D();
  ~DefaultSolver1D(){}

  // Restricting interface for quick and dirty solving
  double SolveF( const ROOT::Math::IGenFunction & f,
                 double xlow, double xup );
  double SolveForY( const ROOT::Math::IGenFunction& f,
                    double y, double xlow, double xup );

  static unsigned default_max_iteration;
  static double default_abs_tolerance;
  static double default_rel_tolerance;
};

/**
 * @brief Default multidimensional function minimizer with decent defaults
 *
 * Since initial value and step sizes will always be an issue, there will be no
 * quick and dirty interface for minimization.
 */
class DefaultMinimizer : public TMinuitMinimizer
{
public:
  DefaultMinimizer();
  ~DefaultMinimizer(){}

  usr::Measurement GetParamAsMeasurement( const unsigned index );
};

/**
 * @brief Default multidimensional function solver with decent defaults
 *
 * Since initial value and step sizes will always be an issue, there will be no
 * quick and dirty interface for function solving.
 */
class DefaultSolverND : public ROOT::Math::GSLMultiRootFinder
{
public:
  DefaultSolverND();
  ~DefaultSolverND(){}

  inline bool
  Solve( const double* x,
         int           maxIter = 1e6,
         double        abs     = 1e-8,
         double        rel     = 1e-10 )
  {
    return ROOT::Math::GSLMultiRootFinder::Solve( x, maxIter, abs, rel );
  }
};


// Helper functions for fast declaration of ROOT::Math function objects
#define DECLARE_IGENFUNCTION_DEFAULTS( NAME )                           \
public:                                                                 \
  ~NAME(){}                                                             \
  inline ROOT::Math::IGenFunction* Clone() const { return new NAME( *this ); }

#define DECLARE_IMULTIGENFUNCTION_DEFAULTS( NAME )                      \
public:                                                                 \
  ~NAME(){}                                                             \
  inline ROOT::Math::IMultiGenFunction* Clone() const {                 \
   return new NAME(*this); }                                            \


}/* usr */


#endif
