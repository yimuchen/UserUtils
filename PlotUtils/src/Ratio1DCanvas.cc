/**
 * @file    Ratio1DCanvas.cc
 * @brief   Implementation of the Ratio1DCanvas functions.
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */
#include "UserUtils/PlotUtils/interface/Ratio1DCanvas.hpp"

namespace usr  {

namespace plt {


/*-----------------------------------------------------------------------------
 *  Constructor and destructor
   --------------------------------------------------------------------------*/
float PadRatio::default_ratio         = 4;
float PadRatio::default_gap           = 0.005;
length_t Ratio1DCanvas::default_width = 0.45*len::a4textwidth_default();
length_t Ratio1DCanvas::default_height
  = ( 5./4. )*0.45*len::a4textwidth_default();

Ratio1DCanvas::Ratio1DCanvas(
  const length_t  width,
  const length_t  height,
  const PadRatio& pad,
  const FontSet&  set
  ) : Canvas( width, height, set )
{
  Add<Top1DPad>(    PadSize( 0, _splitNDC( pad.ratio ), 1, 1 ) );
  Add<Bottom1DPad>( PadSize( 0, 0, 1, _splitNDC( pad.ratio ) ) );
  _init_margin( pad.gap );
}

Ratio1DCanvas::Ratio1DCanvas(
  const RangeByVar& range,
  const length_t    width,
  const length_t    height,
  const PadRatio&   pad,
  const FontSet&    set
  ) : Canvas( width, height, set )
{
  Add<Top1DPad>(    PadSize( 0, _splitNDC( pad.ratio ), 1, 1 ), range );
  Add<Bottom1DPad>( PadSize( 0, 0, 1, _splitNDC( pad.ratio ) ), range );
  _init_margin( pad.gap );
  TopPad().SetAxisFont();
  BottomPad().SetAxisFont();
}

Ratio1DCanvas::~Ratio1DCanvas(){}

/*----------------------------------------------------------------------------*/

float
Ratio1DCanvas::_splitNDC( const float x ) const
{
  return ( 1-3.5*LineHeight()/Height() )/( x+1. )
         + 3.*LineHeight()/Height();
}

/*----------------------------------------------------------------------------*/

void
Ratio1DCanvas::_init_margin( const float gap )
{
  SetTopMargin( 1.5*LineHeight()/Height() );
  SetLeftMargin( 3.5*LineHeight()/Width() );
  SetBottomMargin( 2.0*LineHeight()/Height() );
  SetRightMargin(
    std::max(
      ( 3.5*LineHeight()/Height() - ( 3.5*LineHeight()/Width() ) ),
      ( 0.6*LineHeight()/Width() ) )
    );

  // Additional margin setting for gap
  TopPad().SetBottomMargin( 0.5 * gap * Height() / TopPad().AbsHeight() );
  BottomPad().SetTopMargin( 0.5 * gap * Height() / BottomPad().AbsHeight() );
}

/*-----------------------------------------------------------------------------
 *  Margin Setting functions
   --------------------------------------------------------------------------*/
void
Ratio1DCanvas::SetTopMargin( const float x )
{
  TopPad().SetTopMargin( x * Height() / TopPad().AbsHeight() );
  TCanvas::SetTopMargin( x );
}

void
Ratio1DCanvas::SetLeftMargin( const float x )
{
  TopPad().SetLeftMargin( x );
  BottomPad().SetLeftMargin( x );
  TCanvas::SetLeftMargin( x );
}

void
Ratio1DCanvas::SetRightMargin( const float x )
{
  TopPad().SetRightMargin( x );
  BottomPad().SetRightMargin( x );
  TCanvas::SetRightMargin( x );
}

void
Ratio1DCanvas::SetBottomMargin( const float x )
{
  BottomPad().SetBottomMargin( x * Height() / BottomPad().AbsHeight() );
  TCanvas::SetBottomMargin( x );
}

/*-----------------------------------------------------------------------------
 *  Top/Bottom pad Axis font setting re-setting;
   --------------------------------------------------------------------------*/
void
Top1DPad::SetAxisFont()
{
  Pad1D::SetAxisFont();
  Xaxis().SetTitleSize( 0 );
  Xaxis().SetLabelSize( 0 );
  Yaxis().SetTitleOffset( 2.0 );
}

void
Bottom1DPad::SetAxisFont()
{
  Pad1D::SetAxisFont();
  Xaxis().SetTitleOffset( 2.5 );
  Yaxis().SetNdivisions( 503 );// Better ticks for axis box
  Yaxis().SetTitleOffset( 2.0 );
}


/*-----------------------------------------------------------------------------
 *  Plotting bottom pad functions
   --------------------------------------------------------------------------*/
TH1D&
Ratio1DCanvas::PlotScale(
  const TH1D&                   num,
  const TH1D&                   den,
  const std::vector<RooCmdArg>& arglist )
{
  TH1D* ans = ScaleDivide( &num, &den );
  BottomPad().FrameObj().addObject( ans );

  // Pargin arguments.
  const RooArgContainer args( arglist );
  const RooCmdArg pltopt =
    args.Has( PlotType::CmdName ) ? args.Get( PlotType::CmdName ) :
    PlotType( plt::hist );
  const RooCmdArg trkopt =
    args.Has( TrackY::CmdName ) ? args.Get( TrackY::CmdName ) :
    TrackY( TrackY::both );

  BottomPad().RangeType() = Pad1D::rangetype::ratio;
  BottomPad().PlotHist( ans, pltopt, trkopt );

  return *ans;
}

TGraphAsymmErrors&
Ratio1DCanvas::PlotScale(
  const TGraph&                 num,
  const TGraph&                 den,
  const std::vector<RooCmdArg>& arglist )
{
  TGraphAsymmErrors* ans = ScaleDivide( &num, &den );
  BottomPad().FrameObj().addObject( ans );

  const RooArgContainer args( arglist );
  const RooCmdArg pltopt =
    args.Has( PlotType::CmdName ) ? args.Get( PlotType::CmdName ) :
    PlotType( plt::simplefunc );
  const RooCmdArg trkopt =
    args.Has( TrackY::CmdName ) ? args.Get( TrackY::CmdName ) :
    TrackY( TrackY::both );

  BottomPad().RangeType() = Pad1D::rangetype::ratio;
  BottomPad().PlotGraph( ans, pltopt, trkopt );

  return *ans;
}


/*-----------------------------------------------------------------------------
 *  Static functions - Scale divide
   --------------------------------------------------------------------------*/
TH1D*
Ratio1DCanvas::ScaleDivide(
  const TH1D*  num,
  const TH1D*  den,
  const double cen
  )
{
  TH1D* ans = new TH1D( *num );

  for( int i = 0; i < num->GetNcells(); ++i ){
    const double n = num->GetBinContent( i );
    const double e = num->GetBinError( i );
    const double d = den->GetBinContent( i );
    if( n == 0 || d == 0 ){
      ans->SetBinContent( i, cen );
      ans->SetBinError( i, 0 );
    } else {
      ans->SetBinContent( i, n/d );
      ans->SetBinError( i, e/d );
    }
  }

  return ans;
}

/*----------------------------------------------------------------------------*/

TGraphAsymmErrors*
Ratio1DCanvas::ScaleDivide(
  const TGraph* num,
  const TGraph* den,
  const double  cen
  )
{
  TGraphAsymmErrors* ans = new TGraphAsymmErrors(num->GetN());
  // duplicating style
  ans->SetLineColor( num->GetLineColor() );
  ans->SetLineStyle( num->GetLineStyle() );
  ans->SetLineWidth( num->GetLineWidth() );
  ans->SetFillColor( num->GetFillColor() );
  ans->SetFillStyle( num->GetFillStyle() );
  ans->SetMarkerColor( num->GetMarkerColor() );
  ans->SetMarkerStyle( num->GetMarkerStyle() );
  ans->SetMarkerSize( num->GetMarkerSize() );

  for( int i = 0; i < num->GetN(); ++i ){
    const double origx  = num->GetX()[i];
    const double origy  = num->GetY()[i];
    const double xerrlo = num->GetErrorXlow( i );
    const double xerrhi = num->GetErrorXhigh( i );
    const double yerrlo = num->GetErrorYlow( i );
    const double yerrhi = num->GetErrorYhigh( i );

    const double deny = den->Eval( origx );

    if( deny == 0 || origy == 0 ){
      ans->SetPoint( i, origx, cen );
      ans->SetPointError( i, 0, 0, 0, 0 );
    } else {
      ans->SetPoint( i, origx, origy / deny );
      ans->SetPointError(
        i,
        xerrlo, xerrhi,
        yerrlo/deny, yerrhi/deny );
    }
  }

  ans->SetTitle( "" );
  ans->GetXaxis()->SetLimits(
    num->GetXaxis()->GetXmin(),
    num->GetXaxis()->GetXmax() );

  return ans;
}

}/* plt */

}/* usr  */
