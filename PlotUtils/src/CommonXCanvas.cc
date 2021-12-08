/**
 * @file    CommonXCanvas.cc
 * @brief   Implementation of the CommonXCanvas functions.
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/Maths.hpp"
#include "UserUtils/PlotUtils/interface/CommonXCanvas.hpp"
#else
#include "UserUtils/Common/Maths.hpp"
#include "UserUtils/PlotUtils/CommonXCanvas.hpp"
#endif

namespace usr
{

namespace plt
{

// Defining static variables
length_t CommonXCanvas::default_width  = 0.95 * len::a4textwidth_default();
length_t CommonXCanvas::default_height = 0.587 * len::a4textwidth_default();
FontSet  CommonXCanvas::default_font   = FontSet();


CommonXCanvas::CommonXCanvas( const unsigned number_of_pads,
                              const length_t width,
                              const length_t height,
                              const FontSet& font ) :
  Canvas( width, height, font )
{
  for( unsigned i = 0; i < number_of_pads; ++i ){
    Add<Pad1D>( PadSize( 0, 0, 1, 1 ) );
  }

  _init_margin();
}


CommonXCanvas::CommonXCanvas( const RangeByVar& range,
                              const unsigned    number_of_pads,
                              const length_t    width,
                              const length_t    height,
                              const FontSet&    font ) :
  Canvas( width, height, font )
{
  for( unsigned i = 0; i < number_of_pads; ++i ){
    Add<Pad1D>( PadSize( 0, 0, 1, 1 ), range );
    Pad( i ).SetAxisFont();
    if( i != _padlist.size()-1 ){
      Pad( i ).Xaxis().SetTitleSize( 0 );
      Pad( i ).Xaxis().SetLabelSize( 0 );
    }
  }

  _init_margin();
}


CommonXCanvas::~CommonXCanvas(){}

void
CommonXCanvas::_init_margin()
{
  static const float pad_space           = 0.005;
  const unsigned     npads               = _padlist.size();
  const float        frame_top_offset    = 1.5 * Font().lineheight() / Height();
  const float        frame_bottom_offset = 2.5 * Font().lineheight() / Height();
  const float        frame_height        = 1-frame_top_offset
                                           -frame_bottom_offset;
  const float pad_height = frame_height / npads;

  for( unsigned i = 0; i < npads; ++i ){
    Pad( i ).SetLeftMargin(   3.5 * Font().lineheight() / Width() );
    Pad( i ).SetRightMargin( std::min(
                               ( 4.0 * Font().lineheight() / Height()
                                 -( 3.5 * Font().lineheight() / Width() ) ),
                               ( 0.8 * Font().lineheight() / Width() ) ));

    Pad( i ).SetTopMargin(
      frame_top_offset+i * pad_height+sgn( i ) * 0.5 * pad_space );

    Pad( i ).SetBottomMargin(
      frame_bottom_offset+( npads-i-1 ) * pad_height+sgn( i ) *  0.5
      * pad_space );

    Pad( i ).TPad_().SetFillColorAlpha( kWhite, 0 );
  }
}


void
CommonXCanvas::CheckPadAxis( const TObject*obj )
{
  if( FirsTPad_().GetAxisObject() ){
    return;
  }
  if( obj->InheritsFrom( TH1::Class() ) ){

    for( unsigned i = 0; i < _padlist.size(); ++i ){
      TH1*axisobj = (TH1*)obj->Clone();
      axisobj->Reset();
      axisobj->SetStats( 0 );
      axisobj->SetTitle( "" );
      axisobj->SetName( ( "CommonXaxis"+RandomString( 6 ) ).c_str() );
      Pad( i ).PlotObj( axisobj, "AXIS" );
      Pad( i ).ClaimObject( axisobj );
      Pad( i ).SetAxisFont();
      if( i != _padlist.size()-1 ){
        Pad( i ).Xaxis().SetTitleSize( 0 );
        Pad( i ).Xaxis().SetLabelSize( 0 );
      }
    }
  } else if( obj->InheritsFrom( TGraph::Class() ) ){
    const TGraph*graph = dynamic_cast<const TGraph*>( obj );
    for( unsigned i = 0; i < _padlist.size(); ++i ){
      auto& axishist = Pad( i ).MakeObj<TH1D>(
        ( "CommonXaxis"+RandomString( 6 ) ).c_str(),
        "",
        10,
        GetXmin( graph ),
        GetXmax( graph ) );
      axishist.SetStats( 0 );
      Pad( i ).PlotObj( axishist, "AXIS" );
      Pad( i ).SetAxisFont();
      if( i != _padlist.size()-1 ){
        Pad( i ).Xaxis().SetTitleSize( 0 );
        Pad( i ).Xaxis().SetLabelSize( 0 );
      }
    }
  } else if( obj->InheritsFrom( TF1::Class() ) ){
    const TF1*func = dynamic_cast<const TF1*>( obj );
    for( unsigned i = 0; i < _padlist.size(); ++i ){
      auto& axishist = Pad( i ).MakeObj<TH1D>(
        ( "CommonXaxis"+RandomString( 6 ) ).c_str(),
        "",
        10,
        func->GetXmin(),
        func->GetXmax() );
      axishist.SetStats( 0 );
      Pad( i ).PlotObj( axishist, "AXIS" );
      Pad( i ).SetAxisFont();
      if( i != _padlist.size()-1 ){
        Pad( i ).Xaxis().SetTitleSize( 0 );
        Pad( i ).Xaxis().SetLabelSize( 0 );
      }
    }
  }

  return;
}

}

}
