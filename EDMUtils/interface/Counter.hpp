/*******************************************************************************
*
*  Filename    : Counter.hpp
*  Description : Class exposed to EDM framework to allow for run-level caching
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*  Inspired by edm::MergeableCounter, rewritten for <double>
*******************************************************************************/
#ifndef USERUTILS_EDMUTILS_COUNTER_HPP
#define USERUTILS_EDMUTILS_COUNTER_HPP

namespace usr {

class Counter
{
public:
  // constructor
  Counter();
  Counter( const double );
  virtual
  ~Counter();

  // Most have this function to allow run level storage
  bool mergeProduct( const Counter& nextCounter );

  // Saving value
  double value;
};

}/* usr  */

#endif/* end of include guard: USERUTILS_EDMUTILS_COUNTER_HPP */
