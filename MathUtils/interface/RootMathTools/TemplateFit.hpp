/**
 * @file TemplateFit.hpp
 * @author Yi-Mu "Enoch" Chen
 * @brief Class for performing histogram template fits.
 */
#ifndef USERUTILS_MATHUTILS_ROOTMATHTOOLS_TEMPLATEFIT_HPP
#define USERUTILS_MATHUTILS_ROOTMATHTOOLS_TEMPLATEFIT_HPP

#ifdef CMSSW_GIT_HASH
#include "UserUtils/MathUtils/interface/Measurement/Measurement.hpp"
#else
#include "UserUtils/MathUtils/Measurement/Measurement.hpp"
#endif

#include "Math/Functor.h"
#include "Math/Minimizer.h"
#include "TH1.h"

#include <vector>

namespace usr {

class TemplateFit : public ROOT::Math::IMultiGenFunction
{
public:
  static
  std::vector<Measurement> SimpleFit( const TH1*               target,
                                      const std::vector<TH1*>& constituents,
                                      const bool               norm = true );


  TemplateFit( const TH1*               target,
               const std::vector<TH1*>& constituents,
               const bool               normalize_target = true );
  ~TemplateFit(){}


  unsigned NDim() const;

  ROOT::Math::IMultiGenFunction* Clone() const ;

  // Helping with initializing the minimizer
  void InitMinimizer( ROOT::Math::Minimizer& ) const;

private:
  const TH1* target;
  const std::vector<TH1*> constituents;
  const bool normalize_target;

  // Function for interfacing with ROOT::Math::Minimizers
  double DoEval( const double* x ) const;

};

}

#endif
