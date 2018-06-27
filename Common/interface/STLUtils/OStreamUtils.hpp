/**
 * @file    OStreamUtils.hpp
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 * @brief  Simple classes for ostream manipulation
 */

#ifndef USERUTILS_COMMON_STLUTILS_OSTREAMUTILS_HPP
#define USERUTILS_COMMON_STLUTILS_OSTREAMUTILS_HPP

#include <iostream>

namespace usr {

/**
 * @defgroup ostreamutils ostream Helpers
 * @brief    classes for output stream manipulation.
 * @ingroup  Common
 * @details
 * Collections of classes with a operator<< with the std::ostream overloaded
 * to allow for nicer output manipulation.
 * @{
 */

/**
 * @brief simples for clearing the current line in the screen outputs.
 */
class clearline
{
public:
  clearline(){}
  virtual ~clearline(){}
  friend std ::ostream& operator<<( std::ostream&, const clearline& );
};

/**
 * @brief class for printing a ling of characters on the screen.
 */
class separator
{
private:
  char token;
  size_t n;

public:
  separator( const char x = '*', const size_t m = 50 );
  virtual
  ~separator();
  friend std ::ostream& operator<<( std::ostream&, const separator& );
};

/** @} */

}/* usr */

#endif/* end of include guard: USERUTILS_COMMON_STLUTILS_OSTREAMUTILS_HPP */
