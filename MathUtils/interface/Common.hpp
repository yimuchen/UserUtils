/**
 * @file    Common.hpp
 * @brief   General purpose mathematical utility functions
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */

#ifndef USERUTILS_MATHUTILS_COMMON_HPP
#define USERUTILS_MATHUTILS_COMMON_HPP

namespace usr {


/*******************************************************************************
*   Intersect - Given four points A(p1x,p1y), B(p2x,p2y), C(p3x,p3y), D(p4x,p4y)
*               Check if segments AB and CD are intersecting. If yes, store
*               results in intx, and inty return true;
*               Return false otherewise;
*******************************************************************************/
extern bool Intersect(
  const double p1x, const double p1y,
  const double p2x, const double p2y,
  const double p3x, const double p3y,
  const double p4x, const double p4y,
  double& intx, double& inty
  );

}/* mgr */

#endif/* end of include guard: USERUTILS_MATHUTILS_COMMON_HPP */
