/**
 * @file PlotCommon_MinMax.cc
 * @author Yi-Mu "Enoch" Chen
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/PlotUtils/interface/PlotCommon.hpp"
#else
#include "UserUtils/PlotUtils/PlotCommon.hpp"
#endif

#include "TProfile.h"

namespace usr
{

namespace plt
{

/**
 * @brief Getting the maximum y value of a histogram
 * @details Returning the maximum bin-value of a histogram object with the bin
 * error taken into consideration.
 */
double
GetYmax( const TH1D& hist )
{
  double ans = 0;

  for( int i = 1; i <= hist.GetNbinsX(); ++i ){
    const double bincont = hist.GetBinContent( i );

    // Special case for TProfile... (not sure why yet)
    const double binerr = hist.InheritsFrom( TProfile::Class() ) ?
                          0 :
                          hist.GetBinError( i );

    // Skipping over zero bins
    if( bincont == 0 ){continue; }
    ans = std::max( ans, bincont+binerr );
  }

  return ans;
}


/**
 * @brief Getting the minimum y value of a histogram
 * @details Returning the minimum bin value of a histogram object, with the bin
 * error taken into account.
 */
double
GetYmin( const TH1D& hist )
{
  double ans = 0.3;

  for( int i = 1; i <= hist.GetNbinsX(); ++i ){
    const double bincont = hist.GetBinContent( i );
    const double binerr  = hist.GetBinError( i );

    // Skipping over Zero bins
    if( bincont == 0 ){ continue; }

    // Usually the case for single entry bins
    if( bincont-binerr == 0  ){
      ans = std::min( ans, 0.3 * bincont );
    } else {
      ans = std::min( ans, bincont-0.99 * binerr );
    }
  }

  return ans;
}


/**
 * @brief Get the maximum y value stored in the TGraph
 * @details Returning maximum y value stored in a TGraphs object, with the Y
 * error bar taken into account (The GetErrorYhigh method is virtual, so TGraphs
 * with asymmetric errors would be handled properly.)
 */
double
GetYmax( const TGraph& x )
{
  double ans = -std::numeric_limits<double>::max();

  for( int i = 0; i < x.GetN(); ++i ){
    const double bin = x.GetY()[i]+std::max( x.GetErrorYhigh( i ), 0.0 );
    ans = std::max( ans, bin );
  }

  return ans;
}


/**
 * @brief Get the maximum y value stored in the TGraph
 * @details Returning the minimum y value stored in a TGraph object, with the y
 * error bar taken into account. (Again exploiting the fact that GetErrorYlow is
 * virtual.)
 */
double
GetYmin( const TGraph& x )
{
  double ans = std::numeric_limits<double>::max();

  for( int i = 0; i < x.GetN(); ++i ){
    const double bin = x.GetY()[i]-std::max( x.GetErrorYlow( i ), 0.0 );

    // In the case that the value is exactly zero, we are going to ignore this
    // value!
    if( bin == 0 ){ continue; }
    ans = std::min( ans, bin );
  }

  return ans;
}


/**
 * @brief Get the minimmum x value of a graph.
 * @details Returning the minimum value of x stored in the TGraph object with x
 * errors taken into account.
 */
double
GetXmin( const TGraph& x )
{
  double ans = std::numeric_limits<double>::max();

  for( int i = 0; i < x.GetN(); ++i ){
    const double bin = x.GetX()[i]-std::max( x.GetErrorXlow( i ), 0.0 );
    if( bin == 0 ){ continue; }
    ans = std::min( ans, bin );
  }

  return ans;
}


/**
 * @brief Get the maximum x value of a graph.
 * @details Returning the maximum value of x stored in the TGraph object with x
 * errors taken into account.
 */
double
GetXmax( const TGraph& x )
{
  double ans = -std::numeric_limits<double>::max();

  for( int i = 0; i < x.GetN(); ++i ){
    const double bin = x.GetX()[i]+std::max( x.GetErrorXhigh( i ), 0.0 );
    ans = std::max( ans, bin );
  }

  return ans;
}


/**
 * @brief Get the minimum y value of the TEfficiency object
 *
 * This function also takes into account the bin uncertainties which is helpful
 * for determining plot range.
 */
double
GetYmin( const TEfficiency& x )
{
  double ans = 1;

  const unsigned nbins = x.GetPassedHistogram()->GetNcells();

  for( unsigned i = 1; i <= nbins; ++i ){
    // Skipping over empty bins
    if( x.GetPassedHistogram()->GetBinContent( i ) == 0 ){ continue; }
    const double bin = x.GetEfficiency( i )-x.GetEfficiencyErrorLow( i );
    ans = std::min( ans, bin );
  }

  return ans;
}


/**
 * @brief Get the maximum y value of the TEfficiency object.
 *
 * This function also takes into account the bin uncertainties to allow for help
 * with determining the plot range.
 */
double
GetYmax( const TEfficiency& x )
{
  double ans = 0;

  const unsigned nbins = x.GetPassedHistogram()->GetNcells();

  for( unsigned i = 1; i <= nbins; ++i ){
    // Skipping over empty bins
    if( x.GetPassedHistogram()->GetBinContent( i ) == 0 ){ continue; }
    const double bin = x.GetEfficiency( i )+x.GetEfficiencyErrorUp( i );
    ans = std::max( ans, bin );
  }

  return ans;
}


/**
 * @brief Getting the minimum y value of te THStack object.
 * @details Retuning the maximum bin value of a THStack object, the bin errors
 * of
 * each histogram would *not* be taken into account. This functions assumes
 * *identical* binning of the internal histograms, though this might not be the
 * case. Use at your own risk.
 */
double
GetYmax( const THStack& x )
{
  double ans = 0.3;
  if( x.GetNhists() == 0 ){ return ans; }

  TH1*first = (TH1*)x.GetHists()->At( 0 );

  for( int i = 1; i <= first->GetNbinsX(); ++i ){
    double sum = 0;

    for( int j = 0; j < x.GetNhists(); ++j ){
      sum += ( (TH1*)x.GetHists()->At( j ) )->GetBinContent( i );
    }

    ans = std::max( ans, sum );
  }

  return ans;
}


/**
 * @brief Getting the minimum y value of a THStack object.
 * @details The bin errors of each internal histogram would *not* be taken into
 * account. This function currently assume *identical* binning for the
 * histograms, though this is not necessary the case. Use at your own risk.
 */
double
GetYmin( const THStack& x )
{
  double ans = 0.3;
  if( x.GetNhists() == 0 ){ return ans; }

  TH1*first = (TH1*)x.GetHists()->At( 0 );

  for( int i = 1; i <= first->GetNbinsX(); ++i ){
    double sum = 0;

    for( int j = 0; j < x.GetNhists(); ++j ){
      sum += ( (TH1*)x.GetHists()->At( j ) )->GetBinContent( i );
    }

    ans = std::min( ans, sum );
  }

  return ans;
}

}

}
