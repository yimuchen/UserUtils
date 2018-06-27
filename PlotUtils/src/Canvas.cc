/**
 * @file    Canvas.cc
 * @brief   Better Canvas saving implementations
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"
#include "UserUtils/PlotUtils/interface/Canvas.hpp"
#include "UserUtils/PlotUtils/interface/Constants.hpp"
#else
#include "UserUtils/Common/STLUtils/StringUtils.hpp"
#include "UserUtils/PlotUtils/Canvas.hpp"
#include "UserUtils/PlotUtils/Constants.hpp"
#endif

#include "TFile.h"

#include <boost/format.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

namespace usr  {

namespace plt  {

/**
 * @brief Construct a new canvas with specific width height and font settings.
 *
 * The width and height parameters are in the standard ROOT dimensions units
 * (pixels). For publication dimensions like inches and centimeters, see
 * the functions defined in [PlotUtilsConvert](@ref PlotUtilsConvert).
 */
Canvas::Canvas(
  const length_t width,
  const length_t height,
  const FontSet& fontset
  ) :
  TCanvas( RandomString( 12 ).c_str(), "", width, height ),
  _fontset( fontset )
{}

/**
 * @brief Clears all the pad instances from memory stack.
 */
Canvas::~Canvas()
{
  for( auto pad : _padlist ){
    delete pad;
  }
}

/**
 * @brief Common subroutines for before the Canvas object is saved.
 *
 * This step includes:
 * - Creating the parent directory of the desired stored path.
 * - Looping through all the pads and calling their finalization function
 * @param finalpath [description]
 */
void
Canvas::Finalize( const fs::path& finalpath )
{
  MakeParent( finalpath );

  for( const auto& pad : _padlist ){
    pad->Finalize();
  }
}

/**
 * @brief PDF file saving with additional fixes
 *
 * Saving as PDF fixes the issue of the generated PDF files being rotated when
 * using XeLaTex (I like XeLaTex because I need to typeset in CJK), this
 * function generates a temporary PDF file using the @ROOT engine in the /tmp
 * directory and fixes the PDF file using ghostscript. Reference to the
 * ghostscript command can be found here:
 *  http://tex.stackexchange.com/questions/66522/xelatex-rotating-my-figures-in-beamer
 *
 * @param filepath final path to store the PDF file.
 *                 (Parent directory would be automatically created if possible)
 */
void
Canvas::SaveAsPDF( const fs::path& filepath )
{
  Finalize( filepath );
  const fs::path tmppath = SaveTempPDF( filepath );

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

/**
 * @brief High resolution PNG file saving
 *
 * The file creats a temporary PDF file using the stock @ROOT functions, and
 * uses ImageMagik to convert the PDF into a high resolution PNG file. The
 * `convert` command is take from
 [here](https://stackoverflow.com/questions/6605006/convert-pdf-to-image-with-high-resolution).
 * @param filepath final path to store the PDF file.
 *                 (Parent directory would be automatically created if possible)
 * @param dpi      Required image quality
 */
void
Canvas::SaveAsPNG( const fs::path& filepath, const unsigned dpi )
{
  Finalize( filepath );
  const fs::path tmppath = SaveTempPDF( filepath );

  const float scale = (float)len::ROOT_DPI/(float)dpi;

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

/**
 * @brief Saving the canvas to the a .root file, under a certain name.
 *
 * Since the canvas is assigned a random name during construction, when name
 * the object should be stored under in the root file has to be explicitly
 * specified, during the save call. The .root file (and parent directory) would
 * be created if it doesn't already exits. If an object with the specified name
 * already exists in the .root file, the object would be overwritten.
 */
void
Canvas::SaveToROOT( const fs::path& filepath, const std::string& objname )
{
  Finalize( filepath );
  TFile* myfile = TFile::Open( filepath.c_str(), "UPDATE" );
  TCanvas::Write( objname.c_str(), TFile::kOverwrite );
  delete myfile;
}

/**
 * @brief Saving as CPP file.
 *
 * Apart form addressing of unifying the interface, for the saving functions,
 * this function all addresses the issue of the @ROOT{TCanvas} only generating
 * functions with the .cxx prefix.
 * @param filepath file to save the macro file.
 */
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

/**
 * @brief Saving a temporary PDF file in the from of "/tmp/XXXXXX_<filename>.pdf"
 *
 * The random string at the front of the file is to help avoid collisions when
 * runny multiple plotting scripts at the same time.
 */
fs::path
Canvas::SaveTempPDF( const fs::path& finalpath )
{
  const std::string temppdf = boost::str( boost::format( "/tmp/%s_%s.pdf" )
    % usr::RandomString( 6 )
    % finalpath.stem().string() );

  TCanvas::SaveAs( temppdf.c_str() );
  return temppdf;
}

}/* plt  */

}/* usr  */
