/*******************************************************************************
*
*  Filename    : RooFitExt.hpp
*  Description : Implementation for RooFitExt.hpp defined functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "UserUtils/MathUtils/interface/RooFitExt.hpp"
#include <RooAbsData.h>
#include <RooAbsPdf.h>
#include <RooDataSet.h>
#include <RooRealVar.h>
#include <TMath.h>

#include <boost/format.hpp>
#include <memory>

namespace usr {

/*-----------------------------------------------------------------------------
 *  Helper structure and function for extracting the dataset as a list of
 *  numbers with potential weights
   --------------------------------------------------------------------------*/
struct SimplifiedData
{
  typedef std::pair<double, double> Evt;
  double           sum;
  double           sumsq;
  std::vector<Evt> dataset;

  inline static bool
  Compare( const Evt& x, const Evt& y )
  { return x.first < y.first; }

  // Complicated member function implemented at end of file.
  double EffectiveNum() const;

  SimplifiedData(
    RooDataSet&      data,
    RooRealVar&      x,
    const RooCmdArg& cut1,
    const RooCmdArg& cut2
    );
};

/*----------------------------------------------------------------------------*/

struct SimplifiedCDF
{
  std::unique_ptr<RooAbsReal>             rawcdf;
  RooRealVar&                             var;
  std::vector<std::pair<double, double> > rangelist;
  double                                  norm;

  // Returning the effective CDF value
  double operator()( const double x );
  double rawval( const double x );

  inline double
  min( unsigned x ){ return rangelist.at( x ).first; }
  inline double
  max( unsigned x ){ return rangelist.at( x ).second; }

  SimplifiedCDF(
    RooAbsPdf&       pdf,
    RooRealVar&      x,
    const RooCmdArg& cut1,
    const RooCmdArg& cut2
    );
  void AddRange( const RooCmdArg& cmd );
};

bool CheckCutCmd( const RooCmdArg& cmd, RooRealVar& x );


/*-----------------------------------------------------------------------------
 *  Kolmongorov-Smirov Test implementation functions for Data---PDF comparison
   --------------------------------------------------------------------------*/
double
KSDistance(
  RooDataSet&      dataset,
  RooAbsPdf&       pdf,
  RooRealVar&      var,
  const RooCmdArg& cut1,
  const RooCmdArg& cut2
  )
{
  SimplifiedData sim( dataset, var, cut1, cut2 );
  SimplifiedCDF cdf( pdf, var, cut1, cut2 );

  double empcdf  = 0;
  double maxdist = 0;

  for( const auto& evt : sim.dataset ){
    // emperical cdf is the sum of weights
    // (normalization handled latter for stability )
    const double val = evt.first;
    const double wgt = evt.second;
    empcdf += wgt;
    maxdist = std::max( maxdist, fabs( cdf( val ) - ( empcdf/sim.sum ) ) );
  }

  return sqrt( sim.EffectiveNum() ) * maxdist;
}

/*----------------------------------------------------------------------------*/

double
KSProb(
  RooDataSet&      dataset,
  RooAbsPdf&       pdf,
  RooRealVar&      var,
  const RooCmdArg& cut1,
  const RooCmdArg& cut2
  )
{
  const double dist = KSDistance( dataset, pdf, var, cut1, cut2 );
  return TMath::KolmogorovProb( dist );
}

/*-----------------------------------------------------------------------------
 *  KS-Test data--data comparison
   --------------------------------------------------------------------------*/
double
KSDistance(
  RooDataSet&      set1,
  RooDataSet&      set2,
  RooRealVar&      var,
  const RooCmdArg& cut1,
  const RooCmdArg& cut2
  )
{
  SimplifiedData sim1( set1, var, cut1, cut2 );
  SimplifiedData sim2( set2, var, cut1, cut2 );

  double empcdf1 = 0;
  double empcdf2 = 0;
  double maxdist = 0;

  // Max distance mimiced from the ROOT implementation of TMath::KolmogorovTest
  // https://root.cern.ch/doc/v608/namespaceTMath.html#a2dcadfdb80ed8b7413ca84b71cb56434
  for( auto evt1 = sim1.dataset.begin(), evt2 = sim2.dataset.begin();
       evt1 != sim1.dataset.end() && evt2 != sim2.dataset.end(); ){
    if( evt1->first < evt2->first ){
      empcdf1 += evt1->second;
      evt1++;
    } else if( evt1->first > evt2->first ){
      empcdf2 += evt2->second;
      evt2++;
    } else {// special case for indentical value
      double x = evt1->first;

      while( evt1->first == x && evt1 != sim1.dataset.end() ){
        empcdf1 += evt1->second;
        evt1++;
      }

      while( evt2->first == x && evt2 != sim2.dataset.end() ){
        empcdf2 += evt2->second;
        evt2++;
      }
    }
    maxdist = TMath::Max( maxdist,
      fabs( ( empcdf1/sim1.sum ) - ( empcdf2/sim2.sum ) ) );
  }

  const double num1 = std::max( sim1.EffectiveNum(), sim2.EffectiveNum() );
  const double num2 = std::min( sim1.EffectiveNum(), sim2.EffectiveNum() );
  return maxdist * sqrt( ( num1/( num1 + num2 ) )*num2 );

}

/*----------------------------------------------------------------------------*/
double
KSProb(
  RooDataSet&      set1,
  RooDataSet&      set2,
  RooRealVar&      var,
  const RooCmdArg& cut1,
  const RooCmdArg& cut2 )
{
  const double dist = KSDistance( set1, set2, var, cut1, cut2 );
  return  TMath::KolmogorovProb( dist );
}

/*----------------------------------------------------------------------------*/

double KSProbAlt(
  RooDataSet&      set1,
  RooDataSet&      set2,
  RooRealVar&      var,
  const RooCmdArg& cut1,
  const RooCmdArg& cut2 )
{
  const double dist = KSDistance( set1, set2, var, cut1, cut2 );

  // Alternative calculation of p-value as described in wiki page
  // https://en.wikipedia.org/wiki/Kolmogorov%E2%80%93Smirnov_test#Two-sample_Kolmogorov%E2%80%93Smirnov_test
  // Hard clipping the output to 1 for a more sane output value
  return std::min( exp( -2. * dist * dist ) * 2., 1. );
}

/*-----------------------------------------------------------------------------
 *  SimplifiedData function implementations
   --------------------------------------------------------------------------*/
SimplifiedData::SimplifiedData(
  RooDataSet&      data,
  RooRealVar&      x,
  const RooCmdArg& cut1,
  const RooCmdArg& cut2 ) :
  sum( 0 ),
  sumsq( 0 )
{
  RooDataSet* op_set  = 0 ;
  RooDataSet* tmp_set = 0 ;
  if( !CheckCutCmd( cut1, x ) && !CheckCutCmd( cut2, x ) ){
    op_set = &data;
  } else if( CheckCutCmd( cut1, x ) && !CheckCutCmd( cut2, x ) ){
    op_set = (RooDataSet*)(data.reduce( RooFit::SelectVars( x ), cut1 ));
  } else if( CheckCutCmd( cut2, x ) && !CheckCutCmd( cut1, x ) ){
    op_set = (RooDataSet*)(data.reduce( RooFit::SelectVars( x ), cut2 ));
  } else {
    op_set  = (RooDataSet*)( data.reduce( RooFit::SelectVars( x ), cut1 ) );
    tmp_set = (RooDataSet*)( data.reduce( RooFit::SelectVars( x ), cut2 ) );
    op_set->append( *tmp_set );
  }

  if( !op_set->isWeighted() ){
    sum = sumsq = op_set->numEntries();
  }

  for( int i = 0; i < op_set->numEntries(); ++i ){
    const double d = op_set->get( i )->getRealValue( x.GetName() );
    const double w = op_set->weight();
    if( !op_set->isWeighted() ){
      dataset.push_back( std::pair<double, double>( d, 1 ) );
    } else {
      dataset.push_back( std::pair<double, double>( d, w ) );
      sum   += w;
      sumsq += w*w;
    }
  }

  if( op_set != &data ){ delete op_set; }
  if( tmp_set ){ delete tmp_set; }

  std::sort( dataset.begin(), dataset.end(), Compare );
}

/*----------------------------------------------------------------------------*/

double
SimplifiedData::EffectiveNum() const
{
  if( sum == sumsq ){// unweighted data set
    return sum;
  } else {
    // Effective number of events for weighted data set found here:
    // https://stats.stackexchange.com/questions/193439/two-sample-kolmogorov-smirnov-test-with-weights?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa
    return sum * sum / sumsq;
  }
}

/*-----------------------------------------------------------------------------
 *  SimplifiedCDF implementations
   --------------------------------------------------------------------------*/
SimplifiedCDF::SimplifiedCDF(
  RooAbsPdf&       pdf,
  RooRealVar&      x,
  const RooCmdArg& cut1,
  const RooCmdArg& cut2 ) :
  rawcdf( pdf.createCdf( RooArgSet( x ) ) ),
  var( x )
{
  if( CheckCutCmd( cut1, var ) ){ AddRange( cut1 ); }
  if( CheckCutCmd( cut2, var ) ){ AddRange( cut2 ); }

  if( rangelist.size() == 2 ){
    // Making sure the lower edge is the first range entry.
    if( min( 0 ) > min( 1 ) ){
      auto temp = rangelist.at( 0 );
      rangelist.at( 0 ) = rangelist.at( 1 );
      rangelist.at( 1 ) = temp;
    }

    // reducing the second range if overlaps
    if( max( 0 ) >= min( 1 ) ){
      rangelist.at( 0 ).second = rangelist.at( 1 ).second;
      rangelist.erase( rangelist.begin() + 1 );
    }
  }

  // Saving normalization denominator
  if( rangelist.size() == 0 ){
    norm = rawval( var.getMax() )  - rawval( var.getMin() );// Should be 1
  } else if( rangelist.size() == 1 ){
    norm = rawval( max( 0 ) ) - rawval( min( 0 ) );
  } else {
    norm = ( rawval( max( 0 ) ) - rawval( min( 0 ) ) )
           + ( rawval( max( 1 ) ) - rawval( min( 1 ) ) );
  }
}
/*----------------------------------------------------------------------------*/
double
SimplifiedCDF::operator()( const double x )
{
  double ans = rawval( x );
  if( rangelist.size() == 0 ){
    return ans;
  } else if( rangelist.size() == 1 ){
    if( min( 0 ) < x && x < max( 0 ) ){
      return ( ans - rawval( min( 0 ) ) )/norm;
    } else {
      return ans;
    }
  } else {
    if( min( 0 ) < x && x < max( 0 ) ){
      return ( ans - rawval( min( 0 ) ) )/norm;
    } else if( min( 1 ) < x && x < max( 1 ) ){
      return ( ans - rawval( min( 1 ) ) + ( rawval( max( 0 ) )-rawval( min( 0 ) ) ) )/norm;
    } else {
      return ans;
    }
  }
}

/*----------------------------------------------------------------------------*/

double
SimplifiedCDF::rawval( const double x )
{
  var = x;
  return rawcdf->getVal();
}

/*----------------------------------------------------------------------------*/

void
SimplifiedCDF::AddRange( const RooCmdArg& cmd )
{
  rangelist.push_back( std::pair<double, double>(
      var.getMin( cmd.getString( 0 ) ),
      var.getMax( cmd.getString( 0 ) ) )
    );
}

/*----------------------------------------------------------------------------*/

bool
CheckCutCmd( const RooCmdArg& cmd, RooRealVar& var )
{
  if( std::string( cmd.GetName() ) != "CutRange" ){
    return false;
  } else if( !var.hasRange( cmd.getString( 0 ) ) ){
    return false;
  }
  return true;
}


}/* usr */
