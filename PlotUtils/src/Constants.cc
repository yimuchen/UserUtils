/**
 * @file    Units.hpp
 * @brief   Defining the constants and extended constants
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */

#include "UserUtils/Common/interface/STLUtils/MinMax.hpp"
#include "UserUtils/PlotUtils/interface/Constants.hpp"
#include <vector>

namespace usr  {

namespace plt  {

namespace len {
/*-----------------------------------------------------------------------------
 *  Length constants
   --------------------------------------------------------------------------*/
const unsigned ROOTDPI = 72;
const float INCHTOCM   = 2.54;

unsigned
a4paperwidth(){ return mm( 210 ); }
unsigned
a4paperheight(){ return mm( 297 ); }
unsigned
a4textwidth_default(){ return mm( 160 ); }// Taken from cms-tdr.cls
unsigned
a4textheight_default(){ return mm( 235 ); }

}/* size */

namespace font {
/*-----------------------------------------------------------------------------
 *  Fontsize conversion
 *  cmstdr uses the 'article' and 'report' document classes
 *  predefined conversion could be found here
 *  https://en.wikibooks.org/wiki/LaTeX/Fonts#Built-in_sizes
   --------------------------------------------------------------------------*/
float normalsize = 11;// cmstdr uses 11 base font

// Static helpers for size conversion
enum e_size : short
{
  e_tiny,
  e_footnote,
  e_small,
  e_large,
  e_llarge,
  e_lllarge,
  e_huge
};

static unsigned
getcol( const float x = normalsize )
{
  return fontsize( x ) == 10 ? 0 :
         fontsize( x ) == 11 ? 1 :
         fontsize( x ) == 12 ? 2 :
         3;
}

static const std::vector<std::vector<float> > scaletable = {
  // 10      11          12             others
  {5/10,     6/11.,      6/12.,         0.5            },// tiny
  {8/10,     9/11.,      10/12.,        0.75           },// footnote
  {9/10,     10/11.,     10.95/12.,     0.9            },// small
  {12/10,    12/11.,     14.4/12.,      1.2            },// large
  {14.4/10,  14.4/11.,   17.28/12.,     14.4           },// Large
  {17.28/10, 17.28/11.,  20.74/12.,     1.72           },// LARGE
  {20.7/10,  20.7/11.,   24.8/12.,      2              }// huge
};

unsigned
tiny(){ return fontsize( normalsize*scaletable[e_tiny][getcol()] ); }

unsigned
footnote(){ return fontsize( normalsize*scaletable[e_footnote][getcol()] );}

unsigned
small(){ return fontsize( normalsize*scaletable[e_small][getcol()] ); }

unsigned
large(){ return fontsize( normalsize*scaletable[e_large][getcol()] );}

unsigned
Large(){ return fontsize( normalsize*scaletable[e_llarge][getcol()] );}

unsigned
LARGE(){ return fontsize( normalsize*scaletable[e_lllarge][getcol()] );}

unsigned
huge(){ return fontsize( normalsize*scaletable[e_huge][getcol()] );}

/*----------------------------------------------------------------------------*/

float
mult_tiny(){ return scaletable[e_tiny][getcol()]; }

float
mult_footnote(){ return scaletable[e_footnote][getcol()];}

float
mult_small(){ return scaletable[e_small][getcol()]; }

float
mult_large(){ return scaletable[e_large][getcol()];}

float
mult_Large(){ return scaletable[e_llarge][getcol()];}

float
mult_LARGE(){ return scaletable[e_lllarge][getcol()];}

float
mult_huge(){ return scaletable[e_huge][getcol()];}

}/* font */

/*-----------------------------------------------------------------------------
 *  Constant strings for common units and latex tags
   --------------------------------------------------------------------------*/
namespace unit  {
const std::string GeV   = "GeV";
const std::string GeVc  = "GeV/#it{c}";
const std::string GeVcc = "GeV/#it{c}^{2}";
}/* unit  */

namespace cap  {

const std::string prelim = "Preliminary";
const std::string sim    = "Simulation";

}/* cap  */

/*-----------------------------------------------------------------------------
 *  Common styling options
   --------------------------------------------------------------------------*/
namespace sty {
const short fillnone      = 0;
const short fillsolid     = 1001;
const short filldotdense  = 3001;
const short filldot       = 3002;
const short filldotsparse = 3003;

const short
fillhash( unsigned short distance,
          unsigned short angle1,
          unsigned short angle2 )
{
  distance = usr::max( distance, 1 );
  distance = usr::min( distance, 9 );
  angle1   = usr::min( angle1,   9 );
  angle2   = usr::min( angle2,   9 );
  return 3000 + 100*distance+10*angle1+1*angle2;
}


const short anglenone = 5;

const short
distmm( const float x )
{
  // TAttrFill only defines 1 = 0.5 mm and 9 = 6mm
  // assuming linear relation, rounding to the closest value.
  return x < 0.84375 ? 1 :
         x < 1.53125 ? 2 :
         x < 2.21875 ? 3 :
         x < 2.90625 ? 4 :
         x < 3.59375 ? 5 :
         x < 4.28125 ? 6 :
         x < 4.96875 ? 7 :
         x < 5.65625 ? 8 :
         9;
}

const short
angle1( const float x )
{
  // Rounding to the nearest defined value ( in increaments to 10 degrees )
  return x < 0    ? anglenone :
         x < 5    ? 0 :
         x < 15   ? 1 :
         x < 25   ? 2 :
         x < 37.5 ? 3 :
         x < 52.5 ? 4 :
         x < 65   ? 6 :
         x < 75   ? 7 :
         x < 85   ? 8 :
         x <= 90  ? 9 :
         anglenone;
}

const short
angle2( const float x )
{
  return x > 180   ? anglenone :
         x > 175   ? 0 :
         x > 165   ? 1 :
         x > 155   ? 2 :
         x > 142.5 ? 3 :
         x > 127.5 ? 4 :
         x > 115   ? 6 :
         x > 105   ? 7 :
         x > 95    ? 8 :
         x >= 90   ? 9 :
         anglenone;

}

}/* sty */

}/* plt  */

}/* usr  */
