/**
 * @file    pad1d_test.cc
 * @brief   Testing the simple 1d Canvas.
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */

#include "UserUtils/PlotUtils/interface/Simple1DCanvas.hpp"

#include "RooDataSet.h"
#include "RooGaussian.h"
#include "TRandom3.h"

namespace plt = usr::plt;

int
main( int argc, char* argv[] )
{
  // Basic object building
  TRandom3 r;
  TH1D h1( "h1", "", 21, -10, 10 );
  TH1D h2( "h2", "", 21, -10, 10 );
  TH1D h3( "h3", "", 21, -10, 10 );

  for( int i = 0; i < 5000; ++i ){
    h1.Fill( r.Gaus( 0, 3 ) );
    h2.Fill( r.Gaus( 1, 4 ) );
    h3.Fill( r.Gaus( -2, 1 ), r.Gaus( 0.5, 0.1 ) );
  }

  // Styling
  h1.SetLineColor( kBlack );
  h1.SetMarkerSize( plt::sty::mkrpixels( 5 ) );
  h2.SetLineColor( kBlue );
  h3.SetFillColor( kGray );
  h3.SetFillStyle( plt::sty::filldotdense );

  {// Histogram testing
    plt::Simple1DCanvas c;
    c.PlotHist( h1, plt::PlotType( plt::scatter ), plt::EntryText( "Bkg. 1" ) );
    c.PlotHist( h2, plt::PlotType( plt::hist ), plt::EntryText( "Bkg_{2}" ) );
    c.PlotHist( h3, plt::PlotType( plt::histerr ), plt::EntryText( "Signal" ) );
    c.DrawCMSLabel( "ROOTOBJ TEST" );
    c.DrawLuminosity( 133.7 );
    c.SetHistAxisTitles( "P_{t}", plt::unit::GeVc );

    c.SaveAsPDF( "testfig/simple1dcanvas_test.pdf" );
    c.SetLogy( 1 );
    c.SaveAsPDF( "testfig/simple1dcanvas_log_test.pdf" );
  }

  {// Stacking test
    plt::Simple1DCanvas c;
    c.PlotHist( h1, plt::PlotType( plt::histstack ) );
    c.PlotHist( h2, plt::PlotType( plt::histstack ) );
    c.PlotHist( h3, plt::PlotType( plt::hist ) );

    c.DrawCMSLabel( "STACK TEST" );
    c.DrawLuminosity( 133.7 );
    c.SetHistAxisTitles( "P_{t}", plt::unit::GeVc );

    h1.SetFillStyle( plt::sty::fillsolid );
    h2.SetFillStyle( plt::sty::filldotdense );
    h1.SetFillColor( kCyan );
    h2.SetFillColor( kPink );

    c.SaveAsPDF( "testfig/simple1dcanvas_stack_test.pdf" );
    c.SetLogy( 1 );
    c.SaveAsPDF( "testfig/simple1dcanvas_stack_log_test.pdf" );

  }


  // ROOFIT testing
  RooRealVar x( "x", "x", -10, 10, "GeV/#it{c}^2" );
  RooRealVar m( "m", "m", 0.3, -5, 5 );
  RooRealVar s( "s", "s", 2, 0, 5 );
  RooGaussian g( "g", "g", x, m, s );
  RooDataSet* d     = g.generate( RooArgSet( x ), 5000 );
  RooFitResult* fit = g.fitTo( *d,
    RooFit::Save(),
    RooFit::Verbose( false ),
    RooFit::PrintLevel( -1 ),
    RooFit::Warnings( false ),
    RooFit::PrintEvalErrors( 0 ) );
  {// New plotting
    plt::Simple1DCanvas c2( plt::RangeByVar( x ),
      plt::Simple1DCanvas::default_width );

    auto& dgraph = c2.PlotData( d );
    auto& fgraph = c2.PlotPdf( g, RooFit::VisualizeError( *fit, 1, false ) );

    // SetDataStyle( dgraph ) ;
    // Styling
    dgraph.SetMarkerSize( plt::sty::mkrpixels( 6 ) );
    dgraph.SetLineColor( kBlack );
    fgraph.SetLineColor( kBlue );
    fgraph.SetFillColor( kCyan );

    c2.DrawCMSLabel( "ROOFIT test" );
    c2.DrawLuminosity( 133.7 );
    c2.SetHistAxisTitles( "M_{HHHHH}", plt::unit::GeVcc );

    c2.SaveAsPDF( "testfig/simple1dcanvas_roofit_test.pdf" );
    c2.SetLogy( 1 );
    c2.SaveAsPDF( "testfig/simple1dcanvas_roofit_log_test.pdf" );

    c2.SaveAsCPP( "testfig/roofittest.cxx" );
    // should at least have internal axis
  }


  return 0;
}
