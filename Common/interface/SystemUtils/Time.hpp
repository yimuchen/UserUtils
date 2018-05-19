/**
 * @file    Time.hpp
 * @brief   Simpler interface for std::chrono with specific use cases
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */
#ifndef USERUTILS_COMMON_SYSTEMUTILS_TIMP_HPP
#define USERUTILS_COMMON_SYSTEMUTILS_TIMP_HPP

#include <string>

namespace usr {

/*-----------------------------------------------------------------------------
 *  Getting time as a 64-bit number. Nice for unrepeating simulation
   --------------------------------------------------------------------------*/
extern size_t CurrentTimeInNanSec();
extern size_t CurrentTimeInMuSec();
extern size_t CurrentTimeInMSec();
extern size_t CurrentTimeInSec();

/*-----------------------------------------------------------------------------
 *  Displaying present time
 *  Meaning of format described here:
 *  http://www.cplusplus.com/reference/ctime/strftime/
   --------------------------------------------------------------------------*/
extern std::string CurrentTime( const std::string& fmt="%Y-%m-%d %X");

/*-----------------------------------------------------------------------------
 *  Sleeping a thread for various seconds
   --------------------------------------------------------------------------*/
extern void SleepMicroSec( const unsigned );
extern void SleepMillSec( const unsigned );
extern void SleepSec( const unsigned );


} /* usr */

#endif/* end of include guard: USERUTILS_COMMON_SYSTEMUTILS_TIMP_HPP */
