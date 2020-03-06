#ifdef CMSSW_GIT_HASH
#include "UserUtils/PlotUtils/interface/PlotCommon.hpp"
#else
#include "UserUtils/PlotUtils/PlotCommon.hpp"
#endif

#include "TH1D.h"

#include <iostream>

int main()
{
  TH1D h( "hist", "", 100, -5, 5 );
  h.FillRandom( "gaus", 500 );
  // h.Scale( 1.0/ 500.0 );

  std::cout << usr::plt::GetYmax( &h ) << std::endl;
  std::cout << usr::plt::GetYmin( &h ) << std::endl;
}
