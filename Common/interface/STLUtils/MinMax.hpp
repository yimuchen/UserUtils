/**
 * @file    MinMax.hpp
 * @brief   Lazy min, max implementation, as std::min/max requires same type
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 * @details Detailed descriptions
 */
#ifndef USERUTILS_COMMON_STLUTILS_MINMAX_HPP
#define USERUTILS_COMMON_STLUTILS_MINMAX_HPP

namespace usr {
template<typename A, typename B>
inline bool
min( A a, B b )
{ return a < b ? a : b; }

template<typename A, typename B>
inline bool
max( A a, B b )
{ return a > b ? a : b; }

}/* usr */


#endif/* end of include guard: USERUTILS_COMMON_STLUTILS_MINMAX_HPP  */
