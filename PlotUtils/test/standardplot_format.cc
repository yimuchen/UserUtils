#ifdef CMSSW_GIT_HASH
#include "UserUtils/PlotUtils/interface/StandardPlotFormat.hpp"
#include "UserUtils/Common/interface/STLUtils/OStreamUtils.hpp"
#else
#include "UserUtils/PlotUtils/StandardPlotFormat.hpp"
#include "UserUtils/Common/STLUtils/OStreamUtils.hpp"
#endif

#include <iostream>

int main( int argc, char* argv[] )
{
  usr::plt::fmt::BatchRequest b( argv[1] );

  b.GenerateSimulationTable( std::cout );
  std::cout << "\n\n\n" << usr::separator('-') << "\n\n\n" << std::endl;
  b.GenerateSimulationSummary( std::cout );
  std::cout << "\n\n\n" << usr::separator('-') << "\n\n\n" << std::endl;
  b.GenerateDataTable( std::cout );

  b.GeneratePlots();

  return 0 ;
}