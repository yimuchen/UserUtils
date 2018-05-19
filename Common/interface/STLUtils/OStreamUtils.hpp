/*******************************************************************************
*
*  Filename    : OStreamUtils.hpp
*  Description : Handful of helpful all public classes to enhance ostream usage.
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef USERUTILS_COMMON_STLUTILS_OSTREAMUTILS_HPP
#define USERUTILS_COMMON_STLUTILS_OSTREAMUTILS_HPP

#include <iostream>

namespace usr {

/*-----------------------------------------------------------------------------
 *  Clearing the output of one line (returning carrage and printing whitespace)
   --------------------------------------------------------------------------*/
class clearline
{
public:
  clearline(){}
  virtual ~clearline(){}
  friend std ::ostream& operator<<( std::ostream&, const clearline& );
};

/*-----------------------------------------------------------------------------
 *  Printing the same character multiple times for output splitting
   --------------------------------------------------------------------------*/
class separator
{
private:
  char token;
  size_t n;

public:
  separator( const char x = '*', const size_t m = 50 );
  virtual
  ~separator();
  friend std ::ostream& operator<<( std::ostream&, const separator& );
};



}/* usr */

#endif/* end of include guard: USERUTILS_COMMON_STLUTILS_OSTREAMUTILS_HPP */
