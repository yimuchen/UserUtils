@defgroup PlotUtilsUser PlotUtilsUser
@ingroup  PlotUtils
@brief    User level classes for fast plotting with decent outputs.

While it is unambiguous to defined a rigid pad--canvas relation, it could be
inconvenient to the user who just wants to write a plot an figure.
This package provides classes that provides default layouts that are commonly
used in CMS analysis, as well as providing interface short hands that might be
more familiar to ROOT uses (e.x. plotting on a canvas).

@class usr::plt::Simple1DCanvas
@ingroup PlotUtilsUser
@brief Canvas with exactly 1 Pad1D instance

The Simple1DCanvas class, is a canvas containing exactly 1 Pad1D instance. Its
member functions are generated such that it looks like a users plotting calls
are directly performed on the canvas, greatly simplifying the plotting process.

This class is designed as a preset for simple data plotting, with the margin,
dimension and font defaults tuned to produce decent, publication ready plots
with minimal amount of user code.

The following are a few examples of plotting using the Simple1DCanvas (with
the style/color settings and data filling code removed)

# Plotting style with individual histograms

```cpp
  plt::Simple1DCanvas c;
  c.PlotHist( h1, plt::PlotType( plt::scatter ), plt::EntryText( "Bkg. 1" ) );
  c.PlotHist( h2, plt::PlotType( plt::hist ), plt::EntryText( "Bkg_{2}" ) );
  c.PlotHist( h3, plt::PlotType( plt::histerr ), plt::EntryText( "Signal" ) );
  c.DrawCMSLabel( "ROOTOBJ TEST", "CWS" );
  c.DrawLuminosity( 133.7 );
  c.SetHistAxisTitles( "P_{t}", plt::unit::GeVc );
  c.SetLogy( 1 );
  c.SaveAsPNG( "simple1dcanvas_log.png", 144 );
```

@image html simple1dcanvas_log.png

# Plotting with stacked histograms

```cpp
plt::Simple1DCanvas c;
c.PlotHist( h1, plt::PlotType( plt::histstack ), plt::EntryText("Bkg. 1") );
c.PlotHist( h2, plt::PlotType( plt::histstack ), plt::EntryText("Bkg. 2") );
c.PlotHist( h3, plt::PlotType( plt::hist ), plt::EntryText("Sig") );
c.DrawCMSLabel( "STACK TEST", "CWS" );
c.DrawLuminosity( 133.7 );
c.SetHistAxisTitles( "P_{t}", plt::unit::GeVc );
c.SaveAsPNG( "testfig/simple1dcanvas_stack.png", 144 );
```

@image html simple1dcanvas_stack.png

# Plotting with RooFit

```cpp
plt::Simple1DCanvas c2( plt::RangeByVar( x ) );

auto& dgraph = c2.PlotData( d );
auto& fgraph = c2.PlotPdf( g, RooFit::VisualizeError( *fit, 1, false ) );
// Styling made EASY!
dgraph.SetMarkerSize( 2 );
dgraph.SetLineColor( kBlack );
fgraph.SetLineColor( kBlue );
fgraph.SetFillColor( kCyan );

c2.DrawCMSLabel( "ROOFIT test", "CWS" );
c2.DrawLuminosity( 133.7 );
c2.SetHistAxisTitles( "M_{HHHHH}", plt::unit::GeVcc );

c2.SaveAsPNG( "testfig/simple1dcanvas_roofit_test.png", 144 );
```

@image html simple1dcanvas_roofit.png


@class   usr::plt::Ratio1DCanvas
@ingroup PlotUtilsUser
@brief   Canvas with exactly 2 Pad1D instances for comparison plots.

A canvas where the top pad displays the data, and the bottom pad is used to
display the ratio/pull between data sets is a common method of presenting
numbers in HEP. The ratio canvas, apart from setting up the pads for such
plotting, also provides tools to generate the required plots.

The `PlotObj` functions of the top pad is pass to the canvas, while additional
functions are used for plotting the ratio/pull plots onto the bottom pad. The
generated ratio/pull plot will be owned by the canvas and be destroyed when the
canvas goes out of scope.

The following is some examples of plots created with the Ratio1DCanvas class:

# Stack plots with error and signal comparison

```cpp
plt::Ratio1DCanvas c;

c.PlotHist( hist3, plt::PlotType( plt::histstack ), plt::EntryText( "Bkg_{1}: VV" ) );
c.PlotHist( hist2, plt::PlotType( plt::histstack ), plt::EntryText( "Bkg_{2}: HH" ) );
c.PlotHist( hist1, plt::PlotType( plt::histstack ), plt::EntryText( "Bkg_{3}: TT" ) );
c.PlotHist( histsum, plt::PlotType( plt::histerr ),   plt::EntryText( "Bkg_{1} Unc." ) );
c.PlotHist( data, plt::PlotType( plt::scatter ),   plt::EntryText( "Data" ) );

c.PlotScale( histsum, histsum, plt::PlotType( plt::histerr ), plt::TrackY( plt::TrackY::none ) );
c.PlotScale( data, histsum, plt::PlotType( plt::scatter ) );

c.TopPad().SetHistAxisTitles( "M_{THTHTH}", plt::unit::GeVcc );
c.BottomPad().SetHistAxisTitles( "M_{THTHTH}", plt::unit::GeVcc );
c.BottomPad().Yaxis().SetTitle( "Data/Sim." );

c.DrawCMSLabel( "Ratio1DCanvas", "CWS" );
c.DrawLuminosity( 133.7 );
c.SaveAsPNG( "testfig/ratio1dcanvas_test.png", 144 );
```

@image html ratio1dcanvas.png

# Plotting with RooFit and fitting uncertainties

```cpp
plt::Ratio1DCanvas c( plt::RangeByVar(x) );

auto& fitgraph = c.PlotPdf( g, plt::EntryText( "Fitted Bkg." ),
                               RooFit::VisualizeError( *fit, 1, false ),
                               RooFit::Normalization( 500, RooAbsReal::NumEvent ) );
auto& altgraph = c.PlotPdf( gx,  plt::EntryText( "Alt. Bkg. Model" ),
                                 RooFit::Normalization( 500, RooAbsReal::NumEvent ) );

auto& datgraph = c.PlotData( d, plt::EntryText("Fake Data") );

// Styling made Easy!
datgraph.SetMarkerStyle( 20 );
...

c.PlotScale( fitgraph, fitgraph,
             plt::TrackY(plt::TrackY::none),
             plt::PlotType(plt::fittedfunc) );
c.PlotScale( altgraph, fitgraph,
             plt::TrackY(plt::TrackY::none),
             plt::PlotType(plt::simplefunc) );
c.PlotScale( datgraph, fitgraph,
             plt::PlotType(plt::scatter) );

c.BottomPad().Yaxis().SetTitle("Data/Fit");
c.DrawCMSLabel( "Ratio1DCanvas", "CWS" );
c.DrawLuminosity( 133.7 );
c.SaveAsPNG("testfig/ratio1dcanvas_roofit_test.png",144);
```

@image html ratio1dcanvas_roofit.png
