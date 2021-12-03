/**
 * @file    StatisticsUtil.hpp
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 * @brief   Calculating statistics routines using GSL objects.
 */

#ifndef USERUTILS_MATHUTILS_STATISTICUTIL_HPP
#define USERUTILS_MATHUTILS_STATISTICUTIL_HPP

#ifdef CMSSW_GIT_HASH
#include "UserUtils/MathUtils/interface/RootMathTools/DefaultEngines.hpp"
#else
#include "UserUtils/MathUtils/RootMathTools/DefaultEngines.hpp"
#endif

#include "Math/IFunction.h"

namespace usr
{

namespace stat
{

/**
 * @addtogroup StatUtils
 * @{
 */

/*-----------------------------------------------------------------------------
 *  Sigma interval -- confidence level conversion.
   --------------------------------------------------------------------------*/
extern double NormalCDF( const double x );
extern double GetConfidenceLevel( const double sigmainterval );
extern double GetSigmaInterval( const double confidencelevel );

extern const double onesigma_level;
extern const double twosigma_level;

/*-----------------------------------------------------------------------------
 *  General calculation routines for Minos uncertainties.
   --------------------------------------------------------------------------*/
extern double DeltaNLLFromSigma( const double sigma );
extern double DeltaNLLFromConfidence( const double confidence );

extern int MinosError( const ROOT::Math::IGenFunction& nllfunction,
                       double&                         guess,
                       double&                         min,
                       double&                         max,
                       const double                    confidencelevel = onesigma_level );

extern int MinosError( const ROOT::Math::IMultiGenFunction& nllfunction,
                       const ROOT::Math::IMultiGenFunction& varfunction,
                       const double*                        initguess,
                       double&                              central,
                       double&                              min,
                       double&                              max,
                       const double                         confidencelevel = onesigma_level,
                       const double*                        upperguess      = nullptr,
                       const double*                        lowerguess      = nullptr );

/*-----------------------------------------------------------------------------
 *  Providing common distributions NLL fucntions in standard ROOT::Math formats
   --------------------------------------------------------------------------*/

class GaussianNLL : public ROOT::Math::IGenFunction
{
  double mean, sigma;
  double DoEval( const double x ) const;
  DECLARE_IGENFUNCTION_DEFAULTS( GaussianNLL );

public:
  GaussianNLL( double mean_, double sigma_ ) :
    mean ( mean_ ),
    sigma( sigma_ ){}
};

class PoissonNLL : public ROOT::Math::IGenFunction
{
  double obs;
  double DoEval( const double x ) const;
  DECLARE_IGENFUNCTION_DEFAULTS( PoissonNLL );

public:
  PoissonNLL( double obs_ ) :
    obs( obs_ ){}
};

class BinomialNLL : public ROOT::Math::IGenFunction
{
  double passed, total;
  double DoEval( const double x ) const;
  DECLARE_IGENFUNCTION_DEFAULTS( BinomialNLL );

public:
  BinomialNLL( double passed_, double total_ ) :
    passed( passed_ ),
    total ( total_ )
  {}


};

/** @} */

}/* stat */

}/* usr */

#endif/* end of include guard: USERUTILS_MATHUTILS_STATISTICUTIL_HPP */
