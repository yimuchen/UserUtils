#ifdef CMSSW_GIT_HASH
#include "UserUtils/MathUtils/interface/RootMathTools/RootFit.hpp"
#else
#include "UserUtils/MathUtils/RootMathTools/RootFit.hpp"
#endif

namespace usr
{

namespace fit
{

/**
 * @details Here we unfold the RooCmdArgs into the standard strings of the
 * vanilla ROOT fit routine:
 *
 * - The save string "S" will always be present.
 * - Depending on the FitVerboseLevel argument, "Q", "V" or "" will be added
 *   (defaults to "Q")
 * - Depending of the RunMinosImprovement "E" and "M" may be added (default adds
 *   both)
 * - Depending on the GraphErrorType method, either "", "W" or "EXO" will be
 *   added (defaults to "")
 *
 * The Fit Range is also now specified by an argument
 *
 * All other options strings will be omitted.
 */
extern TFitResult
FitGraph( TGraph& g, TF1& f, const std::vector<RooCmdArg>& arglist )
{
  const RooArgContainer args( arglist, {
        FitVerboseLevel( verbose::QUITE ), //
        RunMinos( true ), //
        RunImprove( true ), //
        GraphErrorType( gerrortype::DEFAULT )
      } );
  const std::string save_str = "SN0";
  const int         verb_int = args.GetInt( "FitVerboseLevel" );
  const std::string verb_str = verb_int == verbose::QUITE ?
                               "Q" :
                               verb_int == verbose::VERBOSE ?
                               "V" :
                               "";
  const std::string minos_str = args.GetInt( "RunMinos" ) ?
                                "E" :
                                "";
  const std::string more_str = args.GetInt( "RunImprove" ) ?
                               "M" :
                               "";
  const int         err_int = args.GetInt( "GraphErrorType" );
  const std::string err_str = err_int == gerrortype::IGNORE_ALL ?
                              "W" :
                              err_int == gerrortype::IGNORE_X ?
                              "EXO" :
                              "";
  const double xmin = args.Has( "GraphXRange" ) ?
                      args.GetDouble( "GraphXRange", 0 ) :
                      0;
  const double xmax = args.Has( "GraphXRange" ) ?
                      args.GetDouble( "GraphXRange", 1 ) :
                      0;

  const std::string root_str = save_str+verb_str+minos_str+more_str+err_str;

  // Running the fit, de-reference the TFitResultPtr class.
  return *( g.Fit( &f, root_str.c_str(), "", xmin, xmax ).Get());
}


//-------------------------------------------------------------------------------

/**
 * @brief Options for specifing verbose level for ROOT  fits
 */
extern RooCmdArg
FitVerboseLevel( const verbose level )
{
  return RooCmdArg( "FitVerboseLevel", level );
}


USERUTILS_COMMON_REGISTERCMD( FitVerboseLevel );


//-------------------------------------------------------------------------------

/**
 * @brief Whether or not to run the minos estimator.
 */
extern RooCmdArg
RunMinos( const bool run )
{
  return RooCmdArg( "RunMinos", run );
}


USERUTILS_COMMON_REGISTERCMD( RunMinos );

//-------------------------------------------------------------------------------

/**
 * @brief Whether or not to run the phase space improver.
 */
extern RooCmdArg
RunImprove( const bool run )
{
  return RooCmdArg( "RunImprove", run );
}


USERUTILS_COMMON_REGISTERCMD( RunImprove );

//-------------------------------------------------------------------------------

/**
 * @brief Specifying the fitting error type.
 */
extern RooCmdArg
GraphErrorType( const gerrortype x )
{
  return RooCmdArg( "GraphErrorType", x );
}


USERUTILS_COMMON_REGISTERCMD( GraphErrorType );

//-------------------------------------------------------------------------------

/**
 * @brief Specifing the fit range.
 */
extern RooCmdArg
GraphXRange( const double x, const double y )
{
  return RooCmdArg( "GraphXRange", 0, 0, x, y );
}


USERUTILS_COMMON_REGISTERCMD( GraphXRange );

//-------------------------------------------------------------------------------

}

}
