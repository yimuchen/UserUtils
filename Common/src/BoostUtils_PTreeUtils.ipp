/*******************************************************************************
*
*  Filename    : BoostUtils_PTreeUtils.ipp
*  Description : Implementation of variadic interfaces
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*   Additional comments
*
*******************************************************************************/
#ifdef USERUTILS_COMMON_BOOSTUTILS_PTREEUTILS_HPP

#include "UserUtils/Common/interface/STLUtils/VectorUtils.hpp"
#include <iostream>

namespace usr {

/*----------------------------------------------------------------------------*/

template<typename T>
extern T
GetSingle( const pt::ptree& tree, const std::string& query )
{
  return tree.get<T>( query );
}

/*----------------------------------------------------------------------------*/

template<typename T>
extern std::vector<T>
GetList( const pt::ptree& tree, const std::string& query )
{
  std::vector<T> ans;

  // Taken from
  // http://stackoverflow.com/questions/23481262/
  // using-boost-property-tree-to-read-int-array
  for( auto& v : tree.get_child( query ) ){
    if( !v.first.empty() ){
      std::cerr << "Warning! Skipping over illegal format at branch: (" << query
                << ")  with index value: (" << v.first.data() << ")" << std::endl;
      continue;
    }
    ans.push_back( v.second.get_value<T>() );
  }

  return ans;
}

/*-----------------------------------------------------------------------------
 *  Variadic interfaces
   --------------------------------------------------------------------------*/
template<typename ... TS>
extern pt::ptree
FromJsonFiles( const std::string& x , TS ... args )
{
  return FromJsonFiles( MakeVector<std::string>(x, args...) );
}

/*----------------------------------------------------------------------------*/

template<typename ... TS>
extern std::string
MakeQueryString( const std::string& x, TS ... args )
{
  return usr::MakeQueryString( usr::MakeVector<std::string>( x, args ... ) );
}

/*----------------------------------------------------------------------------*/

template<typename ... TS>
extern bool
CheckQuery( const pt::ptree& tree, const std::string& x, TS ... args )
{
  return usr::CheckQuery( tree, usr::MakeQueryString( x, args ... ) );
}

/*----------------------------------------------------------------------------*/

template<typename T, typename ... TS>
extern T
GetSingle( const pt::ptree& tree, const std::string& x, TS ... args )
{
  return usr::GetSingle<T>( tree, usr::MakeQueryString( x, args ... ) );
}

/*----------------------------------------------------------------------------*/

template<typename T, typename ... TS>
extern std::vector<T>
GetList( const pt::ptree& tree, const std::string& x, TS ... args )
{
  return usr::GetList<T>( tree, usr::MakeQueryString( x, args ... ) );
}

/*----------------------------------------------------------------------------*/

template<typename ... TS>
extern pt::ptree
GetSubTree( const pt::ptree& tree, const std::string& x, TS ... args )
{
  return usr::GetSubTree( tree, usr::MakeQueryString( x, args ... ) );
}
/*----------------------------------------------------------------------------*/

}/* usr */

#endif/* end of include guard: USERUTILS_COMMON_BOOSTUTILS_PTREEUTILS_HPP */
