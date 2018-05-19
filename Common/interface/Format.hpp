/**
 * @file    Format.hpp
 * @brief   Better formatter for floating point objects
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 * @details
 *  "Better" includes a better interface for defining precision, and
 *  adding separation strings every n digits, handy for mass producing
 *  numbers in journal format.
 *  Also includes a more latex compatible scientific notation format
 *  (i.e. A.BCD\times 10^{+E}).
 *  Notice that the classes force clips the numbers after the decimal
 *  point to 27 digits.
 */
#ifndef USERUTILS_COMMON_FORMAT_HPP
#define USERUTILS_COMMON_FORMAT_HPP

#include <string>
#include <ostream>

namespace usr {

namespace fmt {

namespace base {

extern int precision_default;
extern std::string spacestr_default;
extern unsigned spacesep_default;
extern const unsigned max_precision;

class format
{
public:
  format() : _precision( precision_default ),
    _spacestr( spacestr_default ),
    _spacesep( spacesep_default ){}
  virtual ~format(){
  };

  // Flag setting functions
  inline format&
  precision( int x ){ _precision = x; return *this;}
  inline format&
  spacestr( const std::string& x ){ _spacestr = x; return *this; }
  inline format&
  spacesep( unsigned x ){ _spacesep = x; return *this; }

  // Setting duplication
  format& operator=( const format& ) = default ; // Member-wise copy
  inline format& dupsetting( const format& x ) { *this = x; return *this;}


  // conversion and output interfacing
  inline operator std::string(){
    return str();
  }

  // pure virtual function for returning string representation of something.
  virtual std::string str() const = 0;

protected:
  // Flag setting variables.
  int _precision;
  std::string _spacestr;
  unsigned _spacesep;
};

// Default interfacing
inline std::ostream&
operator<<( std::ostream& os, const format& x )
{ os << x.str(); return os; }


/*-----------------------------------------------------------------------------
 *  decimal representation, in the that the precision is set to
 *  negative, the absolute value of the given precision will be used
 *  with the exception that the trailing zeros after the decimal point will be
 *  stripped off.
   --------------------------------------------------------------------------*/
class decimal : public format
{
public:
  decimal(
    const double input,
    const int    p = -precision_default ) :
    _input( input )
  {
    precision( p );
  }

  virtual
  ~decimal (){}

  virtual std::string str() const;// Explicit function call

private:
  double _input;
};

/*-----------------------------------------------------------------------------
 *  Scientific represenation of a double. Precision must be explicitly set.
   --------------------------------------------------------------------------*/
class scientific : public format
{
public:
  scientific( const double input, const unsigned p );
  virtual
  ~scientific  (){}

  virtual std::string str() const;

private:
  double _mant;
  int _exp;
};

} /* base */
} /* fmt */


}/* usr */

#endif/* end of include guard: USERUTILS_COMMON_STLUTILS_FORMAT_HPP */
