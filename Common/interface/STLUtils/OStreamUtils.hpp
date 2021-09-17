/**
 * @file    OStreamUtils.hpp
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 * @brief  Simple classes for ostream manipulation
 */

#ifndef USERUTILS_COMMON_STLUTILS_OSTREAMUTILS_HPP
#define USERUTILS_COMMON_STLUTILS_OSTREAMUTILS_HPP

#include <iostream>

#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"
#else
#include "UserUtils/Common/STLUtils/StringUtils.hpp"
#endif

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
  friend std::ostream& operator<<( std::ostream&, const clearline& );
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
  virtual ~separator();
  friend std::ostream& operator<<( std::ostream&, const separator& );
};

/**
 * @brief Variadic interface for printf like stream output
 *
 * Details are implemented in the usr::fstr  function
 */
template<typename ... ARGS>
inline
std::ostream&
fout( ARGS... args )
{
  return std::cout << usr::fstr( std::forward<ARGS>( args )... ) << std::flush;
}


/**
 * @brief Simple function for generating logs.
 */
namespace log {

enum level: short
{
  INTERNAL = 0,
  DEBUG    = 1,
  INFO     = 2,
  WARNING  = 3,
  ERROR    = 4,
  FATAL    = 5
};

/**
 * @brief Setting the global log level.
 */
void SetLogLevel( const short i );

/**
 * @brief Printing a single line with log level.
 */
void PrintLog( const short        i,
               const std::string& line,
               const std::string& header = "" );

}/* log */

/** @} */

}/* usr */

namespace std {

/**
 * @brief Default interface for ostreaming a vector.
 *
 * Is required to be in namespace std for boost::program options to use this.
 */
template<typename T>
inline
std::ostream&
operator<<( std::ostream& os, const std::vector<T>& vec )
{
  os << "[";

  for( const auto& item : vec ){
    os << item << " ";
  }

  os << "]" << std::flush;

  return os;
}

}

#endif/* end of include guard: USERUTILS_COMMON_STLUTILS_OSTREAMUTILS_HPP */
