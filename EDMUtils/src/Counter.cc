/*******************************************************************************
*
*  Filename    : Counter.hpp
*  Description : Implementation of counter class
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "UserUtils/EDMUtils/interface/Counter.hpp"

namespace usr {

Counter::Counter(){ value = 0; }
Counter::Counter( const double x ){ value = x; }
Counter::~Counter(){}

bool
Counter::mergeProduct( const Counter& nextcounter )
{
  // No case handling for overflow, assuming that double
  // should be able to handle precision
  value += nextcounter.value;
  return true;
}

}/* usr */
