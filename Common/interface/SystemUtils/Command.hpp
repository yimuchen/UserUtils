/**
 * @file    Command.hpp
 * @brief   Functions for command-line interaction
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */

#ifndef USERUTILS_COMMON_SYSTEMUTILS_COMMAND_HPP
#define USERUTILS_COMMON_SYSTEMUTILS_COMMAND_HPP

#include <string>

namespace usr {

extern unsigned NumOfThreads();

/*-----------------------------------------------------------------------------
 *  GetCMDOutput
 *  Storing the stdout/stderr of a shell command <x> to a return string
 *  Notice that non of the output will be displayed on scree, so the user
 *  is responsible to ensure that the input command <x> will terminate without
 *  user intervention
   --------------------------------------------------------------------------*/
extern std::string GetCMDSTDOutput( const std::string& x );
extern std::string GetCMDERROutput( const std::string& x );
extern std::string GetCMDOutput( const std::string& );


/*-----------------------------------------------------------------------------
*   * HasProcess( x, exclude )
*   Returns the number of processes containing the string <x>, wile excluding
*   processes containing string <exclude>. If <exclude> is left blank, no
*   exclusion will be processed.
   --------------------------------------------------------------------------*/
extern int HasProcess( const std::string& x, const std::string& exclude = "" );

/*-----------------------------------------------------------------------------
 *  WaitProcess
 *  Sleep the thread until the return value of HasProcess reaches zero.
 *  During this time, the system-time information will be refreshed on the
 *  screen for easier monitoring.
   --------------------------------------------------------------------------*/
extern void WaitProcess(
  const std::string& process_tag,
  const std::string& exclude = "",
  const unsigned     waitmills = 100
  );


} /* usr */


#endif /* end of include guard: USERUTILS_COMMON_SYSTEMUTILS_COMMAND_HPP */
