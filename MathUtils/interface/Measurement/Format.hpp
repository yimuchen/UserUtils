/**
 * @file    MeasurementFormat.hpp
 * @brief   Classes to assist in the creation of a string representation of
 *          of a double or a Measurement object. Including fancier control
 *          options including additional space separation string between
 *          digits and such.
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */

/*-----------------------------------------------------------------------------
 *  Latex style string format for representing a measurement with uncertainty
 *  Both will be in the form of:
 *        {central_value}^{+upper_error}_{-lower_error}
 *  With the Scientific option moving a common exponent factor outside like
 *        {central_value}^{+upper_error}_{-lower_error} \times 10^{exp}
 *  if exp != 0,
 *  In the event that the upper_error and lower_error strings are identical
 *  (NOT the doubles that are identical!), the notation of
 *      {central_value}\pm{error}
 *  and
 *      ({central_value}\pm{error})\times 10^{exp}
 *  would be used
 *
 *  The precision represents the number of digits to display for behind the
 *  decimal point for the central value. And number of precision of the error
 *   will be one greater than that of the central value
 *
 *   If the precision is set to negative, (only possible for floating
 *   point notation) all of the digits (up to 8 with trailing zeros stripped)
 *   will be used.
 *
 *   This will use the results from the the base formats bound in the 'Common'
 *   subpackage. To ensure a nicer interface, the classes implemented here
 *   would have case handling to take regular doubles as usual inputs.
   --------------------------------------------------------------------------*/
#ifndef USERUTILS_MATHUTILS_MEASUREMENT_FORMAT_HPP
#define USERUTILS_MATHUTILS_MEASUREMENT_FORMAT_HPP

#include "UserUtils/Common/interface/Format.hpp"
#include "UserUtils/Common/interface/BoostUtils/PTreeUtils.hpp"
#include "UserUtils/MathUtils/interface/Measurement/Measurement.hpp"
#include <string>

namespace usr {

namespace fmt {

/*-----------------------------------------------------------------------------
 *  decimal representation, in the that the precision is set to
 *  negative, the absolute value of the given precision will be used
 *  with the exception that the trailing zeros after the decimal point will be
 *  stripped off.
   --------------------------------------------------------------------------*/
class decimal : public base::format
{
public:
  decimal(
    const double input,
    const int    precision = -base::precision_default );
  decimal(
    const Measurement& input,
    const int          precision = -base::precision_default );

  virtual
  ~decimal (){}

  inline decimal& precision( const int x );// redefining

  virtual std::string str() const;// Explicit function call

private:
  double _central;
  double _upper;
  double _lower;

  void SetPrecision();
};


/*-----------------------------------------------------------------------------
 *  Class for scientific notation representation
 *  If precision is set to nagative for a measurement, the precision would be
 *  automatically determined such that the larger uncertainty displays
 *  at least two significant digits.
   --------------------------------------------------------------------------*/
class scientific : public base::format
{
public:
  scientific(
    const double   input,
    const unsigned precision );
  scientific(
    const Measurement& input,
    const int          precision = -1 ); // default to auto for
  virtual
  ~scientific  (){}

  inline scientific& precision( const int x );// redefining

  std::string str() const;// Explicit function call

private:
  double _central;
  double _upper;
  double _lower;
  unsigned _exp;

  // Private helper functions
  void SetExponent();
  void SetPrecision();
};

}/* fmt */


/*-----------------------------------------------------------------------------
 *  Additional IO formatting options
   --------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 *  Template specialization for the Common/PTreeUtils function
 *  reading a list of doubles with 1-3 parameters as a Measurement
 *  class.
   --------------------------------------------------------------------------*/
template<>
Measurement GetSingle<Measurement>(
  const boost::property_tree::pt::ptree&,
  const std::string&
  );

}/* usr */

#endif/* end of include guard: USERUTILS_MATHUTILS_PARAMETERFORMAT_HPP */
