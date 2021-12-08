#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/RootUtils/RooArgContainer.hpp"
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"
#include "UserUtils/MathUtils/interface/RooFitExt.hpp"
#else
#include "UserUtils/Common/RootUtils/RooArgContainer.hpp"
#include "UserUtils/Common/STLUtils/StringUtils.hpp"
#include "UserUtils/MathUtils/RooFitExt.hpp"
#endif

#include <RooAbsPdf.h>
#include <RooFitResult.h>

namespace usr
{

/**
 * @details
 * This uses the variadic interface to allow for arbitrarily many RooCmdArgs to
 * be added to the fit (instead of being hard capped at just 10 RooCmdArgs). By
 * default, it also adds all of the variables required to suppress the outputs.
 * Users can still enable the RooFit messages by explicitly adding back the
 * commands such as RooFit::Verbose( kTRUE ).
 */
extern RooFitResult*
FitPDFToData( RooAbsPdf&                    pdf,
              RooAbsData&                   data,
              const std::vector<RooCmdArg>& cmdargs )
{
  const usr::RooArgContainer args( cmdargs, {// Explicitly adding options to reduce verbosity
      RooFit::Verbose( false ), RooFit::PrintLevel( -100 ),
      RooFit::Warnings( false ), RooFit::PrintEvalErrors( -100 )  } );
  RooLinkedList roolist = args.MakeRooList();

  return pdf.fitTo( data, roolist );
}


USERUTILS_COMMON_REGISTERCMD( MaxFitIteration );

RooCmdArg MaxFitIteration( unsigned x )
{ return RooCmdArg( "MaxFitIteration", x ); }

/**
 * @details
 * Running the PDF fit routine multiple times until the resulting RooFitResults
 * returns a nominal exit code (status() == 0). You can use the New RooCmdArg
 * `usr::MaxFitIteration` fo specify the maximum number of fit interation to
 * perform (defaults to 10).
 */
extern RooFitResult*
ConvergeFitPDFToData( RooAbsPdf&                    pdf,
                      RooAbsData&                   data,
                      const std::vector<RooCmdArg>& cmdargs )
{
  RooFitResult*              status = nullptr;
  const usr::RooArgContainer args( cmdargs, {// Explicitly adding options to reduce verbosity
      RooFit::Save(), MaxFitIteration( 10 )// Default to at most 10
    } );
  const usr::RooArgContainer og_args( cmdargs );

  RooLinkedList roolist = args.MakeRooList();

  for( int i = 0; i < args.Get( "MaxFitIteration" ).getInt( 0 ); ++i ){
    status = FitPDFToData( pdf, data, args );
    if( status->status() != 0 ){
      if( !og_args.Has( "Save" ) ){
        return nullptr;
      } else {
        return status;
      }
    }
    delete status;
  }

  if( !og_args.Has( "Save" ) ){
    return nullptr;
  } else {
    return status;
  }
}


extern TH1D*
TH1DFromRooData( RooAbsData&                   data,
                 const RooAbsRealLValue &      xvar,
                 const std::vector<RooCmdArg>& cmdargs )
{
  const usr::RooArgContainer args( cmdargs );
  RooLinkedList              roolist = args.MakeRooList();

  TH1*rawhist = data.createHistogram( usr::RandomString( 6 ).c_str(),
                                      xvar,
                                      roolist );

  TH1D*hist = new TH1D( usr::RandomString( 6 ).c_str(),
                        "",
                        rawhist->GetNbinsX(),
                        rawhist->GetXaxis()->GetXmin(),
                        rawhist->GetXaxis()->GetXmax() );

  for( int i = 1; i <= hist->GetNcells(); ++i ){
    hist->SetBinContent( i, rawhist->GetBinContent( i ) );
  }

  delete rawhist;
  return hist;
}

}/* namespace usr */
