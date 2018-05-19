/*******************************************************************************
*
*  Filename    : RooFitExt.hpp
*  Description : Extension to some mathematical computations for RooFit objects
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef USERUTILS_MATHUTILS_ROOFITEXT_HPP
#define USERUTILS_MATHUTILS_ROOFITEXT_HPP

#include <RooAbsPdf.h>
#include <RooDataSet.h>

namespace usr {

/*-----------------------------------------------------------------------------
 *  Kolmogorov--Smirov Test functions for RooFit objects.
 *  This includes the the modified effective number of entires for weighted
 *  data sets: N_eff = (sum w)^2 / ( sum (w^2) )
 *  At most supporting two-range settings
   --------------------------------------------------------------------------*/
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

}/* usr */

#endif/* end of include guard: USERUTILS_MATHUTILS_ROOFITEXT_HPP */
