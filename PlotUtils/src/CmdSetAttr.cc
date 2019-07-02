#include "CmdSetAttr.hpp"

#include <iostream>

extern void
SetTextAttr( TAttText&                        obj,
             const usr::plt::RooArgContainer& args )
{
  if( args.Has( usr::plt::TextColor::CmdName ) ){
    obj.SetTextColorAlpha(
      args.Get( usr::plt::TextColor::CmdName ).getInt( 0 ),
      args.Get( usr::plt::TextColor::CmdName ).getDouble( 0 )
      );
  }

  if( args.Has( usr::plt::TextSize::CmdName ) ){
    obj.SetTextSize( args.Get( usr::plt::TextSize::CmdName ).getDouble( 0 ) );
  }
}

extern void
SetLineAttr( TAttLine&                        obj,
             const usr::plt::RooArgContainer& args  )
{
  if( args.Has( usr::plt::LineColor::CmdName ) ){
    obj.SetLineColorAlpha(
      args.Get( usr::plt::LineColor::CmdName ).getInt( 0 ),
      args.Get( usr::plt::LineColor::CmdName ).getDouble( 0 )
      );
  }

  if( args.Has( usr::plt::LineStyle::CmdName ) ){
    obj.SetLineStyle( args.Get( usr::plt::LineStyle::CmdName ).getInt( 0 ) );
  }

  if( args.Has( usr::plt::LineWidth::CmdName ) ){
    obj.SetLineWidth( args.Get( usr::plt::LineWidth::CmdName ).getInt( 0 ) );
  }
}

extern void
SetFillAttr( TAttFill&                        obj,
             const usr::plt::RooArgContainer& args  )
{
  if( args.Has( usr::plt::FillColor::CmdName ) ){
    obj.SetFillColorAlpha(
      args.Get( usr::plt::FillColor::CmdName ).getInt( 0 ),
      args.Get( usr::plt::FillColor::CmdName ).getDouble( 0 )
      );
  }

  if( args.Has( usr::plt::FillStyle::CmdName ) ){
    obj.SetFillStyle( args.Get( usr::plt::FillStyle::CmdName ).getInt( 0 ) );
  }
}

extern void
SetMarkAttr( TAttMarker&                      obj,
             const usr::plt::RooArgContainer& args  )
{
  if( args.Has( usr::plt::MarkerColor::CmdName ) ){
    obj.SetMarkerColorAlpha(
      args.Get( usr::plt::MarkerColor::CmdName ).getInt( 0 ),
      args.Get( usr::plt::MarkerColor::CmdName ).getDouble( 0 )
      );
  }

  if( args.Has( usr::plt::MarkerStyle::CmdName ) ){
    obj.SetMarkerStyle( args.Get( usr::plt::MarkerStyle::CmdName ).getInt( 0 ) );
  }

  if( args.Has( usr::plt::MarkerSize::CmdName ) ){
    obj.SetMarkerSize( args.Get( usr::plt::MarkerSize::CmdName ).getDouble( 0 ) );
  }
}
