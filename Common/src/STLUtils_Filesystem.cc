/**
 * @file    STLUtils_Filesystem.cc
 * @brief   Implementation of filesystem enhancement functions
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */
#include "UserUtils/Common/interface/STLUtils/Filesystem.hpp"
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"

#include <glob.h>
#include <iostream>

namespace usr  {

/*-----------------------------------------------------------------------------
 *  Global constants and common path implementation functions
   --------------------------------------------------------------------------*/
fs::path
cmssw_src(){ return GetEnv( "CMSSW_BASE" ) + "/src/";  }

/*----------------------------------------------------------------------------*/

fs::path
pkgpath( const std::string& x )
{
  return cmssw_src() / fs::path( x );
}

/*----------------------------------------------------------------------------*/

fs::path
subpkgpath( const std::string& x, const std::string& y )
{
  return pkgpath( x )/fs::path( y );
}

/*----------------------------------------------------------------------------*/

fs::path
datapath( const std::string& x, const std::string& y )
{
  return subpkgpath( x, y )/fs::path( "data" );
}

/*----------------------------------------------------------------------------*/

fs::path
resultpath( const std::string& x, const std::string& y )
{
  return subpkgpath( x, y )/fs::path( "result" );
}

/*-----------------------------------------------------------------------------
 *  Local Globing function
   --------------------------------------------------------------------------*/
std::vector<fs::path>
GlobLocal( const std::string& query )
{
  glob_t glob_result;
  glob( query.c_str(), GLOB_TILDE, NULL, &glob_result );

  std::vector<fs::path> ans(
    glob_result.gl_pathv,
    glob_result.gl_pathv + glob_result.gl_pathc
    );
  globfree( &glob_result );

  return ans;
}


/*-----------------------------------------------------------------------------
 *  File checking functions
 *  Note: this functions is explicitly noisy to allow for better
 *  tracking in case of bad input
   --------------------------------------------------------------------------*/
bool
MakeParent( const fs::path& filepath )
{
  const fs::path parent = fs::path( filepath ).parent_path();
  if( fs::is_directory( parent ) || parent.empty() ){
    return true;// Early exit if already exists
  }

  if( fs::create_directories( parent ) ){
    std::cout << "New directory [" << parent << "] created!"
              << std::endl;
    return true;
  } else {
    std::cout << "Failed making new directory [" << parent << "]!"
              <<  std::endl;
    return false;
  }
}

}/* usr  */
