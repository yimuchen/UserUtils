/**
 * @file    simple1dcanvas.cc
 * @brief   Testing the simple 1d Canvas.
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/PlotUtils/interface/Simple1DCanvas.hpp"
#else
#include "UserUtils/PlotUtils/Simple1DCanvas.hpp"
#endif

#include "RooDataSet.h"
#include "RooGaussian.h"
#include "TRandom3.h"

namespace plt = usr::plt;

int
main( int argc, char* argv[] )
{
  {// Histogram testing
    TRandom3 r;
    TH1D h1( "h1", "", 21, -10, 10 );
    TH1D h2( "h2", "", 21, -10, 10 );
    TH1D h3( "h3", "", 21, -10, 10 );

    for( int i = 0; i < 5000; ++i ){
      h1.Fill( r.Gaus( 0, 3 ) );
      h2.Fill( r.Gaus( 1, 4 ) );
      h3.Fill( r.Gaus( -2, 1 ), r.Gaus( 0.5, 0.1 ) );
    }

    plt::Simple1DCanvas c;
    c.PlotHist( h1, plt::PlotType( plt::scatter ), plt::EntryText( "Bkg. 1" ) );
    c.PlotHist( h2, plt::PlotType( plt::hist ),    plt::EntryText( "Bkg. 2" ) );
    c.PlotHist( h3, plt::PlotType( plt::histerr ), plt::EntryText( "Signal" ) );
    c.DrawCMSLabel( "ROOTOBJ TEST", "CWS" );
    c.DrawLuminosity( 133.7 );
    c.SetHistAxisTitles( "P_{t}", plt::unit::GeVc );

    // Styling
    h1.SetLineColor( kBlack );
    h1.SetMarkerStyle( 20 );
    h1.SetMarkerSize( 0.2 );
    h2.SetLineColor( kBlue );
    h3.SetFillColor( kGray );
    h3.SetFillStyle( plt::sty::filldotdense );

    c.SaveAsPNG( "image/simple1dcanvas.png",         72 );
    c.SaveAsPNG( "image/simple1dcanvas_highres.png", 300 );
    c.SaveAsPDF( "image/simple1dcanvas.pdf" );
    c.SetLogy( 1 );
    c.SaveAsPNG( "image/simple1dcanvas_log.png",         72 );
    c.SaveAsPNG( "image/simple1dcanvas_log_highres.png", 300 );
    c.SaveAsPDF( "image/simple1dcanvas_log.pdf" );
  }

  {// Stacking test
    TRandom3 r;
    TH1D h1( "h1_new", "", 21, -10, 10 );
    TH1D h2( "h2_new", "", 21, -10, 10 );
    TH1D h3( "h3_new", "", 21, -10, 10 );

    for( int i = 0; i < 5000; ++i ){
      h1.Fill( r.Gaus( 0, 3 ) );
      h2.Fill( r.Gaus( 1, 4 ) );
      h3.Fill( r.Gaus( -2, 1 ), r.Gaus( 0.5, 0.1 ) );
    }

    // Styling must happend before plotting
    h1.SetFillStyle( plt::sty::fillsolid );
    h1.SetFillColor( kCyan );
    h1.SetLineColor( kCyan );
    h2.SetFillStyle( plt::sty::filldotdense );
    h2.SetFillColor( kPink );
    h2.SetLineColor( kPink );
    h3.SetLineColor( kBlue );


    plt::Simple1DCanvas c;
    c.PlotHist( h1,
      plt::PlotType( plt::histstack ),
      plt::EntryText( "Bkg. 1" ) );
    c.PlotHist( h2,
      plt::PlotType( plt::histstack ),
      plt::EntryText( "Bkg. 2" ) );
    c.PlotHist( h3,
      plt::PlotType( plt::hist ),
      plt::EntryText( "Sig" ) );

    c.DrawCMSLabel( "STACK TEST", "CWS" );
    c.DrawLuminosity( 133.7 );
    c.SetHistAxisTitles( "P_{t}", plt::unit::GeVc );

    c.SaveAsPNG( "image/simple1dcanvas_stack.png",         72 );
    c.SaveAsPNG( "image/simple1dcanvas_stack_highres.png", 300 );
    c.SaveAsPDF( "image/simple1dcanvas_stack.pdf" );
    c.SetLogy( 1 );
    c.SaveAsPNG( "image/simple1dcanvas_stack_log.png",         72 );
    c.SaveAsPNG( "image/simple1dcanvas_stack_log_highres.png", 300 );
    c.SaveAsPDF( "image/simple1dcanvas_stack_log.pdf" );
  }


  // ROOFIT testing
  {
    RooRealVar x( "x", "x", -10, 10, "GeV/#it{c}^2" );
    RooRealVar m( "m", "m", 0.3, -5, 5 );
    RooRealVar s( "s", "s", 2, 0, 5 );
    RooGaussian g( "g", "g", x, m, s );
    RooDataSet* d     = g.generate( RooArgSet( x ), 800 );
    RooFitResult* fit = g.fitTo( *d,
      RooFit::Save(),
      RooFit::Verbose( false ),
      RooFit::PrintLevel( -1 ),
      RooFit::Warnings( false ),
      RooFit::PrintEvalErrors( 0 ) );
    plt::Simple1DCanvas c2( plt::RangeByVar( x ),
                            plt::Simple1DCanvas::default_width );

    auto& dgraph = c2.PlotData( d );
    auto& fgraph = c2.PlotPdf( g,
      RooFit::VisualizeError( *fit, 1, false ),
      usr::plt::PlotUnder( dgraph ) );

    dgraph.SetMarkerStyle( 20 );
    dgraph.SetMarkerSize( 0.2 );
    dgraph.SetLineColor( kBlack );
    fgraph.SetLineColor( kBlue );
    fgraph.SetFillColor( kCyan );

    c2.DrawCMSLabel( "ROOFIT test", "CWS" );
    c2.DrawLuminosity( 133.7 );
    c2.SetHistAxisTitles( "M_{HHHHH}", plt::unit::GeVcc );

    c2.SaveAsPNG( "image/simple1dcanvas_roofit.png",         72 );
    c2.SaveAsPNG( "image/simple1dcanvas_roofit_highres.png", 300 );
    c2.SaveAsPDF( "image/simple1dcanvas_roofit.pdf" );
    c2.SetLogy( 1 );
    c2.SaveAsPNG( "image/simple1dcanvas_roofit_log.png",         72 );
    c2.SaveAsPNG( "image/simple1dcanvas_roofit_log_highres.png", 300 );
    c2.SaveAsPDF( "image/simple1dcanvas_roofit_log.pdf" );
  }


  return 0;
}
