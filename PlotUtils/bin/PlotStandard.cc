#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/ArgumentExtender.hpp"
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"
#include "UserUtils/PlotUtils/interface/StandardPlotFormat.hpp"
#else
#include "UserUtils/Common/ArgumentExtender.hpp"
#include "UserUtils/Common/STLUtils/StringUtils.hpp"
#include "UserUtils/PlotUtils/StandardPlotFormat.hpp"
#endif


std::string ParsePlotCommand( const std::string&,
                              const usr::ArgumentExtender& args );
std::string ParseJsonFile( const std::string&,
                           const usr::ArgumentExtender& args );

void RunComparison( usr::plt::fmt::BatchRequest& batch,
                    const usr::ArgumentExtender& args );
void RunComparison2D( usr::plt::fmt::BatchRequest& batch,
                      const usr::ArgumentExtender& args );
void RunStandard( usr::plt::fmt::BatchRequest& batch,
                  const usr::ArgumentExtender& args  );
void RunTable( usr::plt::fmt::BatchRequest& batch,
               const usr::ArgumentExtender& args );

typedef void (* RunBatch)( usr::plt::fmt::BatchRequest&,
                           const usr::ArgumentExtender& );

std::map<std::string, RunBatch> MakeCommandMap();
const std::map<std::string, RunBatch> CommandMap = MakeCommandMap();


int
main( int argc, char* argv[] )
{
  usr::po::options_description desc_common(
    usr::fstr(
      "Running a standard plotting sequence interactively, Basic usage require "
      "first two arguments being the plotting command "
      "[compare/compare2d/standard/table] "
      "and the plot settings json file. So something like:\n"
      ">> %s  standard my_plots.json\n", argv[0] )
    );

  usr::po::options_description desc_io(
    "Input output options. This will override the io settings sections in the "
    "input json file."
    );

  desc_io.add_options()
    ( "fileprefix", usr::po::value<std::string>(),
    "File prefix to be used for histogram files" )
    ( "keyprefix", usr::po::value<std::string>(),
    "Common key prefix to be used for obtaining the histogram objects" )
    ( "outputprefix", usr::po::value<std::string>(),
    "Prefix for output files" )
    ( "outputpostfix", usr::po::value<std::string>(),
    "Post fix for output files (before the extension)" )
  ;

  usr::ArgumentExtender args;
  args.AddOptions( desc_common );
  args.AddOptions( desc_io );

  // Parsing the standard sequences that must exist in the command.
  if( argc  < 3 ){
    std::cout << "Error parsing command!" << std::endl;
    args.PrintHelpAndExit();
  }

  // Getting the command
  const std::string command    = ParsePlotCommand( argv[1], args );
  const std::string input_json = ParseJsonFile( argv[2], args );

  // Adding additional options for optsion for different sub-routines.

  // Parsing command latter options if exists
  // Keeping the last command since argc cannot be 0
  args.ParseOptions( argc-2, argv+2 );

  usr::plt::fmt::BatchRequest batch( input_json );

  // Running the command
  CommandMap.at( command )( batch, args );

  return 0;
}

std::string
ParsePlotCommand( const std::string&           str,
                  const usr::ArgumentExtender& args )
{
  const std::string ans = usr::ToLower( str );
  if( CommandMap.count( ans ) ){
    return str;
  } else {
    args.PrintHelpAndExit();
    return "";
  }
}

std::string
ParseJsonFile( const std::string&           str,
               const usr::ArgumentExtender& args )
{
  return str;
}

std::map<std::string, RunBatch>
MakeCommandMap()
{
  std::map<std::string, RunBatch> ans;

  ans["compare"]   = RunComparison;
  ans["compare2d"] = RunComparison2D;
  ans["standard"]  = RunStandard;
  ans["table"]     = RunTable;

  return ans;
}


void
RunComparison( usr::plt::fmt::BatchRequest& batch,
               const usr::ArgumentExtender& args )
{
  batch.GenerateSampleComparePlot();
}

void
RunComparison2D( usr::plt::fmt::BatchRequest& batch,
                 const usr::ArgumentExtender& args )
{
  batch.Generate2DComaprePlot();
}

void
RunStandard( usr::plt::fmt::BatchRequest& batch,
             const usr::ArgumentExtender& args  )
{
  batch.GeneratePlots();
}
void
RunTable( usr::plt::fmt::BatchRequest& batch,
          const usr::ArgumentExtender& args )
{
  batch.GenerateSimulationTable( std::cout );
}
