/**
 * @file    STLUtils_VectorUtils.ipp
 * @brief   Implemenation of Vector utility functions
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */
#ifdef USERUTILS_COMMON_STLUTILS_VECTORUTILS_HPP

#include <algorithm>
#include <set>

namespace usr {

/*-----------------------------------------------------------------------------
   Duplicate removal
   Taken from https://stackoverflow.com/questions/12200486/how-to-remove-duplicates-from-unsorted-stdvector-while-keeping-the-original-or
  ----------------------------------------------------------------------------*/
template<typename T>
std::vector<T>
RemoveDuplicate( const std::vector<T>& dupvec )
{
  std::set<T> unique_set;
  std::vector<T> ans;

  for( const auto& item : dupvec ){
    if( unique_set.insert( item ).second ){
      ans.push_back( item );
    }
  }

  return ans;
}


/*-----------------------------------------------------------------------------
   Variadic functions
   ---------------------------------------------------------------------------*/
template<typename OBJTYPE, typename ARGTYPE>
std::vector<OBJTYPE>
MakeVector( ARGTYPE first )
{
  OBJTYPE x( first );
  return std::vector<OBJTYPE>( 1, x );
}

/*----------------------------------------------------------------------------*/

template<typename OBJTYPE, typename ARGTYPE, typename ... VARTYPE>
std::vector<OBJTYPE>
MakeVector( ARGTYPE first, VARTYPE ... arg )
{
  std::vector<OBJTYPE> ans = MakeVector<OBJTYPE>( arg ... );
  ans.insert( ans.begin(), OBJTYPE( first ) );
  return ans;
}

/*-----------------------------------------------------------------------------
 *  Vector Full range STL recipe
   --------------------------------------------------------------------------*/
template<typename OBJTYPE>
void
ClearValue( std::vector<OBJTYPE>& vec, const OBJTYPE& x )
{
  vec.erase( std::remove( vec.begin(), vec.end(), x ), vec.end() );
}

template<typename OBJTYPE>
bool
FindValue( std::vector<OBJTYPE>& vec, const OBJTYPE& x )
{
  return (std::find( vec.begin(), vec.end(), x ) != vec.end());
}


}/* usr */



#endif/* end of include guard: USERUTILS_COMMON_VARIADIC_IPP */
