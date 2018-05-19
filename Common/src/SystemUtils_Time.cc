/**
 * @file    SystemUtils_Time.cc
 * @brief   Implementation of system time related functions
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */

#include <chrono>
#include <ctime>
#include <string>
#include <thread>

namespace usr {

/*-----------------------------------------------------------------------------
 *  Helper template fnuctions
   --------------------------------------------------------------------------*/
template<typename T>
inline size_t
CurrentTimeIn()// Ensures loosy conversion is handled
{
  return std::chrono::duration_cast<T>(
    std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

/*-----------------------------------------------------------------------------
 *  Time as 64bit unsigned integer
   --------------------------------------------------------------------------*/
size_t
CurrentTimeInNanSec()
{
  return CurrentTimeIn<std::chrono::nanoseconds>();
}

/*----------------------------------------------------------------------------*/

size_t
CurrentTimeInMuSec()
{
  return CurrentTimeIn<std::chrono::microseconds>();
}

/*----------------------------------------------------------------------------*/

size_t
CurrentTimeInMSec()
{
  return CurrentTimeIn<std::chrono::milliseconds>();
}

/*----------------------------------------------------------------------------*/

size_t
CurrentTimeInSec()
{
  return CurrentTimeIn<std::chrono::seconds>();
}

/*-----------------------------------------------------------------------------
 *  Displaying time in string format, still no C++ support, using old
 *  C style structure for now
   --------------------------------------------------------------------------*/
std::string
CurrentTime( const std::string& fmt )
{
  // Main reference for formatting
  // http://www.cplusplus.com/reference/ctime/strftime/
  static const unsigned MAXBUFFER = 80;
  time_t rawtime;
  struct tm* timeinfo;
  char buffer [MAXBUFFER];
  time( &rawtime );
  timeinfo = localtime( &rawtime );
  strftime( buffer, MAXBUFFER, fmt.c_str(), timeinfo );
  return buffer;
}


/*-----------------------------------------------------------------------------
 *  Sleeping functions
   --------------------------------------------------------------------------*/
template<typename T>
inline void
SleepFor( const unsigned x )
{
  std::this_thread::sleep_for( T( x ) );
}

/*----------------------------------------------------------------------------*/

void
SleepMicroSec( const unsigned x )
{
  SleepFor<std::chrono::microseconds>( x );
}

/*----------------------------------------------------------------------------*/

void
SleepMillSec( const unsigned x )
{
  SleepFor<std::chrono::milliseconds>( x );
}

/*----------------------------------------------------------------------------*/

void
SleepSec( const unsigned x )
{
  SleepFor<std::chrono::seconds>( x );
}


}/* usr */
