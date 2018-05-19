/**
 * @file    Filesystem.hpp
 * @brief   Common functions for filesystem enhance ments
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 * @details Detailed descriptions
 */
#ifndef USERUTILS_COMMON_STLUTILS_FILESYSTEM_HPP
#define USERUTILS_COMMON_STLUTILS_FILESYSTEM_HPP

#include <string>
#include <vector>
#include <experimental/filesystem>


namespace usr {

namespace fs=std::experimental::filesystem;

/*-----------------------------------------------------------------------------
 *  Constant global variables (Implemented as parameterless functions)
 *  and commonly used paths
   --------------------------------------------------------------------------*/
extern fs::path cmssw_src();
extern fs::path pkgpath( const std::string& );
extern fs::path subpkgpath( const std::string&, const std::string& );
extern fs::path datapath( const std::string&, const std::string& );
extern fs::path resultpath( const std::string&, const std::string& );

/*-----------------------------------------------------------------------------
 *  Globbing functions
   --------------------------------------------------------------------------*/
extern std::vector<fs::path> GlobLocal( const std::string& );

/*-----------------------------------------------------------------------------
 *  File checking functions
 *  MakeParent -- returns false if not all parents directories could be
 *  created
   --------------------------------------------------------------------------*/
extern bool MakeParent( const fs::path& );

}

#endif /* end of include guard: USERUTILS_COMMON_STLUTILS_FILESYSTEM_HPP */
