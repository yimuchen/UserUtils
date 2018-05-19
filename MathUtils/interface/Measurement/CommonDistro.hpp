/*******************************************************************************
*
*  Filename    : Efficiency.hpp
*  Description : Wraper function for our parameter to TEfficiency class
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*  Wrapper functions for storing selection efficiencies to the Measurement
*  objects. Mainly powered by the TEfficiency static functions
*
*******************************************************************************/
#ifndef USERUTILS_MATHUTILS_MEASUREMENT_COMMONDISTRO_HPP
#define USERUTILS_MATHUTILS_MEASUREMENT_COMMONDISTRO_HPP

#include "UserUtils/MathUtils/interface/Measurement/Measurement.hpp"
#include "UserUtils/MathUtils/interface/StatisticsUtil.hpp"

namespace usr {

/*-----------------------------------------------------------------------------
 *  Efficiency related structures and helper functions
   --------------------------------------------------------------------------*/
namespace Efficiency
{

extern Measurement Minos(
  const double passed,
  const double total,
  const double confidencelevel = usr::stat::onesigma_level
  );

/*-----------------------------------------------------------------------------
 *  General Wrapper for TEfficiency::Baysian functions
   --------------------------------------------------------------------------*/
extern const bool shortest_interval;
extern const bool central_interval;

extern Measurement Bayesian(
  double passed,
  double total,
  double confidencelevel,
  bool   confidencemethod,
  double alpha,
  double beta
  );

};// Efficiency


/*******************************************************************************
*   Poisson - all Poison related method for creating Measurements
*******************************************************************************/
namespace Poisson
{
extern Measurement Minos(
  const double obs ,
  const double confidencelevel = usr::stat::onesigma_level
);

// TODO: Add more methods

};// Poisson

}/* usr */

#endif/* end of include guard: USERUTILS_MATHUTILS_EFFICIENCY_HPP */
