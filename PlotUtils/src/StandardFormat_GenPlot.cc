#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/Maths.hpp"
#include "UserUtils/Common/interface/STLUtils.hpp"
#include "UserUtils/MathUtils/interface/Measurement.hpp"
#include "UserUtils/PlotUtils/interface/Ratio1DCanvas.hpp"
#include "UserUtils/PlotUtils/interface/Simple1DCanvas.hpp"
#include "UserUtils/PlotUtils/interface/StandardPlotFormat.hpp"
#else
#include "UserUtils/Common/Maths.hpp"
#include "UserUtils/Common/STLUtils.hpp"
#include "UserUtils/MathUtils/Measurement.hpp"
#include "UserUtils/PlotUtils/Ratio1DCanvas.hpp"
#include "UserUtils/PlotUtils/Simple1DCanvas.hpp"
#include "UserUtils/PlotUtils/StandardPlotFormat.hpp"
#endif

#include "TColor.h"


namespace usr {

namespace plt {

namespace fmt {

void BatchRequest::GeneratePlots()
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

void BatchRequest::GenerateSampleComparePlot()
{
  for( const auto& histrequest : histlist ){
    Simple1DCanvas c;

    for( const auto& process : signallist ){
      c.PlotHist( process.GetNormalizedClone( histrequest.filekey ),
        usr::plt::PlotType( usr::plt::hist ),
        usr::plt::LineColor( TColor::GetColor( process.color.c_str() ) ),
        usr::plt::EntryText( process.name ) );
    }

    c.Pad().SetHistAxisTitles( histrequest.xaxis
                             , histrequest.units
                             , histrequest.yaxis );

    c.DrawCMSLabel( "Simulation", "CMS" );
    c.SaveAsPDF( histrequest.name + ".pdf" );
  }
}

void BatchRequest::PlotOnPad( const HistRequest& histrequest, Pad1D& pad )
{
  // Plotting the background
  for( unsigned i = background.size()-1; i < background.size(); --i ){
    const auto& group = background.at( i );

    const auto stack_hist = _background_stack.at( i ).get();

    pad.PlotHist( stack_hist,
      usr::plt::PlotType( usr::plt::histstack ),
      usr::plt::FillColor( TColor::GetColor( group.color.c_str() ) ),
      usr::plt::FillStyle( usr::plt::sty::fillsolid ),
      usr::plt::LineColor( TColor::GetColor( group.color.c_str() ) ),
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
                                       , _total_luminosity ),
      usr::plt::PlotType( usr::plt::hist ),
      usr::plt::LineColor( TColor::GetColor( signal.color.c_str() ) ),
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


void BatchRequest::GenerateBackgroundObjects( const HistRequest& hist )
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
      TH1D* central = process.GetScaledClone( filekey, _total_luminosity );
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
            process.GetScaledClone( unc_up_key, _total_luminosity ) );
          std::unique_ptr<TH1D> unc_lo_hist(
            process.GetScaledClone( unc_lo_key, _total_luminosity ) );
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

void BatchRequest::GenerateData( const HistRequest& hist )
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
