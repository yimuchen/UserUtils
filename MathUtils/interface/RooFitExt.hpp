/**
 * @file   RooFitExt.hpp
*  @brief  Extension to some mathematical computations for RooFit objects
*  @author Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*/
#ifndef USERUTILS_MATHUTILS_ROOFITEXT_HPP
#define USERUTILS_MATHUTILS_ROOFITEXT_HPP

#include <RooAbsPdf.h>
#include <RooDataSet.h>

namespace usr {

/**
 * @defgroup KSRooFit KSRooFit
 * @brief    Kolmogorov--Smirnov test routines for RooFit objects
 * @ingroup MathUtils
 * @details
 * For some strange reason, there doesn't exist the [Kolmogorov--Smirnov test](https://en.wikipedia.org/wiki/Kolmogorov%E2%80%93Smirnov_test)
 * routines doesn't exit for RooFit objects. The functions provides includes
 * the test for both data--model comparison and data--data comparison, with
 * up to 2 variable range cut for regional and side-band goodness-of-fit tests.
 * Test includes both the maximum cumulative distribution function distances
 * and the resulting p-value when comparing the distance to the Kolmogorov
 * distribution
 * @{
 */
extern double KSDistance(
  RooDataSet&      dataset,
  RooAbsPdf&       pdf,
  RooRealVar&      var,
  const RooCmdArg& cut1 = RooCmdArg::none(),
  const RooCmdArg& cut2 = RooCmdArg::none()
  );

extern double KSProb(
  RooDataSet&      dataset,
  RooAbsPdf&       pdf,
  RooRealVar&      var,
  const RooCmdArg& cut1 = RooCmdArg::none(),
  const RooCmdArg& cut2 = RooCmdArg::none()
  );

extern double KSDistance(
  RooDataSet&      set1,
  RooDataSet&      set2,
  RooRealVar&      var,
  const RooCmdArg& cut1 = RooCmdArg::none(),
  const RooCmdArg& cut2 = RooCmdArg::none()
  );

extern double KSProb(
  RooDataSet&      set1,
  RooDataSet&      set2,
  RooRealVar&      var,
  const RooCmdArg& cut1 = RooCmdArg::none(),
  const RooCmdArg& cut2 = RooCmdArg::none()
  );

extern double KSProbAlt(
  RooDataSet&      set1,
  RooDataSet&      set2,
  RooRealVar&      var,
  const RooCmdArg& cut1 = RooCmdArg::none(),
  const RooCmdArg& cut2 = RooCmdArg::none()
  );
/* @} */

}/* usr */

#define USR_ROOFIT_SILENCE \
RooFit::Verbose(false), RooFit::PrintLevel(-1), \
Warnings(false), PrintEvalErrors(-1)

#endif/* end of include guard: USERUTILS_MATHUTILS_ROOFITEXT_HPP */
