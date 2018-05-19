/*******************************************************************************
*
*  Filename    : STLUtils_OStreamUtils.cc
*  Description : Ostream interaction class Implementation
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "UserUtils/Common/interface/STLUtils/OStreamUtils.hpp"

#include <ostream>

namespace usr {

/*******************************************************************************
*   Clear line implementation
*******************************************************************************/
std::ostream&
operator<<( std::ostream& os, const clearline& )
{
  for( size_t i = 0; i < 255; ++i ){
    os << "\b \b" << std::flush;
  }

  return os;
}


/*******************************************************************************
*   separator implementation
*******************************************************************************/
separator::separator( const char x, const size_t m ) :
  token( x ),
  n(m){}

separator::~separator(){}

std::ostream&
operator<<( std::ostream& os, const separator& x )
{
  for( size_t i = 0; i < x.n; ++i ){
    os << x.token << std::flush;
  }

  return os;
}

}/* usr */
