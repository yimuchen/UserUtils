/**
 * @file    Pad1D_helper.cc
 * @brief   Implementation of helper classes and function of Pad1D class
 *          including static member functions and helper classes.
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"
#include "UserUtils/PlotUtils/interface/Pad1D.hpp"

#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"

#include <algorithm>

namespace usr {

namespace plt {

/*-----------------------------------------------------------------------------
 *  Static member functions, Histogram min-max functions
   --------------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------------*/

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

/*-----------------------------------------------------------------------------
 *  Static member funtcions - Graph min-max functions
   --------------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------------*/

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

/*-----------------------------------------------------------------------------
 *  Static member functions - THStack min-max functions
   --------------------------------------------------------------------------*/

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

/*----------------------------------------------------------------------------*/

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

/*-----------------------------------------------------------------------------
 *  Additional plotting arguments
   --------------------------------------------------------------------------*/
const std::string PlotType::CmdName = "PlotType";
PlotType::PlotType( const int i ) :
  RooCmdArg( CmdName.c_str(), i ){}

PlotType::PlotType( const std::string& drawopt ) :
  RooCmdArg( CmdName.c_str(), 0, 0, 0, 0, drawopt.c_str() ){}

const std::string TrackY::CmdName = "TrackY";
TrackY::TrackY( const int i ) :
  RooCmdArg( CmdName.c_str(), i ){}

const std::string EntryText::CmdName = "EntryText";
EntryText::EntryText( const std::string& str ) :
  RooCmdArg( CmdName.c_str(), 0, 0, 0, 0, str.c_str() ){}


/*-----------------------------------------------------------------------------
 *  Constructor helper functions
   --------------------------------------------------------------------------*/
const double RangeByVar::find_default
  = std::numeric_limits<double>::lowest();

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

/*----------------------------------------------------------------------------*/

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

/*-----------------------------------------------------------------------------
 *  Argument parser
   --------------------------------------------------------------------------*/
RooArgContainer::RooArgContainer( const std::vector<RooCmdArg>& arglist )
{
  std::set<std::string> nameset;

  for( const auto& arg : arglist ){
    if( nameset.insert( arg.GetName() ).second ){
      push_back( arg );
    }
  }
}

RooArgContainer::~RooArgContainer(){}

/*----------------------------------------------------------------------------*/

bool
RooArgContainer::Has( const std::string& name ) const
{
  auto iter = std::find_if( begin(), end(),
    [&name]( const RooCmdArg& item ){
        return item.GetName() == name;
      } );
  return iter != end();
}

/*----------------------------------------------------------------------------*/

const RooCmdArg&
RooArgContainer::Get( const std::string& name ) const
{
  auto iter = std::find_if( begin(), end(),
    [&name]( const RooCmdArg& item ){
        return item.GetName() == name;
      } );
  return *iter;
}

/*-----------------------------------------------------------------------------
 *  Simple Roo1DFrame extension
   --------------------------------------------------------------------------*/
Roo1DFrame::Roo1DFrame( const RangeByVar& range ) :
  RooPlot( RandomString( 12 ).c_str(), "",
           *range.var, range.xmin, range.xmax, range.nbin )
{

}

Roo1DFrame::Roo1DFrame( const double min, const double max ) :
  RooPlot( min, max )
{
  RooPlot::SetName( RandomString( 12 ).c_str() );
  RooPlot::SetTitle( "" );
}

Roo1DFrame::Roo1DFrame() :
  RooPlot()
{// Do nothing
}

Roo1DFrame::~Roo1DFrame(){}

}/* plt */

}/* usr */
