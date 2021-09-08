/**
 * @file    STLUtils_OStreamUtils.cc
 * @brief   ostream interaction class implementation.
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */

#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/STLUtils/OStreamUtils.hpp"
#else
#include "UserUtils/Common/STLUtils/OStreamUtils.hpp"
#endif

#include "TError.h"
#include <ostream>

namespace usr {

/**
 * @details currently the clear line implementation consists of printing
 *          multiple backspace and whitespaces. Maybe not the best?
 */
std::ostream&
operator<<( std::ostream& os, const clearline& )
{
  for( size_t i = 0; i < 255; ++i ){
    os << "\b \b" << std::flush;
  }

  return os;
}

separator::separator( const char x, const size_t m ) :
  token( x ),
  n( m ){}

separator::~separator(){}

/**
 * @brief Implementation is essentially what it says on the tin
 *
 * duplicating the character multiple times.
 */
std::ostream&
operator<<( std::ostream& os, const separator& x )
{
  for( size_t i = 0; i < x.n; ++i ){
    os << x.token << std::flush;
  }

  return os;
}


/**
 * @brief Very simple function for generating the print level control.
 *
 * For analysis function, one typically wants to have a print levels that can be
 * accessed during development and disabled during production. Here we provide a
 * very simple facility to generate such outputs:
 *
 * A global log level object is created, which can bet set using the provided
 * SetLogLevel function. For every log that occurs in the analysis function, the
 * user defines the log level of that message, if the log will only be printed if
 * the message log level is greater than that of the global log level variable.
 * Internal function defined in the UserUtils package should refine from using
 * this function, as these should be well defined functions with minimal output.
 * Internal functions should only use the INTERNAL output level.
 */
namespace log
{

short __global_log_level = WARNING;

/**
 * @details
 *
 * Here we are providing the global log level.
 */
void
SetLogLevel( const short new_level )
{
  __global_log_level = new_level;
}

/**
 * @details
 * The output will be passed to STDERR such that logs can be stored seperately of
 * nominal analysis output for simpler debugging.
 */
void
PrintLog( const short i, const std::string& line, const std::string& header )
{
  if( i >= __global_log_level ){
    if( header.size() > 0 ){
      std::cerr << header << " " << line << std::endl;
    } else {
      std::cerr << header << " " << line << std::endl;
    }
  }
}

/**
 * @brief Dummy function for forcing a global setting of the ROOT output level.
 */
static inline short
__dummy_set_root_level()
{
  gErrorIgnoreLevel = kWarning;
  return 0;
}
short __dummy_int = __dummy_set_root_level();

}

}/* usr */
