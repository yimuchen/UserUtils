#ifdef CMSSW_GIT_HASH
#include "UserUtils/PlotUtils/interface/PlotCommon.hpp"
#else
#include "UserUtils/PlotUtils/PlotCommon.hpp"
#endif

#include "CmdSetAttr.hpp"

#include <iostream>

inline int   Col( const RooCmdArg& x )  { return x.getInt( 0 ); }
inline float Alpha( const RooCmdArg& x ){ return x.getDouble( 0 ); }

extern void
SetTextAttr( TAttText&                   obj,
             const usr::RooArgContainer& args )
{
  if( args.Has( "TextColor" ) ){
    const auto cmd = args.Get( "TextColor" );
    obj.SetTextColorAlpha( Col( cmd ), Alpha( cmd ) );
  }

  if( args.Has( "TextSize" ) ){
    obj.SetTextSize( args.Get( "TextSize" ).getDouble( 0 ) );
  }

  if( args.Has( "TextAngle" ) ){
    obj.SetTextAngle( args.Get( "TextAngle" ).getDouble( 0 ) );
  }

  if( args.Has( "TextAlign" ) ){
    obj.SetTextAlign( args.Get( "TextAlign" ).getInt( 0 ) );
  }
}

extern void
SetLineAttr( TAttLine&                   obj,
             const usr::RooArgContainer& args  )
{
  if( args.Has( "LineColor" ) ){
    const auto cmd = args.Get( "LineColor" );
    obj.SetLineColorAlpha( Col( cmd ), Alpha( cmd ) );
  }

  if( args.Has( "LineStyle" ) ){
    obj.SetLineStyle( args.Get( "LineStyle" ).getInt( 0 ) );
  }

  if( args.Has( "LineWidth" ) ){
    obj.SetLineWidth( args.Get( "LineWidth" ).getInt( 0 ) );
  }
}

extern void
SetFillAttr( TAttFill&                   obj,
             const usr::RooArgContainer& args  )
{
  if( args.Has( "FillColor" ) ){
    const auto cmd = args.Get( "FillColor" );
    obj.SetFillColorAlpha( Col( cmd ), Alpha( cmd ) );
  }

  if( args.Has( "FillStyle" ) ){
    obj.SetFillStyle( args.Get( "FillStyle" ).getInt( 0 ) );
  }
}

extern void
SetMarkAttr( TAttMarker&                 obj,
             const usr::RooArgContainer& args  )
{
  if( args.Has( "MarkerColor" ) ){
    const auto cmd = args.Get( "MarkerColor" );
    obj.SetMarkerColorAlpha( Col( cmd ), Alpha( cmd ) );
  }

  if( args.Has( "MarkerStyle" ) ){
    obj.SetMarkerStyle( args.Get( "MarkerStyle" ).getInt( 0 ) );
  }

  if( args.Has( "MarkerSize" ) ){
    obj.SetMarkerSize( args.Get( "MarkerSize" ).getDouble( 0 ) );
  }
}
extern void
CopyTextAttrTo( const TAttText& source,
                TAttText&       target )
{
  target.SetTextColor( source.GetTextColor() ); // alpha handled
  target.SetTextSize( source.GetTextSize() );
  target.SetTextAngle( source.GetTextAngle() );
  target.SetTextAlign( source.GetTextAlign() );
}

extern void
CopyLineAttrTo( const TAttLine& source,
                TAttLine&       target )
{
  target.SetLineColor( source.GetLineColor() );// alpha already handled
  target.SetLineStyle( source.GetLineStyle() );
  target.SetLineWidth( source.GetLineWidth() );
}

extern void
CopyFillAttrTo( const TAttFill& source,
                TAttFill&       target )
{
  target.SetFillColor( source.GetFillColor() ); // alpha handled
  target.SetFillStyle( source.GetFillStyle() );
}

extern void
CopyMarkAttrTo( const TAttMarker& source,
                TAttMarker&       target )
{
  target.SetMarkerColor( source.GetMarkerColor() ); // alpha handled
  target.SetMarkerStyle( source.GetMarkerStyle() );
  target.SetMarkerSize( source.GetMarkerSize() );
}
