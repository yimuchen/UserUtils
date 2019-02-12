/**
 * @file    Pad1D_helper.cc
 * @brief   Implementation of helper classes and function of Pad1D class
 *          including static member functions and helper classes.
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"
#include "UserUtils/PlotUtils/interface/Pad1D.hpp"
#else
#include "UserUtils/Common/STLUtils/StringUtils.hpp"
#include "UserUtils/PlotUtils/Pad1D.hpp"
#endif

#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"

#include <algorithm>

namespace usr {

namespace plt {

/**
 * Returning the maximum bin-value of a histogram object with the bin error
 * taken into consideration.
 */
double
Pad1D::GetYmax( const TH1D* hist )
{
  double ans = 0;

  for( int i = 1; i <= hist->GetNcells(); ++i ){
    const double bincont = hist->GetBinContent( i );
    const double binerr  = hist->GetBinError( i );
    ans = std::max( ans, bincont + binerr );
  }

  return ans;
}

/**
 * Returning the minimum bin value of a histogram object, with the bin error
 * taken into account.
 */
double
Pad1D::GetYmin( const TH1D* hist )
{
  double ans = 0.3;

  for( int i = 0; i <= hist->GetNcells(); ++i ){
    const double bincont = hist->GetBinContent( i );
    const double binerr  = hist->GetBinError( i );
    ans = std::min( ans, bincont - binerr );
  }

  return ans;
}

/**
 * Returning maximum y value stored in a TGraphs object, with the Y error bar
 * taken into account (The GetErrorYhigh method is virtual, so TGraphs with
 * assymetric errors would be handled properly.)
 */
double
Pad1D::GetYmax( const TGraph* x )
{
  double ans = -std::numeric_limits<double>::max();

  for( int i = 0; i < x->GetN(); ++i ){
    const double bin = x->GetY()[i] + x->GetErrorYhigh( i );
    ans = std::max( ans, bin );
  }

  return ans;
}

/**
 * Returning the minimum y value stored in a TGraph object, with the y error
 * bar taken into account. (Again exploiting the fact that GetErrorYlow is
 * virtual.)
 */
double
Pad1D::GetYmin( const TGraph* x )
{
  double ans = std::numeric_limits<double>::max();

  for( int i = 0; i < x->GetN(); ++i ){
    const double bin = x->GetY()[i] - x->GetErrorYlow( i );
    ans = std::min( ans, bin );
  }

  return ans;
}

/**
 * @brief
 *
 * @param x
 * @return double
 */
double
Pad1D::GetXmin( const TGraph* x )
{
  double ans = std::numeric_limits<double>::max() ;

  for( int i = 0 ; i < x->GetN(); ++i ){
    const double bin = x->GetX()[i] + x->GetErrorXhigh( i );
    ans = std::min(ans,bin );
  }

  return ans;
}

double
Pad1D::GetXmax( const TGraph* x )
{
  double ans = -std::numeric_limits<double>::max() ;

  for( int i = 0 ; i < x->GetN(); ++i ){
    const double bin = x->GetX()[i] - x->GetErrorXlow( i );
    ans = std::max(ans,bin );
  }

  return ans;
}

/**
 * Retuning the maximum bin value of a THStack object, the bin errors of each
 * histogram would *not* be taken into account. This functions assumes
 * *identical* binning of the internal histograms, though this might not be the
 * case. Use at your own risk.
 */
double
Pad1D::GetYmax( const THStack* x )
{
  double ans = 0.3;
  if( x->GetNhists() == 0 ){ return ans; }

  TH1* first = (TH1*)x->GetHists()->At( 0 );

  for( int i = 1; i < first->GetNcells(); ++i ){
    double sum = 0;

    for( int j = 0; j < x->GetNhists(); ++j ){
      sum += ( (TH1*)x->GetHists()->At( j ) )->GetBinContent( i );
    }

    ans = std::max( ans, sum );
  }

  return ans;
}

/**
 * Getting the minimum bin value of a THStack object, the bin erros of each
 * internal histogram would *not* be taken into accound. This function currently
 * assume *identical* binning for the histograms, though this is not nessesary
 * the case. Use at your own risk.
 */
double
Pad1D::GetYmin( const THStack* x )
{
  double ans = 0.3;
  if( x->GetNhists() == 0 ){ return ans; }

  TH1* first = (TH1*)x->GetHists()->At( 0 );

  for( int i = 1; i < first->GetNcells(); ++i ){
    double sum = 0;

    for( int j = 0; j < x->GetNhists(); ++j ){
      sum += ( (TH1*)x->GetHists()->At( j ) )->GetBinContent( i );
    }

    ans = std::min( ans, sum );
  }

  return ans;
}

/**
 * @brief string for defining the RooCmdArg name
 */
const std::string PlotType::CmdName = "PlotType";

/**
 * @brief defining plot type via enum
 */
PlotType::PlotType( const int i ) :
  RooCmdArg( CmdName.c_str(), i ){}

/**
 * @brief defining plot type via ROOT draw strings
 */
PlotType::PlotType( const std::string& drawopt ) :
  RooCmdArg( CmdName.c_str(), 0, 0, 0, 0, drawopt.c_str() ){}

/**
 * @brief string for defining the RooCmdArg name
 */
const std::string TrackY::CmdName = "TrackY";

/**
 * @brief define track y via enum
 */
TrackY::TrackY( const int i ) :
  RooCmdArg( CmdName.c_str(), i ){}

/**
 * @brief strong for defining the RooCmdArg name
 */
const std::string EntryText::CmdName = "EntryText";

/**
 * @brief define entry text string.
 */
EntryText::EntryText( const std::string& str ) :
  RooCmdArg( CmdName.c_str(), 0, 0, 0, 0, str.c_str() ){}


/**
 * @class RangeByVar
 * Constructor containter for constructing a Pad1D instance by using RooRealVar
 * as the x axis. By passing this class into the Pad1D constructor, the Pad1D
 * would know to generate a corresponding RooPlot instance for RooFit plot
 * object generation.
 */
const double RangeByVar::find_default
  = std::numeric_limits<double>::lowest();

/**
 * @brief Define a range using the @ROOT{RooRealVar} reference.
 *
 * Could defined x range and number of bins to use for the x axis.
 */
RangeByVar::RangeByVar(
  const RooRealVar& _var,
  const double      _xmin,
  const double      _xmax,
  const int         _nbin
  ) : var( &_var )
{
  xmin = _xmin == find_default ? var->getMin() :
         var->getMin() > _xmin ? var->getMin() :
         _xmin > _xmax         ? var->getMin() :
         _xmin;
  xmax = _xmax == find_default ? var->getMax() :
         _xmax > var->getMax() ? var->getMax() :
         _xmin < _xmax         ? var->getMax() :
         _xmax;
  nbin = _nbin < 0 ? var->getBins() : _nbin;
}

/**
 * @brief Define a range using the @ROOT{RooRealVar} pointer
 *
 * Could defined x range and number of bins to use for the x axis.
 */
RangeByVar::RangeByVar(
  const RooRealVar* _var,
  const double      _xmin,
  const double      _xmax,
  const int         _nbin
  ) : var( _var )
{
  xmin = _xmin == find_default ? var->getMin() :
         var->getMin() > _xmin ? var->getMin() :
         _xmin > _xmax         ? var->getMin() :
         _xmin;
  xmax = _xmax == find_default ? var->getMax() :
         _xmax > var->getMax() ? var->getMax() :
         _xmin > _xmax         ? var->getMax() :
         _xmax;
  nbin = _nbin < 0 ? var->getBins() : _nbin;
}

/**
 * @class RooArgContainer
 * @details We will not be using the @ROOT{RooLinkedList} class because
 * that has the issues of deleting the command objects after going out of
 * scope which has causes unintended cansequences.
 */

/**
 * @brief constructing out argument container from a std::vector.
 *
 * This is not directly a copy constructor, but add additional detection
 * routines so that the command names are unique.
 */

RooArgContainer::RooArgContainer( const std::vector<RooCmdArg>& arglist )
{
  std::set<std::string> nameset;

  for( const auto& arg : arglist ){
    if( nameset.insert( arg.GetName() ).second ){
      push_back( arg );
    }
  }
}

/** brief Nothing to do... */
RooArgContainer::~RooArgContainer(){}

/**
 * @brief Checking if the argument list has a command of a given name.
 */
bool
RooArgContainer::Has( const std::string& name ) const
{
  auto iter = std::find_if( begin(), end(),
    [&name]( const RooCmdArg& item ){
        return item.GetName() == name;
      } );
  return iter != end();
}

/**
 * @brief Returning the argument in the list with a given name.
 *
 * @details No not-found protection would be performed.
 */
const RooCmdArg&
RooArgContainer::Get( const std::string& name ) const
{
  auto iter = std::find_if( begin(), end(),
    [&name]( const RooCmdArg& item ){
        return item.GetName() == name;
      } );
  return *iter;
}

/**
 * @class Roo1DFrame
 * @details
 * An simple wrapper object designed to expose additional features of the
 * RooPlot class (the internal TH1 used for axis plotting) and a feature for
 * allow the creation objects directly under the ownership of a RooPlot instance
 */

/**
 * @brief Construct a frame with a variable.
 * @brief Name of the object would be assigned a random string to avoid
 * collision
 */
Roo1DFrame::Roo1DFrame( const RangeByVar& range ) :
  RooPlot( RandomString( 12 ).c_str(), "",
           *range.var, range.xmin, range.xmax, range.nbin )
{

}

/**
 * @brief Construct a frame with just the x axis range.
 */
Roo1DFrame::Roo1DFrame( const double min, const double max ) :
  RooPlot( min, max )
{
  RooPlot::SetName( RandomString( 12 ).c_str() );
  RooPlot::SetTitle( "" );
}

/**
 * @brief Must allow for null constructor for pad that doesn't use a frame.
 */
Roo1DFrame::Roo1DFrame() :
  RooPlot()
{// Do nothing
}

Roo1DFrame::~Roo1DFrame(){}

}/* plt */

}/* usr */
