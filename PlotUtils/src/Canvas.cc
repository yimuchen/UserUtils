/**
 * @file    Canvas.cc
 * @brief   Better Canvas saving implementations
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"
#include "UserUtils/PlotUtils/interface/Canvas.hpp"
#include "UserUtils/PlotUtils/interface/Constants.hpp"

#include "TFile.h"

#include <boost/format.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

namespace usr  {

namespace plt  {

/*-----------------------------------------------------------------------------
 *  Static variables
   --------------------------------------------------------------------------*/
float FontSet::default_size       = font::normalsize;
font::face FontSet::default_face  = font::helvetica;
float FontSet::default_lineheight = font::em( 1.1 );

FontSet FullPageFont = FontSet();
FontSet HalfPageFont = FontSet( font::footnote() );

/*-----------------------------------------------------------------------------
 *  Canvas constructor and destructors
   --------------------------------------------------------------------------*/
Canvas::Canvas(
  const length_t width,
  const length_t height,
  const FontSet& fontset
  ) :
  TCanvas( RandomString( 12 ).c_str(), "", width, height ),
  _fontset( fontset )
{}

Canvas::~Canvas()
{
  for( auto pad : _padlist ){
    delete pad;
  }
}

/*-----------------------------------------------------------------------------
 *  Saving functions - These are dileberately made to be noisy.
   --------------------------------------------------------------------------*/
void
Canvas::Finalize( const fs::path& finalpath )
{
  MakeParent( finalpath );

  for( const auto& pad : _padlist ){
    pad->Finalize();
  }
}

/*----------------------------------------------------------------------------*/

void
Canvas::SaveAsPDF( const fs::path& filepath )
{
  Finalize( filepath );
  const fs::path tmppath = SaveTempPDF( filepath );

  // Command found at
  // http://tex.stackexchange.com/questions/66522/
  // xelatex-rotating-my-figures-in-beamer
  const std::string cmd = boost::str( boost::format(
      "gs"
      "  -sDEVICE=pdfwrite"
      "  -dCompatibilityLevel=1.4"
      "  -dPDFSETTINGS=/screen"
      "  -dNOPAUSE"
      "  -dQUIET"
      "  -dBATCH"
      "  -sOutputFile=%s"
      "  %s"
      ) % filepath % tmppath );
  std::system( cmd.c_str() );
  system( ( "rm "+ tmppath.string() ).c_str() );

  std::cout << "Saving Canvas to " << filepath << std::endl;
}

/*----------------------------------------------------------------------------*/

void
Canvas::SaveToROOT( const fs::path& filepath, const std::string& objname )
{
  Finalize( filepath );
  TFile* myfile = TFile::Open( filepath.c_str(), "UPDATE" );
  TCanvas::Write( objname.c_str(), TFile::kOverwrite );
  delete myfile;
}

/*----------------------------------------------------------------------------*/

void
Canvas::SaveAsPNG( const fs::path& filepath, const unsigned dpi )
{
  Finalize( filepath );
  const fs::path tmppath = SaveTempPDF( filepath );

  const float scale = (float)len::ROOTDPI/(float)dpi;
  // Command found at
  // https://stackoverflow.com/questions/6605006/
  // convert-pdf-to-image-with-high-resolution
  const std::string cmd = boost::str( boost::format(
      "convert"
      "   -density %d"
      "   -trim"// removing additional transparenct around the edges
      "   %s"
      "   -quality 100"
      "   -sharpen 0x1.0"
      "   -resize %lf%%"
      "   %s"
      ) % dpi % tmppath % ( scale*100 ) % filepath );

  std::system( cmd.c_str() );
  std::system( ( "rm "+ tmppath.string() ).c_str() );

  std::cout << "Saving Canvas to " << filepath << std::endl;
}

/*----------------------------------------------------------------------------*/

fs::path
Canvas::SaveTempPDF( const fs::path& finalpath )
{
  // Saving to a temporary file /tmp/XXXXXX_<filename>.pdf
  // Forcing postfix to be '.pdf'
  const std::string temppdf = boost::str( boost::format( "/tmp/%s_%s.pdf" )
    % usr::RandomString( 6 )
    % finalpath.stem().string() );

  // Default saving options
  TCanvas::SaveAs( temppdf.c_str() );
  MakeParent( finalpath );
  return temppdf;
}

/*----------------------------------------------------------------------------*/

void
Canvas::SaveAsCPP( const fs::path& filepath )
{
  Finalize( filepath );
  const std::string tempfile = boost::str( boost::format( "/tmp/%s_%s.cxx" )
    % RandomString( 6 )
    % filepath.stem().string() );
  TCanvas::SaveAs( tempfile.c_str() );

  std::system( ( "cp "+tempfile+" "+filepath.string() ).c_str() );
  std::system( ( "rm "+tempfile ).c_str() );
}

/*-----------------------------------------------------------------------------
 *  Font dimension access functions
   --------------------------------------------------------------------------*/
float
Canvas::FontSize() const { return _fontset.size; }

short
Canvas::FontFace() const { return font::fontface( _fontset.face ); }

float
Canvas::LineHeight() const { return _fontset.lineheight; }


/*-----------------------------------------------*/
/*-----------------------------------------------*/
/*-----------------------------------------------*/

/*-----------------------------------------------------------------------------
 *  PadBase functions
   --------------------------------------------------------------------------*/
PadBase::PadBase( const PadSize& size ) :
  TPad( RandomString( 12 ).c_str(), "",
        size.xmin, size.ymin, size.xmax, size.ymax )
{
  // Additional TPad initialization
  TPad::SetTicks( 1, 1 );// Common setting in HEP plots

  // Latex initializing
  _latex.SetTextFont( ParentCanvas().FontFace() );
  _latex.SetTextSize( ParentCanvas().FontSize() );
  _latex.SetTextAlign( font::top_left );
}

/*----------------------------------------------------------------------------*/

PadBase::~PadBase(){}

/*----------------------------------------------------------------------------*/

Canvas&
PadBase::ParentCanvas() const
{
  // Dynamic cast and static not working because of public inheritance
  return *( (Canvas*)( TPad::GetMother() ) );
}

/*-----------------------------------------------------------------------------
 *  Dimension access functions
   --------------------------------------------------------------------------*/
double
PadBase::RelWidth() const { return TPad::GetWNDC(); }

double
PadBase::RelHeight() const { return TPad::GetHNDC(); }

double
PadBase::AbsWidth() const { return ParentCanvas().Width() * RelWidth(); }

double
PadBase::AbsHeight() const { return ParentCanvas().Height() * RelHeight(); }


/*-----------------------------------------------------------------------------
 *  Latex writing functions
   --------------------------------------------------------------------------*/
PadBase&
PadBase::WriteAtData( const double x, const double y, const std::string& line )
{
  TPad::cd();
  _latex.DrawLatex( x, y, line.c_str() );
  return *this;
}

/*----------------------------------------------------------------------------*/

PadBase&
PadBase::WriteLine( const std::string& line )
{
  TPad::cd();
  _latex.DrawLatexNDC( _latex_cursorx, _latex_cursory, line.c_str() );
  _latex_cursory -= RelLineHeight();
  return *this;
}

/*----------------------------------------------------------------------------*/

PadBase&
PadBase::SetTextCursor( const double x, const double y, const font::align a )
{
  _latex.SetTextAlign( a );
  return SetTextCursor( x, y );
}

/*----------------------------------------------------------------------------*/

PadBase&
PadBase::SetTextCursor( const double x, const double y )
{
  _latex_cursorx = x;
  _latex_cursory = y;
  return *this;
}

/*-----------------------------------------------------------------------------
 *  Drawing options
   --------------------------------------------------------------------------*/
void
PadBase::PlotObj( TObject& obj, Option_t* opt )
{
  TPad::cd();
  obj.Draw( opt );
}

}/* plt  */

}/* usr  */
