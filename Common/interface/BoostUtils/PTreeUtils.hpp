/*******************************************************************************
*
*  Filename    : PTreeUtils.hpp
*  Description : Helper functions for PTree query and traversing
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef USERUTILS_COMMON_BOOSTUTILS_PTREEUTILS_HPP
#define USERUTILS_COMMON_BOOSTUTILS_PTREEUTILS_HPP

#include "UserUtils/Common/interface/STLUtils/VectorUtils.hpp"
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <vector>

namespace usr {

namespace pt = boost::property_tree;

/*-----------------------------------------------------------------------------
 *  Making ptree objects from a given file
   --------------------------------------------------------------------------*/
extern pt::ptree FromJsonFile( const std::string& );
extern pt::ptree FromJsonFiles( const std::vector<std::string>& );
//variadic interface
template<typename... TS>
extern pt::ptree FromJsonFiles( const std::string&, TS ... args);

/*-----------------------------------------------------------------------------
 *  Two way merging functions.
   --------------------------------------------------------------------------*/
extern pt::ptree MergeTrees( const pt::ptree& first, const pt::ptree& second );

/*-----------------------------------------------------------------------------
 *  Dumping the contents of a pt::ptree object
   --------------------------------------------------------------------------*/
extern void PrintPTree( const pt::ptree&, unsigned level = 0 );

/*-----------------------------------------------------------------------------
 *  Simpler General interface fuctions.
 *   * MakeQueryString : Contract vector of strings {a,b,c} to "a.b.c"
 *   * CheckQuery : Check if the tree has the the value at "<query>"
 *   * GetSingle  : Extract the value at "<query>"
 *   * GetList    : Getting the list at "<query>"
   --------------------------------------------------------------------------*/
extern std::string MakeQueryString( const std::vector<std::string>& );

extern bool CheckQuery( const pt::ptree&, const std::string& );

template<typename T>
extern T GetSingle( const pt::ptree&, const std::string& );

template<typename T>
extern std::vector<T> GetList( const pt::ptree&, const std::string& );

pt::ptree GetSubTree( const pt::ptree&, const std::string& );

// Variadic interfaces.
template<typename ... TS>
extern std::string MakeQueryString(
  const std::string& x, TS ... args );

template<typename ... TS>
extern bool CheckQuery( const pt::ptree&, const std::string&, TS ... args );

template<typename T, typename ... TS>
extern T GetSingle( const pt::ptree&, const std::string&, TS ... args );

template<typename T, typename ... TS>
extern std::vector<T> GetList( const pt::ptree&, const std::string&, TS ... args );

template<typename ... TS>
extern pt::ptree GetSubTree( const pt::ptree&, const std::string&, TS ... args );

}/* usr*/

#include "UserUtils/Common/src/BoostUtils_PTreeUtils.ipp"

#endif/* end of include guard: USERUTILS_COMMON_BOOSTUTILS_PTREEUTILS_HPP */
