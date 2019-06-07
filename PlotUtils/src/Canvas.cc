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
#include <fstream>
#include <iostream>
#include <string>

#include <ghostscript/iapi.h>
#include <ghostscript/ierrors.h>

static bool run_ghostscript( const std::vector<std::string>& );

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

  TPad::cd();
  TCanvas::Update();
}

/**
 * @brief PDF file saving with additional fixes
 *
 * The PDF file generated by ROOT has several problems. First is that the PDF
 * files are rotated when the using XeLaTex, due to the incorrect PDF tags (I
 * like XeLaTex because I need to typeset in CJK). Second is that the PDF
 * dimensions is wrong: ROOT generateds a fixed page size (842pt x 595pt), while
 * the Cropbox is scaled accordingly.
 *
 * This function generates a temporary PDF file using the @ROOT engine in the
 * /tmp directory and fixes the PDF file using ghostscript. Reference to the
 * ghostscript command can be found here:
 * - For fixing the rotation:
 *   http://tex.stackexchange.com/questions/66522/xelatex-rotating-my-figures-in-beamer:
 * - For detecting the Cropbox size:
 *   https://stackoverflow.com/questions/2943281/using-ghostscript-to-get-page-size
 *
 * @param filepath final path to store the PDF file. (Parent directory would be
 *                 automatically created if possible)
 */
void
Canvas::SaveAsPDF( const fs::path& filepath )
{
  Finalize( filepath );
  const fs::path tmppath  = SaveTempPDF( filepath );
  const fs::path tmp2path = fs::path( tmppath ).replace_extension( ".tmp.pdf" );
  const fs::path dimpath  = fs::path( tmppath ).replace_extension( ".txt" );

  if( filepath.extension() != ".pdf" ){
    std::cerr << "Warning! File extension is not .pdf. "
              << "The file will be created but external programs "
              << "might not able to understand it" << std::endl;
  }

  // Fixing the rotation and margin issue
  const std::vector<std::string> gs_fixrotate = {
    "gs",
    "-dNOPAUSE",
    "-dQUIET",
    "-dBATCH",
    "-sstdout=/dev/null",// Supperssing error messages
    "-sDEVICE=pdfwrite",
    "-dCompatibilityLevel=1.4",
    "-dPDFSETTINGS=/screen",
    // Getting rid of the external transparent margin
    "-dUseCropBox",
    ( boost::format( "-sOutputFile=%s" )% tmp2path.string() ).str(),
    "-f", tmppath
  };

  if( !run_ghostscript( gs_fixrotate ) ){
    std::cerr << "Error in starting ghostscript processes, saving as "
              << "unaltered PDF file" << std::endl;
    fs::copy( tmppath, filepath );
    fs::remove( tmppath );
    return;
  }

  fs::remove( tmppath );

  // Command for getting the dimensions
  const std::vector<std::string> gs_getdim = {
    "gs",
    "-dNODISPLAY",
    "-dQUIET",
    ( boost::format( "-sstdout=%s" )%dimpath.string() ).str(),
    ( boost::format( "-sFileName=%s" )%tmp2path.string() ).str(),
    "-c",
    "FileName (r) file "
    "runpdfbegin 1 1 pdfpagecount {"
    "  pdfgetpage /MediaBox get {=print ( ) print"
    "} forall (\n) print} for quit"
  };

  if( !run_ghostscript( gs_getdim ) ){
    std::cerr << "Cannot get PDF file dimensions, saving a unscaled version "
              << "of the PDF " << std::endl;
    fs::copy( tmp2path, filepath );
    fs::remove( tmp2path );
    fs::remove( dimpath );
    return;
  }

  // Redesigning the dimensions
  std::fstream dimstream( dimpath, std::ios::in );
  float x1, y1, x2, y2;
  dimstream >> x1 >> y1 >> x2 >> y2;
  dimstream.close();
  fs::remove( dimpath );


  const float scale
    = std::max( (float)Width()/( x2-x1 ), (float)Height()/( y2-y1 ) );

  const unsigned newwidth  = std::ceil( ( x2-x1 ) * scale );
  const unsigned newheight = std::ceil( ( y2-y1 ) * scale );

  // Command for rescaling the PDF file.
  const std::vector<std::string> gs_fixscale = {
    "gs",
    "-dNOPAUSE",
    "-dQUIET",
    "-dBATCH",
    "-sDEVICE=pdfwrite",
    "-dCompatibilityLevel=1.4",
    ( boost::format( "-dDEVICEWIDTHPOINTS=%d" )%newwidth ).str(),
    ( boost::format( "-dDEVICEHEIGHTPOINTS=%d" )%newheight ).str(),
    "-dFIXEDMEDIA",
    "-dPDFFitPage",
    ( boost::format( "-sOutputFile=%s" ) % filepath.string() ).str(),
    "-f", tmp2path
  };

  if( !run_ghostscript( gs_fixscale ) ){
    std::cerr << "Error in running rescaling processes, saving as "
              << "unscaled PDF file" << std::endl;
    fs::copy( tmp2path, filepath );
    fs::remove( tmp2path );
    return;
  }

  //  Exit norminally.
  std::cout << "Saved Canvas to " << filepath << std::endl;
  fs::remove( tmp2path );
}

/**
 * @brief High resolution PNG file saving
 *
 * The file creates a temporary PDF file using the stock @ROOT functions, and
 * uses ghostscript to convert the PDF into a high resolution PNG file.
 *
 * @param filepath final path to store the PDF file.
 *                 (Parent directory would be automatically created if possible)
 * @param dpi      Required image quality
 */
void
Canvas::SaveAsPNG( const fs::path& filepath, const unsigned dpi )
{
  Finalize( filepath );

  if( filepath.extension() != ".png" ){
    std::cerr << "Warning! File extension is not .png. "
              << "The file will be created but external programs "
              << "might not able to understand it" << std::endl;
  }

  const fs::path tmppath = SaveTempPDF( filepath );

  const double scale = (double)len::ROOT_DPI / dpi;

  // Using ghostscript C api to avoid using system fuction
  const std::vector<std::string> gs_png = {
    "gs",
    "-dNOPAUSE",
    "-dQUIET",
    "-dBATCH",
    "-sstdout=/dev/null",// suppressing all error
    "-sDEVICE=pngalpha",
    ( boost::format( "-sOutputFile=%s" )%filepath.string() ).str(),
    ( boost::format( "-r%d" )%dpi ).str(),
    ( boost::format( "-dDEVICEWIDTHPOINTS=%d" )%( Width()*scale ) ).str(),
    ( boost::format( "-dDEVICEHEIGHTPOINTS=%d" )%( Height()*scale ) ).str(),
    "-dUseCropBox",// Trimming the PDF file
    "-f",
    tmppath };

  if( run_ghostscript( gs_png ) ){
    std::cout << "Saving Canvas to " << filepath << std::endl;
    fs::remove( tmppath );
  } else {
    std::cout << "Ghostscript conversion failed, saving via in-built root "
              << "function (Display maybe bad!)" << std::endl;
    fs::remove( tmppath );
    TCanvas::SaveAs( filepath.c_str() );
  }
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

  fs::copy( tempfile, filepath );
  fs::remove( tempfile );
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

// Running Ghostscript API
bool
run_ghostscript( const std::vector<std::string>& args )
{
  void* gs_inst        = nullptr;
  int gs_status        = 0;
  int gs_status1       = 0;
  int gs_argc          = args.size();
  const char** gs_argv = new const char*[args.size()];

  for( unsigned i = 0; i < args.size(); ++i ){
    gs_argv[i] = args.at( i ).c_str();
  }

  // Generating new ghostscript instance
  if( gsapi_new_instance( &gs_inst, nullptr ) < 0 ){
    delete[] gs_argv;
    return false;
  }

  // Calling GS function
  if( gsapi_set_arg_encoding( gs_inst, GS_ARG_ENCODING_UTF8 ) ){
    delete[] gs_argv;
    gsapi_delete_instance( gs_inst );
    return false;
  }

  gs_status = gsapi_init_with_args( gs_inst, gs_argc,
    const_cast<char**>( gs_argv ) );
  gs_status1 = gsapi_exit( gs_inst );
  if( gs_status == 0 || gs_status1 == gs_error_Quit ){
    gs_status = gs_status1;
  }

  delete[] gs_argv;
  gsapi_delete_instance( gs_inst );

  if( gs_status == 0 || gs_status == gs_error_Quit ){
    return true;
  } else {
    return false;
  }
}
