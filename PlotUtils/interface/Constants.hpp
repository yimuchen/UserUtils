/**
 * @file    Units.hpp
 * @brief   Unit conversion helper functions
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */
#ifndef USERUTILS_PLOTUTILS_UNITS_HPP
#define USERUTILS_PLOTUTILS_UNITS_HPP

#include <cmath>
#include <string>

namespace usr  {

namespace plt  {

/*-----------------------------------------------------------------------------
 *  Length conversion functions
 *  Length in ROOT uses pixels with a fixed DPI.
 *  all conversion function thus returns a unsigned int.
   --------------------------------------------------------------------------*/
typedef unsigned length_t;// Abstracting the notation of Length in root
namespace len {

// Constants for conversion
extern const unsigned ROOTDPI;
extern const float INCHTOCM;

// inline conversion functions
inline length_t
inch( const float x ){ return std::round( ROOTDPI*x ); }

inline length_t
cm( const float x ){ return inch( x/INCHTOCM ); }

inline length_t
mm( const float x ){ return cm( x/10 ); }

// Common lengths for publication
// Extended variables ( requires function calculation )
// so declaring as parameterless functions
extern length_t a4paperwidth();
extern length_t a4paperheight();
extern length_t a4textwidth_default();
extern length_t a4textheight_default();

}/* size */

/*-----------------------------------------------------------------------------
 *  Font conversion functions
 *  Font is indexed using short in ROOT, writting a better enum interface
 *  Font size should be used in tandom to ensure proper font size.
 *  Absolute fontsize in ROOT uses pixels, which is equivalent to the
 *  conversiontional pt using the ROOT DPI settings.
 *  A helper functions helps with unifying fontsize in terms of the document
 *  font (em), floating points founding would be used.
   --------------------------------------------------------------------------*/
namespace font {

enum face : short// Only regular. Bold/italic modifiers should use latex.
{
  helvetica = 40,
  courier   = 80,
  times     = 130
};

// The +3 ensures that the font precision is absolute (in pixels)
inline short
fontface( face x ){ return ( x/10 )*10 + 3; }

// Using rounding to determine float -> unsigned conversion
inline unsigned
fontsize( const float x ){ return std::round( x ); }

// Fontsize conversion functions,
extern float normalsize;// This is a floating value!!
                        // All other fontsizes would be functions
inline unsigned
em( const float x, const float base = normalsize )
{
  return fontsize( base * x );
}

inline unsigned
pt( const float x ){ return fontsize( x ); }

// Latex build in sizes, dependent on normalsize, so declaring as function
extern unsigned tiny();
extern unsigned footnote();
extern unsigned small();
extern unsigned large();
extern unsigned Large();
extern unsigned LARGE();
extern unsigned huge();

// Returning the multipler of font size compared with normalsize
extern float mult_tiny();
extern float mult_footnote();
extern float mult_small();
extern float mult_large();
extern float mult_Large();
extern float mult_LARGE();
extern float mult_huge();

/*-----------------------------------------------------------------------------
 *  Text alignment enumeration, full documentation:
 *  https://root.cern.ch/doc/master/classTLatex.html#L10
   --------------------------------------------------------------------------*/
enum align
{
  bottom_left   = 11,
  bottom_center = 21,
  bottom_right  = 31,
  top_left      = 13,
  top_center    = 23,
  top_right     = 33
};

}/* font */


/*-----------------------------------------------------------------------------
 *  Unit strings and formatting string
 *  Ensuring title conventions for CMS publication
   --------------------------------------------------------------------------*/
namespace unit {
extern const std::string GeV;
extern const std::string GeVc;
extern const std::string GeVcc;
}/* unit */

namespace cap  {

extern const std::string prelim;
extern const std::string sim;

} /* cap  */

/*-----------------------------------------------------------------------------
 *  Styling options.
 *  Translating the rather convoluted fill style and marker style into
 *  more human readable formats.
   --------------------------------------------------------------------------*/
namespace sty  {

// Marker styles
inline float
mkrpixels( const int x ){ return x / 8;  }

// Fill styles
extern const short fillnone;
extern const short fillsolid;
extern const short filldotdense;
extern const short filldot;
extern const short filldotsparse;// all other fill styles are not defined.

// Fill style as defined in for TAttrFill.
// Additional function will be added to make the function more readable:
// fillhash(distmm(0.9),angle1(90),angle2(100))
const short fillhash( unsigned short distance,
                      unsigned short angle1,
                      unsigned short angle2 );
extern const short distmm( const float );
inline const short
distpx( const float x ){ return distmm( x/len::ROOTDPI );}
extern const short anglenone;
extern const short angle1( const float );
extern const short angle2( const float );

}/* sty  */

}/* plt  */

}/* usr  */

#endif/* end of include guard: USERUTILS_PLOTUTILS_CANVAS_HPP */
