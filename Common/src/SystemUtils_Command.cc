/**
 * @file    SystemUtils_Command.cc
 * @brief   Implementation of command line interaction option
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */

#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"
#include "UserUtils/Common/interface/SystemUtils/Command.hpp"
#include "UserUtils/Common/interface/SystemUtils/Time.hpp"

#include <boost/format.hpp>

#include <array>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

using namespace std;

namespace usr {

/******************************************************************************/

unsigned
NumOfThreads()
{
  return std::thread::hardware_concurrency();
}

/*******************************************************************************
*   Getting command line ouputs
*******************************************************************************/
std::string
GetCMDOutput( const std::string& cmd )
{
  static const unsigned MAXBUFFER = 65536;
  static unsigned callcount       = 0;
  // Main reference
  // https://stackoverflow.com/questions/478898/how-to-execute-a-command-and-get-output-of-command-within-c-using-posix
  std::array<char, MAXBUFFER> buffer;
  std::string result;
  std::shared_ptr<FILE> pipe( popen( cmd.c_str(), "r" ), pclose );
  ++callcount;
  if( !pipe ){ throw std::runtime_error( "popen() failed!" );}

  while( !feof( pipe.get() ) ){
    if( fgets( buffer.data(), 128, pipe.get() ) != nullptr ){
      result += buffer.data();
    }
  }

  return result;
}

/******************************************************************************/

std::string
GetCMDSTDOutput( const std::string& cmd )
{
  return GetCMDOutput( cmd+" 2> /dev/null" );

}

/******************************************************************************/

std::string
GetCMDERROutput( const string& cmd )
{
  return GetCMDOutput( cmd+" > /dev/null  2>&1 " );
}

/*-----------------------------------------------------------------------------
 *  Getting process
   --------------------------------------------------------------------------*/
int
HasProcess( const std::string& x, const std::string& exclude )
{
  static const std::string common = "ps -U ${USER} | awk '{print $4}' ";
  static const std::string count  = " | wc --lines";
  const std::string grepcmd       = " | grep " + x;
  const std::string excludecmd    = exclude == "" ? "" :
                                    " | grep --invert-match " + exclude;

  const std::string ans = GetCMDOutput( common + grepcmd + excludecmd + count );
  return stoi( ans );
}

/******************************************************************************/

void
WaitProcess(
  const std::string& x,
  const std::string& exclude,
  const unsigned     sleeptime )
{
  while( 1 ){
    const int proc_count          = HasProcess( x, exclude );
    const std::string time_string = CurrentTime();

    if( proc_count == 0 ){ break; }

    cout << boost::format( "\r[%s|%s] %d instance(s) of [%s]..." )
      % GetEnv( "HOSTNAME" )
      % time_string
      % proc_count
      % x
         << flush;

    SleepMillSec( sleeptime );
  }

  cout << "All Done!" << endl;
}

}/* usr */
