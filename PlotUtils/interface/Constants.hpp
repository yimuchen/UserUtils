/**
 * @file    Constants.hpp
 * @brief   Unit conversion helper functions
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifndef USERUTILS_PLOTUTILS_UNITS_HPP
#define USERUTILS_PLOTUTILS_UNITS_HPP

#include <cmath>
#include <string>

namespace usr  {

namespace plt  {

/**
 * @defgroup PlotUtilsConvert PlotUtilsConvert
 * @ingroup PlotUtils
 * @brief ROOT plotting's magic number conversion functions and constants.
 * @{
 */

/** @brief Abstact length unit used in the plotting routines in root */
typedef unsigned length_t;

/**
 * @brief length converion functions and constants.
 */
namespace len {

/**
 * @addtogroup PlotUtilsConvert
 * @{
 */
extern const unsigned ROOT_DPI;

extern length_t inch( const float x );
extern length_t cm( const float x );
extern length_t mm( const float x );

extern length_t a4paperwidth();
extern length_t a4paperheight();
extern length_t a4textwidth_default();
extern length_t a4textheight_default();
/** @} */

}/* len */

/**
 * @brief Container for a font setting and size conversion.
 * @ingroup PlotUtilsConvert
 */
class font
{
public: // static and user enums.

  /** @brief enum interface for defined fonts. */
  enum face : short
  {
    helvetica = 40,
    courier   = 80,
    times     = 130
  };

  /** @brief enum interface for text alignment in @ROOT{TLatex} */
  enum align : short
  {
    bottom_left   = 11,
    bottom_center = 21,
    bottom_right  = 31,
    top_left      = 13,
    top_center    = 23,
    top_right     = 33
  };

  static float      default_basesize;
  static font::face default_face;
  static float      default_lineheight;

public:
  font( float basesize   = default_basesize,
        face  face       = default_face,
        float lineheight = default_lineheight );

  float size() const ;
  float lineheight() const;
  short fontface() const;
  float em( const float mult ) const ;

  float tiny() const ;
  float footnote() const ;
  float small() const ;
  float large() const ;
  float Large() const ;
  float LARGE() const ;
  float huge() const ;

protected:
  float      _basesize ;
  font::face _face;
  float      _lineheight;
};

/** @brief Strings for a unified presentation of units in plots */
namespace unit {
extern const std::string GeV;
extern const std::string GeVc;
extern const std::string GeVcc;
}/* unit */

/** @brief String for unifed presentation of common captions text in plots */
namespace cap  {
extern const std::string prelim;
extern const std::string sim;
}/* cap  */

/** @brief root style conversion functions  */
namespace sty  {

/**
 * @addtogroup PlotUtilsConvert
 * @{
 */

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
extern const short anglenone;
extern const short angle1( const float );
extern const short angle2( const float );

/** @} */

}/* sty  */

/** @} */

}/* plt  */

}/* usr  */

#endif/* end of include guard: USERUTILS_PLOTUTILS_CANVAS_HPP */
