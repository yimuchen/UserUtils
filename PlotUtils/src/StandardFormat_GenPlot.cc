/**
 * @file StandardFormat_GenPlot.cc
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2020-06-15
 *
 * @copyright Copyright (c) 2020
 *
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/Maths.hpp"
#include "UserUtils/Common/interface/STLUtils.hpp"
#include "UserUtils/MathUtils/interface/Measurement.hpp"
#include "UserUtils/PlotUtils/interface/Flat2DCanvas.hpp"
#include "UserUtils/PlotUtils/interface/Ratio1DCanvas.hpp"
#include "UserUtils/PlotUtils/interface/Simple1DCanvas.hpp"
#include "UserUtils/PlotUtils/interface/StandardPlotFormat.hpp"
#else
#include "UserUtils/Common/Maths.hpp"
#include "UserUtils/Common/STLUtils.hpp"
#include "UserUtils/MathUtils/Measurement.hpp"
#include "UserUtils/PlotUtils/Flat2DCanvas.hpp"
#include "UserUtils/PlotUtils/Ratio1DCanvas.hpp"
#include "UserUtils/PlotUtils/Simple1DCanvas.hpp"
#include "UserUtils/PlotUtils/StandardPlotFormat.hpp"
#endif

namespace usr {

namespace plt {

namespace fmt {

/**
 * @brief The highest level process for generating standard background stack
 * histogram plots.
 *
 * The generation of the Background objects (stack, and uncertainty histograms)
 * are handled in the BatchRequest::GenerateBackgroundObjects method. Handling of
 * the plotting sequences are defined in the BatchReqest::PlotOnPad method. This
 * method handles the additional sequences such as the distinction between the
 * data/background comparison functions, and the histogram request loop.
 */
void
BatchRequest::GeneratePlots()
{
  for( const auto histrequest :  histlist ){
    GenerateBackgroundObjects( histrequest );
    GenerateData( histrequest );


    if( histrequest.type == "simple" ){
      Simple1DCanvas c;
      PlotOnPad( histrequest, c.Pad() );

      c.SaveAsPDF( histrequest.name + ".pdf" );
    } else {
      Ratio1DCanvas c;
      PlotOnPad( histrequest, c.TopPad() );

      if( histrequest.type == "pull" ){
        c.PlotPull( _data_hist.get(), _background_sys.get()
                  , usr::plt::PlotType( usr::plt::scatter ) );
        c.BottomPad().SetHistAxisTitles( histrequest.xaxis
                                       , histrequest.units
                                       , histrequest.yaxis  );
        c.BottomPad().Yaxis().SetTitle( "#frac{Data - Bkg.}{Bkg. unc}" );
      } else {
        c.PlotScale( _background_sys.get(), _background_stat.get()
                   , usr::plt::PlotType( usr::plt::histerr )
                   , usr::plt::FillColor( usr::plt::col::gray )
                   , usr::plt::FillStyle( usr::plt::sty::fillsolid ) );
        c.PlotScale( _data_hist.get(), _background_stat.get()
                   , usr::plt::PlotType( usr::plt::scatter ) );
        c.BottomPad().SetHistAxisTitles( histrequest.xaxis
                                       , histrequest.units
                                       , histrequest.yaxis  );
        c.BottomPad().Yaxis().SetTitle( "Data/Bkg." );
      }
      c.SaveAsPDF( histrequest.name + ".pdf" );
    }
  }
}

/**
 * @brief Generating all comparisons of signal histograms.
 *
 * Each histogram is generated to match a total luminosity of 1.0pb.
 */
void
BatchRequest::GenerateSampleComparePlot()
{
  for( const auto& histrequest : histlist ){
    Simple1DCanvas c;

    for( unsigned i = signallist.size()-1; i < signallist.size(); --i ){
      const auto& process = signallist.at( i );
      if( !process.CheckKey( histrequest.filekey ) ){
        usr::fout(
          "Warning! Objects for plot [%s] in process [%s] is not found!\n"
          "\t File: %s\n"
          "\t Key:  %s\n"
          "The plot would not be plotted\n",
          histrequest.name, process.name,
          process.file,
          process.MakeKey( histrequest.filekey )
          );
        continue;
      }
      auto x = c.PlotHist( process.GetScaledClone( histrequest.filekey, 1.0 ),
        usr::plt::PlotType( usr::plt::hist ),
        usr::plt::TrackY( usr::plt::tracky::both ),
        usr::plt::LineColor( usr::plt::col::color( process.color ) ),
        usr::plt::EntryText( process.name ) );
    }

    c.Pad().SetHistAxisTitles( histrequest.xaxis
                             , histrequest.units
                             , histrequest.yaxis );

    c.Pad().SetLogy( histrequest.logy );

    c.DrawCMSLabel( "Simulation", "CMS" );
    c.SaveAsPDF( histrequest.name + ".pdf" );
  }
}

/**
 * @brief Plotting all comparison of 2D histogram distributions.
 *
 * Each signal samples will have their 2D histogram plotted as a scatter plot.
 * Again, the samples are plotted in reverse order to ensure that the item listed
 * first will also appear on top and first in the histogram.
 */
void
BatchRequest::Generate2DComaprePlot()
{
  for( const auto& histrequest : histlist ){
    Flat2DCanvas c;

    for( unsigned i = signallist.size()-1; i < signallist.size(); --i ){
      const auto& process = signallist.at( i );

      if( !process.CheckKey( histrequest.filekey ) ){
        usr::fout(
          "Warning! Objects for plot [%s] in process [%s] is not found!\n"
          "\t File: %s\n"
          "\t Key:  %s\n"
          "The plot would not be plotted\n",
          histrequest.name, process.name,
          process.file,
          process.MakeKey( histrequest.filekey )
          );
        continue;
      }
      c.PlotHist( process.Get2DClone( histrequest.filekey ),
        usr::plt::Plot2DF( usr::plt::density ),
        usr::plt::MarkerColor(
          usr::plt::col::color( process.color ), process.transparency ),
        usr::plt::FillColor(
          usr::plt::col::color( process.color ), process.transparency ),
        usr::plt::MarkerSize( 0.05 ),
        usr::plt::EntryText( process.name ) );
    }

    c.Pad().Xaxis().SetTitle( histrequest.xaxis.c_str() );
    c.Pad().Yaxis().SetTitle( histrequest.yaxis.c_str() );

    const std::string name = iosetting.output_prefix + histrequest.name +
                             iosetting.output_postfix + ".pdf";

    c.SaveAsPDF( name );
  }
}

/**
 * @brief Plotting the standard 1D histogram on a Pad.
 *
 * The background is plotted first, followed by the background uncertainties,
 * then the signal samples, then the data. Plotting of the standard luminosity
 * labels and the CMS label in the upper left corner of the pad is also performed
 * at this stage, as well as the naming of the axis titles.
 */
void
BatchRequest::PlotOnPad( const HistRequest& histrequest, Pad1D& pad )
{
  // Plotting the background
  for( unsigned i = background.size()-1; i < background.size(); --i ){
    const auto& group = background.at( i );

    const auto stack_hist = _background_stack.at( i ).get();

    pad.PlotHist( stack_hist,
      usr::plt::PlotType( usr::plt::histstack ),
      usr::plt::FillColor( usr::plt::col::color( group.color ) ),
      usr::plt::FillStyle( usr::plt::sty::fillsolid ),
      usr::plt::LineColor( usr::plt::col::color( group.color ) ),
      usr::plt::EntryText( group.name ) );
  }

  if( _background_stat ){
    pad.PlotHist( _background_sys.get(),
      usr::plt::PlotType( usr::plt::histerr ),
      usr::plt::FillColor( usr::plt::col::gray, 0.5 ),
      usr::plt::FillStyle( usr::plt::sty::fillsolid ),
      usr::plt::LineColor( usr::plt::col::gray, 0.5 ),
      usr::plt::MarkerStyle( usr::plt::sty::mkrcircle ),
      usr::plt::MarkerSize( 0.0 ),
      usr::plt::EntryText( "Bkg. unc (sys.)", true ) );
  }

  pad.PlotHist( _background_stat.get(),
    usr::plt::PlotType( usr::plt::histerr ),
    usr::plt::FillColor( usr::plt::col::slategray, 0.5 ),
    usr::plt::FillStyle( usr::plt::sty::filldotdense ),
    usr::plt::LineColor( usr::plt::col::slategray, 0.5 ),
    usr::plt::MarkerStyle( usr::plt::sty::mkrcircle ),
    usr::plt::MarkerSize( 0.0 ),
    usr::plt::EntryText( "Bkg. unc (stat.)", true ) );

  for( const auto& signal : signallist ){
    pad.PlotHist( signal.GetScaledClone( histrequest.filekey
                                       , _total_luminosity * signal.scale ),
      usr::plt::PlotType( usr::plt::hist ),
      usr::plt::LineColor( usr::plt::col::color( signal.color ) ),
      usr::plt::EntryText( signal.name ) );
  }

  pad.PlotHist( _data_hist.get(),
    usr::plt::PlotType( usr::plt::scatter ),
    usr::plt::MarkerSize( 0.2 ),
    usr::plt::MarkerStyle( usr::plt::sty::mkrcircle ),
    usr::plt::MarkerColor( usr::plt::col::black ),
    usr::plt::LineColor( usr::plt::col::black ),
    usr::plt::EntryText( data.name ) );

  pad.DrawLuminosity( _total_luminosity/1000 );
  pad.DrawCMSLabel( "Preliminary" );

  pad.SetHistAxisTitles( histrequest.xaxis
                       , histrequest.units
                       , histrequest.yaxis );

  pad.SetLogy( histrequest.logy );
}

/**
 * @brief Generating objects required for the 1D background plotting.
 *
 * This functions generates the following objects:
 * - The THStack for different color display
 * - A histogram for displaying the statistical uncertainty.
 * - A histogram for displaying the total systematic uncertainties (listed in the
 *   "uncertainties" of the BatchReqest).
 *
 * The THStack and the statistical uncertainty histograms are simple: simply
 * sum/stack up the central template for each of the background processes (taking
 * care to use the appropriate bin-error options for the statistical
 * uncertainty).
 *
 * For the histogram displaying the systematic, 2 templates are generated from
 * the uncertainty source templates summed over all process, which essentially
 * assumes that the uncertainties between each processes for the same uncertainty
 * source is 100% correlated. For each uncertainty source, the contribution to
 * the uncertainty per bin is then calculated as if each uncertainty is
 * uncorrelated (using the methods defined for the `usr::Measurement` class).
 *
 * All three histogram objects are owned by the BatchReqest class and will be
 * destroyed between different plots.
 */
void
BatchRequest::GenerateBackgroundObjects( const HistRequest& hist )
{
  std::vector<std::unique_ptr<TH1D> > unc_histlist;
  const std::string filekey = hist.filekey;

  // Clearing previous results;
  _background_stat = nullptr;
  _background_sys  = nullptr;
  _background_stack.clear();

  // Inserting two histograms for the full coverage
  for( unsigned i = 0; i < uncertainties.size(); ++i ){
    unc_histlist.push_back( nullptr );
    unc_histlist.push_back( nullptr );
  }

  // First loop over background. Generating background uncertainty templates
  for( const auto& group : background ){
    _background_stack.push_back( nullptr );

    for( const auto& process : group ){
      if( !process.CheckKey( filekey ) ){
        usr::fout( "Histogram [%s] with object name for process [%s] doesn't "
          "exists! Background normalization might be broken\n",
          hist.name, hist.filekey, process.name );
        continue;
      }
      TH1D* central = process.GetScaledClone( filekey
                                            , _total_luminosity *process.scale );
      if( _background_stack.back() == nullptr ){
        _background_stack.back().reset( central );
      } else {
        _background_stack.back()->Add( central );
      }

      // Generating the systematic shape uncertainty templates
      for( unsigned i = 0; i < uncertainties.size(); ++i ){
        const auto& unc = uncertainties.at( i );
        if( unc_histlist.at( 2*i ) == nullptr ){
          unc_histlist[2*i].reset( (TH1D*)( central->Clone() ) );
          unc_histlist[2*i]->Reset( 0 );
          unc_histlist[2*i+1].reset( (TH1D*)( central->Clone() ) );
          unc_histlist[2*i+1]->Reset( 0 );
        }

        if( unc.name == "Cross Section" ){
          std::unique_ptr<TH1D> xsec_up_hist( (TH1D*)central->Clone() );
          std::unique_ptr<TH1D> xsec_lo_hist( (TH1D*)central->Clone() );
          xsec_up_hist->Scale( 1 + process.cross_section.RelUpperError() );
          xsec_lo_hist->Scale( 1 - process.cross_section.RelLowerError() );
          unc_histlist[2*i]->Add( xsec_up_hist.get() );
          unc_histlist[2*i+1]->Add( xsec_lo_hist.get() );
        } else if( unc.key == "" ){
          std::unique_ptr<TH1D> xsec_up_hist( (TH1D*)central->Clone() );
          std::unique_ptr<TH1D> xsec_lo_hist( (TH1D*)central->Clone() );
          xsec_up_hist->Scale( 1 + unc.norm_uncertainty.RelUpperError() );
          xsec_lo_hist->Scale( 1 - unc.norm_uncertainty.RelLowerError() );
          unc_histlist[2*i]->Add( xsec_up_hist.get() );
          unc_histlist[2*i+1]->Add( xsec_lo_hist.get() );
        } else {
          const std::string unc_up_key
            = process.CheckKey( filekey + unc.key + "Up" ) ?
              filekey + unc.key + "Up" : filekey;
          const std::string unc_lo_key
            = process.CheckKey( filekey + unc.key + "Down" )  ?
              filekey + unc.key + "Down" : filekey;
          std::unique_ptr<TH1D> unc_up_hist(
            process.GetScaledClone( unc_up_key
                                  , _total_luminosity * process.scale  ) );
          std::unique_ptr<TH1D> unc_lo_hist(
            process.GetScaledClone( unc_lo_key
                                  , _total_luminosity * process.scale ) );
          unc_histlist[2*i]->Add( unc_up_hist.get() );
          unc_histlist[2*i+1]->Add( unc_lo_hist.get() );
        }
      }

      // For statistic uncertainty plot just sum central value
      if( _background_stat == nullptr ){
        _background_stat.reset( (TH1D*)central->Clone() );
        _background_stat->Reset();
      }
      _background_stat->Add( central );

      // Creating empty histogram for background part.
      if( _background_sys == nullptr ){
        _background_sys.reset( (TH1D*)central->Clone() );
        _background_sys->Reset();
      }
    }
  }

  // Looping over background uncertainties: Treat each template as uncorrelated.
  for( int bin = 1;
       bin <= _background_sys->GetNcells() && uncertainties.size() > 0; ++bin ){
    usr::Measurement total_unc( 1, 0, 0 );

    const double cen_binval = _background_stat->GetBinContent( bin );

    for( unsigned i = 0; i < uncertainties.size(); ++i  ){
      const TH1D* unc_up_hist = unc_histlist.at( 2*i ).get();
      const TH1D* unc_lo_hist = unc_histlist.at( 2*i+1 ).get();

      const double upval = unc_up_hist->GetBinContent( bin );
      const double loval = unc_lo_hist->GetBinContent( bin );
      const double max   = std::max( upval, std::max( cen_binval, loval ) );
      const double min   = std::min( upval, std::min( cen_binval, loval ) );

      const double up_err = max - cen_binval;
      const double lo_err = cen_binval - min;

      usr::Measurement sunc = usr::Measurement( cen_binval, up_err, lo_err );
      sunc = sunc.NormParam();

      total_unc *= sunc;
    }

    total_unc = total_unc.NormParam();

    const double center = cen_binval * ( total_unc.UpperValue()
                                         + total_unc.LowerValue() ) /2;
    const double error = cen_binval * ( total_unc.UpperValue()
                                        - total_unc.LowerValue() ) / 2;

    _background_sys->SetBinContent( bin, center );
    _background_sys->SetBinError( bin, error );
  }
}

/**
 * @brief Making the data histogram
 *
 * All data histograms should be unweighted, so we are getting a simple histogram
 * sum. The only adjustment we need is to set the histogram uncertainties to the
 * committee recommend asymmetric uncertainty type (TH1:kPoisson)
 */
void
BatchRequest::GenerateData( const HistRequest& hist )
{
  _data_hist = nullptr;

  for( const auto process : data ){
    if( _data_hist == nullptr ){
      _data_hist.reset( process.GetClone( hist.filekey ) );
    } else {
      _data_hist->Add( process.GetClone( hist.filekey ) );
    }
  }

  _data_hist->SetBinErrorOption( TH1::kPoisson );
}


// Don't write anything beyond these lines --------------------------------------
// Namespace ends
// ------------------------------------------------------------------------------
}

}

}
