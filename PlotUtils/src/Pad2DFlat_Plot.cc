#include "UserUtils/PlotUtils/interface/Pad2DFlat.hpp"
#include "UserUtils/PlotUtils/interface/PlotCommon.hpp"

#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"

#include "TLegendEntry.h"

namespace usr {

namespace plt {

TH2D&
Pad2DFlat::PlotHist( TH2D& hist, const std::vector<RooCmdArg>& arglist )
{
  const RooArgContainer args( arglist );

  const int opt =
    !args.Has( Plot2DF::CmdName ) ? plot2df::heat :
    args.Get( Plot2DF::CmdName ).getString( 0 ) ? plot2df_dummy :
    args.Get( Plot2DF::CmdName ).getInt( 0 );
  std::string optraw =
    !args.Has( Plot2DF::CmdName ) ?  "" :
    !args.Get( Plot2DF::CmdName ).getString( 0 ) ? "" :
    args.Get( Plot2DF::CmdName ).getString( 0 );

  const std::string legopt =
    opt == plot2df::heat ? "" :
    opt == plot2df::heatcont ? "L" :
    opt == plot2df::cont ? "L" :
    "PFLE";

  // Parsing plotting flag
  if( opt == plot2df::heat ){
    PlotObj( hist, "COLZ" );
    SetAxisFont();
  } else if( opt == plot2df::heatcont ){
    PlotObj( hist, "COLZ" );
    SetAxisFont();
    PlotObj( hist, "CONT3 SAME" );
  } else if( opt == plot2df::cont ){
    PlotObj( hist, "CONT3 SAME" );
  } else if( opt == plot2df_dummy && optraw != "" ){
    // Special case for raw options parsing. (Must remove axis and add string)
    ToUpper( optraw );
    StripSubstring( optraw, "AXIS" );
    PlotObj( hist, ( optraw+" SAME" ).c_str() );
  } else {// Skipping over stuff
    std::cerr << "Skipping over invalid value" << std::endl;
  }

  if( args.Has( EntryText::CmdName ) ){
    _legend.AddEntry( &hist,
      args.Get( EntryText::CmdName ).getString( 0 ), legopt.c_str() );
  }

  return hist;
}



TGraph2D&
Pad2DFlat::PlotGraph( TGraph2D& graph, const std::vector<RooCmdArg>& arglist )
{
  const RooArgContainer args( arglist );

  const int opt =
    !args.Has( Plot2DF::CmdName ) ? plot2df::heat :
    args.Get( Plot2DF::CmdName ).getString( 0 ) ? plot2df_dummy :
    args.Get( Plot2DF::CmdName ).getInt( 0 );
  std::string optraw =
    !args.Has( Plot2DF::CmdName ) ?  "" :
    !args.Get( Plot2DF::CmdName ).getString( 0 ) ? "" :
    args.Get( Plot2DF::CmdName ).getString( 0 );

  const std::string legopt =
    opt == plot2df::heat ? "" :
    opt == plot2df::heatcont ? "L" :
    opt == plot2df::cont ? "L" :
    "PFLE";

  if( !GetAxisObject() ){
    auto& axishist = _frame.MakeObj<TH2D>(
      ( "axishist" + RandomString( 12 ) ).c_str(), "",
      10, graph.GetXminE(), graph.GetXmaxE(),
      10, graph.GetYminE(), graph.GetYmaxE()
      );
    axishist.SetStats( 0 );
    PadBase::PlotObj( axishist, "AXIS" );
    SetAxisFont();
  }

  // Parsing plotting flag
  if( opt == plot2df::heat ){
    PlotObj( graph, "CONT4 SAME" );
    SetAxisFont();
  } else if( opt == plot2df::heatcont ){
    PlotObj( graph, "CONT4 SAME" );
    SetAxisFont();
    PlotObj( graph, "CONT3 SAME" );
  } else if( opt == plot2df::cont ){
    PlotObj( graph, "CONT3 SAME" );
  } else if( opt == plot2df_dummy && optraw != "" ){
    // Special case for raw options parsing. (Must remove axis and add string)
    ToUpper( optraw );
    StripSubstring( optraw, "A" );
    PlotObj( graph, optraw.c_str() );
  } else {// Skipping over stuff
    std::cerr << "Skipping over invalid value" << std::endl;
  }

  if( args.Has( EntryText::CmdName ) ){
    _legend.AddEntry( &graph,
      args.Get( EntryText::CmdName ).getString( 0 ), legopt.c_str() );
  }

  return graph;
}

TH2D&
Pad2DFlat::PlotFunc( TF2& func, const std::vector<RooCmdArg>& arglist )
{
  static const unsigned sep = 300;

  const double xmin = func.GetXmin();
  const double xmax = func.GetXmax();
  const double ymin = func.GetYmin();
  const double ymax = func.GetYmax();
  const double dx   = ( xmax-xmin )/sep;
  const double dy   = ( ymax-ymin )/sep;

  TH2D& hist = _frame.MakeObj<TH2D>(
    ( func.GetName() + RandomString( 12 ) ).c_str(), "",
    sep, xmin, xmax,
    sep, ymin, ymax );

  for( unsigned i = 0; i < sep; ++i ){
    for( unsigned j = 0; j < sep; ++j ){
      const double x = xmin + ( i + 0.5 ) * dx;
      const double y = ymin + ( j + 0.5 ) * dy;
      const double z = func.Eval( x, y );

      const int binidx = hist.FindBin( x, y );

      hist.SetBinContent( binidx, z );
      hist.SetBinError( binidx, 0 );
    }
  }

  return PlotHist( hist, arglist );
}

TGraph&
Pad2DFlat::Plot1DGraph( TGraph& graph, const std::vector<RooCmdArg>& arglist )
{
  const RooArgContainer args( arglist );

  const int opt =
    !args.Has( PlotType::CmdName ) ? plottype::scatter :
    args.Get( PlotType::CmdName ).getString( 0 ) ? plottype_dummy :
    args.Get( PlotType::CmdName ).getInt( 0 );
  std::string optraw =
    !args.Has( Plot2DF::CmdName ) ?  "" :
    !args.Get( Plot2DF::CmdName ).getString( 0 ) ? "" :
    args.Get( Plot2DF::CmdName ).getString( 0 );

  const std::string legopt = "PLE";

  // Parsing plotting flag
  if( opt == plottype::scatter ){
    PadBase::PlotObj( graph, "PZ0" );
  } else if( opt == plottype_dummy && optraw != "" ){
    // Special case for raw options parsing. (Must remove axis and add string)
    ToUpper( optraw );
    PadBase::PlotObj( graph, optraw.c_str() );
  } else {// Skipping over stuff
    std::cerr << "Skipping over invalid value" << std::endl;
  }

  if( args.Has( EntryText::CmdName ) ){
    _legend.AddEntry( &graph,
      args.Get( EntryText::CmdName ).getString( 0 ), legopt.c_str() );
  }

  return graph;
}


void
Pad2DFlat::MakeLegend()
{
  TPad::cd();
  float width  = 0;
  float height = 1.2*LineHeight() * _legend.GetListOfPrimitives()->GetEntries();

  for( const auto&& obj : *_legend.GetListOfPrimitives() ){
    const char* label = ( (TLegendEntry*)obj )->GetLabel();
    TLatex* textmp    = new TLatex( 0, 0, label );
    textmp->SetTextFont( FontFace() );
    textmp->SetTextSize( FontSize() );
    width = std::max( int(width),
      int(1.3 *textmp->GetXsize() * AbsWidth() + 5) );
    delete textmp;
  }

  width *= 1.1;// Relieving spacing a little
  width += 1.0 * LineHeight();// Reserving space for legend icon boxes.

  // For whatever reason, the y corordinates of the TLegend counts from the
  // top.... Don't ask. Just ROOT things
  const float xmin = 0.5 * float(LineHeight()) / AbsWidth() ;
  const float ymax = 1   - GetTopMargin() ;
  const float xmax = GetLeftMargin() - 5*float(LineHeight()) / AbsWidth();
  const float ymin = ymax - height/ AbsHeight();
  _legend.SetX1NDC( xmin );
  _legend.SetX2NDC( xmax );
  _legend.SetY1NDC( ymin );
  _legend.SetY2NDC( ymax );

  if( !TPad::FindObject( &_legend ) ){
    PadBase::PlotObj( _legend );
  }
}


}

}
