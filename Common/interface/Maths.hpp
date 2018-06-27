/**
 * @file    Maths.hpp
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 * @brief   Simple concrete type maths functions
 */

namespace usr {

/**
 * @defgroup STLMaths Math with concrete types.
 * @brief    Calculations with concrete types that are not avaialbe in STL
 * @ingroup  Common
 * @{
 */

/**
 * @brief the [sign function](https://en.wikipedia.org/wiki/Sign_function) for
 *        any 0 comparable type.
 *
 * For any type that can be constructed and compared with 0, returns 1 if
 * greater than 0, -1 if smaller than 0, 0 if exactly zero.
 */
template<typename TYPE>
inline int
sgn( TYPE val )
{
  return ( TYPE( 0 ) < val ) - ( val < TYPE( 0 ) );
}

/*-----------------------------------------------------------------------------
 *  Exponential and Related functions
   --------------------------------------------------------------------------*/
extern int    ReduceToMant( double& x );
extern int    GetExponent( double x );
extern double IntPower( double base, int exp );

/** @} */

}/* usr */
