/**
 * @file RootUtils_Misc.cc
 * @author Yi-Mu "Enoch" Chen
 * @brief Implementation of the ROOT object helper functions
 *
 */

#include <string>
#include <vector>

#include "TChain.h"
#include "TH1D.h"

namespace usr {

/**
 * @brief A nicer interface for initializing a TChain using std containers.
 */
TChain*
MakeTChain( const std::string&              title,
            const std::vector<std::string>& inputfiles )
{
  TChain* tree = new TChain( title.c_str() );

  for( const auto file : inputfiles ){
    if( !file.empty() ){
      tree->Add( file.c_str() );
    }
  }

  return tree;
}

/**
 * @brief A nicer interface of making a TH1D using `std::vector`s
 */
TH1D
MakeHist( const std::string&         name,
          const std::string&         title,
          const std::vector<double>& bins )
{
  return TH1D( name.c_str(), title.c_str(), bins.size() -1, bins.data() );
}

/**
 * @brief Reverse engineering the effective number of entries of a bin in a
 * histogram.
 *
 * The official recipe for computing the effective number of events for a list of
 * (non-negative) weighted events is typically:
 *
 * \[
 *  N_{eff} = \frac{\left(\sum_i w_i\right)^2}{\sum_i w_i^2 }
 * \]
 *
 * In fact is this the method of obtaining the effective number of events for an
 * entire histogram, as implemented in the TH1::GetEffectiveEntries() method.
 * This method however is not readily available for Effective Entry in a single
 * bin, but a per bin implementation does exists for the official calculation of
 * the bin uncertainties using the TH1::SumW2 method, defaulted for weighted
 * histograms:
 *
 * \[
 *    UNC = \sqrt{N_{eff}} \frac{\sum_i w_i}{N_i}
 * \]
 *
 * Effectively, ROOT treats the bin as having effectively \[N_{eff}\] events, and
 * scale the uncertainty according to the sum of weight to have an identical
 * relative uncertainty.
 *
 * So our function will effectively return
 * \[
 * N_{eff} = \left(\frac{GetBinContent(i)}{GetBinError(i)}\right)^2
 * \]
 * With additional parses to avoid NAN errors:
 */
double
GetBinEffectiveEntry( const TH1& hist,
                      const int  bin )
{
  if( hist.GetBinError( bin ) == 0 ){
    return 0;
  } else {
    return ( hist.GetBinContent( bin ) * hist.GetBinContent( bin ) ) /
           ( hist.GetBinError( bin ) * hist.GetBinError( bin ) );
  }

  return 0.0;
}


}
