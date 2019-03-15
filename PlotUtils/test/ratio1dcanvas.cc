/**
 * @file    ratiocanvas_test.cc
 * @brief   Testing the default value settings of the ratiocanvas class
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/PlotUtils/interface/Ratio1DCanvas.hpp"
#else
#include "UserUtils/PlotUtils/Ratio1DCanvas.hpp"
#endif

#include "TH1D.h"
#include "TRandom3.h"
#include "RooGaussian.h"
#include "RooDataSet.h"
#include "RooConstVar.h"
#include <iostream>

namespace plt = usr::plt;

template<typename T>
void
SetDataStyle( T& x )
{
  x.SetMarkerStyle( 20 );
  x.SetMarkerSize( 0.5 );
  x.SetMarkerSize( kBlack );
}

int
main( int argc, char* argv[] )
{
  {// Root object testing.
    TH1D hist1( "h1", "", 21, -10.5, 10.5 );
    TH1D hist2( "h2", "", 21, -10.5, 10.5 );
    TH1D hist3( "h3", "", 21, -10.5, 10.5 );
    TH1D histsum( "sig", "", 21, -10.5, 10.5 );
    TH1D data( "dat", "", 21, -10.5, 10.5 );
    TRandom3 rnd;

    for( size_t i = 0; i < 5000; i++ ){
      hist1.Fill( rnd.Gaus( -2, 2 ), 0.3 );
      hist2.Fill( rnd.Gaus( -2.3, 2.5 ), 0.3*rnd.Uniform( 0.0, 0.6 ) );
      hist3.Fill( rnd.Gaus( -1.7, 3.0 ), 0.3*rnd.Uniform( 0.0, 0.4 ) );
      data.Fill( rnd.Gaus( -2, 2.2 ), 0.5 );
    }

    histsum.Add( &hist1 );
    histsum.Add( &hist2 );
    histsum.Add( &hist3 );

    // Artificially magnifying uncertainty
    for( int i = 0; i < histsum.GetNcells(); i++ ){
      histsum.SetBinError( i,
        std::min( histsum.GetBinError( i )*1.5*rnd.Uniform( 0.8, 1.2 ),
          histsum.GetBinContent( i ) ) );
      data.SetBinError( i,
        std::min( histsum.GetBinError( i )*2*rnd.Uniform( 0.8, 1.2 ),
          histsum.GetBinContent( i ) ) );
    }

    // Styling
    data.SetLineColor( kBlack );
    data.SetMarkerStyle( 20 );
    data.SetMarkerSize( 0.2 );

    histsum.SetFillStyle( plt::sty::filldotdense );
    histsum.SetFillColor( kGray );

    hist1.SetFillStyle( plt::sty::fillsolid );
    hist1.SetFillColor( kCyan );
    hist1.SetLineColor( kCyan );
    hist2.SetFillStyle( plt::sty::fillsolid );
    hist2.SetFillColor( kRed );
    hist2.SetLineColor( kRed );
    hist3.SetFillStyle( plt::sty::fillsolid );
    hist3.SetFillColor( kBlue );
    hist3.SetLineColor( kBlue );

    plt::Ratio1DCanvas c;

    c.PlotHist( hist3,
      plt::PlotType( plt::histstack ), plt::EntryText( "Bkg_{1}: VV" ) );
    c.PlotHist( hist2,
      plt::PlotType( plt::histstack ), plt::EntryText( "Bkg_{2}: HH" ) );
    c.PlotHist( hist1,
      plt::PlotType( plt::histstack ), plt::EntryText( "Bkg_{3}: TT" ) );
    c.PlotHist( histsum,
      plt::PlotType( plt::histerr ),   plt::EntryText( "Bkg_{1} Unc." ) );
    c.PlotHist( data,
      plt::PlotType( plt::scatter ),   plt::EntryText( "Data" ) );

    c.PlotScale( histsum, histsum,
      plt::PlotType( plt::histerr ), plt::TrackY( plt::TrackY::none ) );
    c.PlotScale( data, histsum,
      plt::PlotType( plt::scatter ) );

    c.TopPad().SetHistAxisTitles( "M_{THTHTH}", plt::unit::GeVcc );
    c.BottomPad().SetHistAxisTitles( "M_{THTHTH}", plt::unit::GeVcc );
    c.BottomPad().Yaxis().SetTitle( "Data/Sim." );

    c.DrawCMSLabel( "Ratio1DCanvas", "CWS" );
    c.DrawLuminosity( 133.7 );
    c.SaveAsPNG( "image/ratio1dcanvas.png", 72 );
    c.SaveAsPNG( "image/ratio1dcanvas_highres.png", 300 );
    c.SaveAsPDF( "image/ratio1dcanvas_highres.pdf" );
    // c.SaveAsCPP( "image/ratio1dcanvas_test.cc" );
  }
  {// RooFit Testing
    RooRealVar x( "x", "x", -10, 10, plt::unit::GeVcc.c_str() );
    RooRealVar m( "m", "m", -2, -5, 5 );
    RooRealVar s( "s", "s", 1.5, 0, 5 );
    RooGaussian g( "g", "g", x, m, s );
    RooGaussian gx( "gx", "gx", x,
      RooFit::RooConst( -1.85 ), RooFit::RooConst( 1.40 ) );
    RooDataSet* d     = g.generate( RooArgSet( x ), 500 );
    RooFitResult* fit = g.fitTo( *d,
      RooFit::Save(),
      RooFit::Verbose( false ),
      RooFit::PrintLevel( -1 ),
      RooFit::Warnings( false ),
      RooFit::PrintEvalErrors( 0 ) );

    plt::Ratio1DCanvas c( plt::RangeByVar(x),
      plt::Ratio1DCanvas::default_width );
    // c.PlotData( d, RooFit::Invisible() );
    auto& fitgraph = c.PlotPdf( g,
      plt::EntryText( "Fitted Bkg." ),
      RooFit::VisualizeError( *fit, 1, false ),
      RooFit::Normalization( 500, RooAbsReal::NumEvent ) );
    auto& altgraph = c.PlotPdf( gx,
      plt::EntryText( "Alt. Bkg. Model" ),
      RooFit::Normalization( 500, RooAbsReal::NumEvent ) );
    auto& datgraph = c.PlotData( d, plt::EntryText("Fake Data") );

    datgraph.SetMarkerStyle( 20 );
    datgraph.SetMarkerSize( 0.2 );
    datgraph.SetLineColor( kBlack );
    fitgraph.SetLineColor( kBlue );
    fitgraph.SetFillColor( kCyan );
    altgraph.SetLineColor( kRed );
    altgraph.SetFillColorAlpha( kWhite, 0 );

    c.PlotScale( fitgraph, fitgraph,
       plt::TrackY(plt::TrackY::none), plt::PlotType(plt::fittedfunc) );
    c.PlotScale( altgraph, fitgraph,
       plt::TrackY(plt::TrackY::none), plt::PlotType(plt::simplefunc) );
    c.PlotScale( datgraph, fitgraph,
      plt::PlotType(plt::scatter) );

    c.BottomPad().Yaxis().SetTitle("Data/Fit");

    c.DrawCMSLabel( "Ratio1DCanvas", "CWS" );
    c.DrawLuminosity( 133.7 );
    c.SaveAsPNG("image/ratio1dcanvas_roofit.png", 72 );
    c.SaveAsPNG("image/ratio1dcanvas_roofit_highres.png", 300 );
    c.SaveAsPDF("image/ratio1dcanvas_roofit.pdf" );
  }

  return 0;
}
