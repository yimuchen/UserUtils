/**
 * @file    Maths.hpp
 * @brief   Simple math functions for concrete c++ types
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */

namespace usr {

/*-----------------------------------------------------------------------------
 *  Simple sgn function
   --------------------------------------------------------------------------*/
template<typename T>
inline int
sgn( T val ){ return ( T( 0 ) < val ) - ( val < T( 0 ) ); }

/*-----------------------------------------------------------------------------
 *  Exponential and Related functions
   --------------------------------------------------------------------------*/
extern int    ReduceToMant( double& x );
extern int    GetExponent( double x );
extern double IntPower( double base, int exp );

}/* usr */
