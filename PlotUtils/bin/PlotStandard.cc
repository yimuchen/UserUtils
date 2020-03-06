#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/ArgumentExtender.hpp"
#include "UserUtils/PlotUtils/interface/StandardPlotFormat.hpp"
#else
#include "UserUtils/Common/ArgumentExtender.hpp"
#include "UserUtils/PlotUtils/StandardPlotFormat.hpp"
#endif

int main( int argc, char* argv[] )
{
  usr::po::options_description desc(
    "Generating standardized plots described in a given json file" );
  desc.add_options()
    ( "input,i", usr::po::value<std::string>(),
    "Input Json file" )
    ( "comparison,c", "Run the comparison plot routine" )
    ( "standard,s", "Run the standard data-bkg-signal plot routine" )
    ( "fulltable,t", usr::po::value<std::string>(),
    "Running the table generation routine, store to a file if specified" )
  ;

  usr::ArgumentExtender args;
  args.AddOptions( desc );
  args.ParseOptions( argc, argv );

  const std::string input_json = args.Arg( "input" );

  usr::plt::fmt::BatchRequest batch( input_json );

  if( args.CheckArg( "comparison" ) ){
    batch.GenerateSampleComparePlot();
  }
  if( args.CheckArg( "standard" ) ){
    batch.GeneratePlots();
  }
  if( args.CheckArg( "fulltable" ) ){
    batch.GenerateSimulationTable( std::cout );
  }


  return 0;
}
