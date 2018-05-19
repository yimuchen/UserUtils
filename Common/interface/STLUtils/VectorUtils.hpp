/**
 * @file    VectorUtils.hpp
 * @brief   std::vector Enhancement functions
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */

#ifndef USERUTILS_COMMON_STLUTILS_VECTORUTILS_HPP
#define USERUTILS_COMMON_STLUTILS_VECTORUTILS_HPP
#include <vector>

namespace usr {

/**
 * Removing duplicates from vector while maintaining vector order,
 * regardless if the vector is sorted or not.
 * @param myvec Input vector will not be changed
 */
template<typename T>
std::vector<T> RemoveDuplicate( const std::vector<T>& dupvec );

/**
 * Constructing a vector of same-typed inputs of arbitrary number of function
 * inputs
 * @param  first First object
 * @param  arg   Variadic template interface
 * @return       return the constructed vector object
 */
template<typename OBJTYPE, typename ARGTYPE, typename ... VARTYPE>
std::vector<OBJTYPE> MakeVector( ARGTYPE first, VARTYPE ... arg );

/**
 * Constructing a vector of same-typed inputs, required as a starting point for
 * the recursive variadic template parsing.
 * @param  first Single object arguments
 * @return       Single object vector
 */
template<typename OBJTYPE, typename ARGTYPE>
std::vector<OBJTYPE> MakeVector( ARGTYPE first );

/*-----------------------------------------------------------------------------
 *  STL full range shorthands
   --------------------------------------------------------------------------*/

// Removing elements of vector if value is equal to x
template<typename OBJTYPE>
void ClearValue( std::vector<OBJTYPE>& vec, const OBJTYPE& x );

// Find by Value 
template<typename OBJTYPE>
bool FindValue( std::vector<OBJTYPE>& vec, const OBJTYPE& x );


} /* usr */

#include "UserUtils/Common/src/STLUtils_VectorUtils.ipp"

#endif /* end of include guard: USERUTILS_COMMON_VECTORUTILS */
