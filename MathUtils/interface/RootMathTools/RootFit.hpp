/**
 * @file RootFit.hpp
 * @brief Defining functions for a nicer ROOT fitting interface.
 */
#ifndef USERUTILS_MATHUTILS_ROOTMATHTOOLS_ROOTFIT_HPP
#define USERUTILS_MATHUTILS_ROOTMATHTOOLS_ROOTFIT_HPP

#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/RootUtils/RooArgContainer.hpp"
#include "UserUtils/Common/interface/STLUtils/VectorUtils.hpp"
#else
#include "UserUtils/Common/RootUtils/RooArgContainer.hpp"
#include "UserUtils/Common/STLUtils/VectorUtils.hpp"
#endif

#include "TF1.h"
#include "TFitResult.h"
#include "TGraph.h"
#include "TH1.h"

namespace usr
{

namespace fit
{

/**
 * @defgroup RootFit RootFit
 * @ingroup MathUtils
 * @brief A set of functions for providing a RooFit-esque, interface to fitting
 * simple ROOT object.
 *
 * @{
 */


/**
 * @{
 * @brief Fitting TGraphs to a functions
 */
extern TFitResult FitGraph( TGraph&, TF1&, const std::vector<RooCmdArg>& );
inline TFitResult FitGraph( TGraph& g, TF1& f )
{ return FitGraph( g, f, {} ); }
template<typename ... Args>
inline TFitResult FitGraph( TGraph&          g,
                            TF1&             f,
                            const RooCmdArg& arg1,
                            Args ... args )
{ return FitGraph( g, f, MakeVector<RooCmdArg>( arg1, args ... ) ); }

/** @} */


enum verbose
{
  QUITE = 0, INTERMEDIATE = 1, VERBOSE = 2
};

extern RooCmdArg FitVerboseLevel( const verbose level );
extern RooCmdArg RunMinos( const bool run   = true );
extern RooCmdArg RunImprove( const bool run = true );

enum gerrortype
{
  DEFAULT = 0, IGNORE_X = 1, IGNORE_ALL = 2,
};

extern RooCmdArg GraphErrorType( const gerrortype = DEFAULT );
extern RooCmdArg GraphXRange( const double, const double );


/** @} */

}

} /* usr */


#endif
