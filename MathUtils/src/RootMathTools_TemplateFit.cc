/**
 * @file RootMathTools_TemplateFit.cc
 * @author Yi-Mu "Enoch" Chen
 * @brief Implementing the Template Fit functions
 */

#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/RootUtils/Misc.hpp"
#include "UserUtils/MathUtils/interface/Measurement/CommonDistro.hpp"
#include "UserUtils/MathUtils/interface/RootMathTools/DefaultEngines.hpp"
#include "UserUtils/MathUtils/interface/RootMathTools/TemplateFit.hpp"
#else
#include "UserUtils/Common/RootUtils/Misc.hpp"
#include "UserUtils/MathUtils/Measurement/CommonDistro.hpp"
#include "UserUtils/MathUtils/RootMathTools/DefaultEngines.hpp"
#include "UserUtils/MathUtils/RootMathTools/TemplateFit.hpp"
#endif

#include "Math/Functor.h"

#include <cassert>

namespace usr {

TemplateFit::TemplateFit( const TH1*               target_,
                          const std::vector<TH1*>& constituents_,
                          const bool               normalize_target_ ) :
  target( target_ ),
  constituents( constituents_ ),
  normalize_target( normalize_target_ )
{
  RebuildFunctor();
}


double
TemplateFit::operator()( const double* x ) const
{
  double chi2 = 0;

  for( int bin = 1; bin <= target->GetNcells(); ++bin ){
    // Getting the target histograms, with upper and lower uncertainties
    const double cen = normalize_target ?
                       target->GetBinContent( bin ) / target->Integral() :
                       target->GetBinContent( bin );

    const double eff_num = usr::GetBinEffectiveEntry( *target, bin );
    const double scale   = eff_num == 0 ? 1.0 : ( cen / eff_num );
    // Avoid scaling by NAN

    const usr::Measurement binerr = usr::Poisson::CMSStatCom( eff_num )
                                    * scale;
    assert( cen == cen );

    // Summing constituent histograms
    double diff   = 0;
    double valsum = 0;

    for( unsigned index = 0; index < constituents.size(); ++index ){
      if( !normalize_target || index < constituents.size() -1  ){
        diff += x[index] * constituents.at( index )->GetBinContent( bin )
                / constituents.at( index )->Integral();
        valsum += x[index];
      } else {
        diff += ( 1.0-valsum ) * constituents.at( index )->GetBinContent( bin )
                / constituents.at( index )->Integral();
      }
      assert( diff == diff );
    }

    // Calculating the different relative to the Poisson error
    if( diff > cen ){
      diff = ( diff - cen )/ binerr.AbsUpperError();
    } else if( diff == cen ){
      diff = 0.0;
    } else {
      diff = ( cen - diff ) / binerr.AbsLowerError();
    }

    // NAN error
    if( diff != diff ){
      continue;
    }

    chi2 += diff * diff;
  }

  // TODO : Add extended log-likelihood part for normalized fits
  return chi2;
}

/**
 * @brief To be called *after* minimizer.SetFunction().
 */
void
TemplateFit::InitMinimizer( ROOT::Math::Minimizer& minimizer ) const
{
  minimizer.SetFunction( fcn );

  // Initializing the parameters
  const double integral = target->Integral();
  const double init     = 1.0/constituents.size();

  // Setting initial value to a flat distribution among constiuents
  for( unsigned i = 0; i < nparams(); ++i ){
    const double val    = normalize_target ? init : integral * init;
    const double step   = 0.1* val;
    const double minval = 0.0;// Assuming positive stacking.
    const double maxval = normalize_target ? 1.0 : integral;
    minimizer.SetVariable( i, "", val, step );
    minimizer.SetVariableLimits( i, minval, maxval );
  }
}

unsigned
TemplateFit::nparams() const
{
  return normalize_target ? constituents.size() -1 : constituents.size();
}


void
TemplateFit::RebuildFunctor()
{
  fcn = ROOT::Math::Functor( *this, nparams() );
}

std::vector<Measurement>
TemplateFit::SimpleFit( const TH1*               target,
                        const std::vector<TH1*>& constituents,
                        const bool               norm )
{
  DefaultMinimizer minimizer;
  TemplateFit fit( target, constituents, norm );

  fit.InitMinimizer( minimizer );

  minimizer.Minimize();

  std::vector<Measurement> ans;

  for( unsigned i = 0; i < fit.nparams(); ++i ){
    ans.push_back( minimizer.GetParamAsMeasurement( i ) );
  }

  return ans;
}

}
