/**
 * @file    Maths.cc
 * @brief   Simple concrete type math functions
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <vector>

namespace usr {

/**
 * @brief Reducing the double x to it's mantissa component in it's scientific
 *        notation, returns the exponent.
 */
int
ReduceToMant( double& x )
{
  int ans = 0;
  if( x == 0 ){ return 0; }

  while( fabs( x ) > 10 ){
    ans++;
    x /= 10;
  }

  while( fabs( x ) < 0.1 ){
    ans--;
    x *= 10;
  }

  return ans;
}

/**
 * @brief returning the exponent for the scientific notation of a double x
 */
int
GetExponent( double x )
{
  return ReduceToMant( x );
}

/**
 * @brief faster implementation of the power function with integers
 *
 * This actually has minimal uses in analysis, I just thought it was a nice
 * exercise for writting a [square-and-multiply algorithm](https://en.wikipedia.org/wiki/Exponentiation_by_squaring) :/
 */
double
IntPower( double base, int exp )
{
  if( exp < 0 ){ return IntPower( 1./base, -exp ); }
  double ans = 1;


  for( int x = 1; x; x <<= 1, base *= base ){
    if( x & exp ){ ans *= base; }
  }

  return ans;
}

/**
 * @brief Rounding up to the closest power of 2
 *
 * Using a nice algorithm by bit mashing.
 * https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
 */
unsigned
RoundUpToP2( unsigned x )
{
  x--;
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  x++;
  return x;
}

/**
 * @brief Generic 64-bit randomization
 *
 * We will be using the 64-bit
 * [LCG](https://en.wikipedia.org/wiki/Linear_congruential_generator) to
 * initialize a [xorshift* algorithm](https://en.wikipedia.org./wiki/Xorshift)
 * away from 0 Which is common when attempting to hash numerical values.
 *
 * - LCG parameters are taken from MMIX by Donald Knuth
 * - Xorshift parameters are taken from "A7" of the Numerical recipes (Press)
 */
static uint64_t
HashValue64( uint64_t x )
{
  x  = x * 6364136223846793005LL + 1442695040888963407LL;
  x ^= x >> 21;// a
  x ^= x << 37;// b
  x ^= x >> 4;// c
  return x;
}

/**
 * @brief Generic 32-bit randomization for
 *
 * We will be using the 32-bit
 * [LCG](https://en.wikipedia.org/wiki/Linear_congruential_generator) to
 * initialize a [xorshift* algorithm](https://en.wikipedia.org./wiki/Xorshift)
 * away from 0 Which is common when attempting to hash numerical values.
 *
 * - LCG parameters are taken from "Numerical recipes" (Press)
 * - Xorshift parameters are taken from Example of "Xorshift RNG" (Marsaglia)
 */
static uint32_t
HashValue32( uint32_t x )
{
  x  = x * 1664525 + 1013904223;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x >> 5;
  return x;
}



/**
 * @brief A 64-bit pseudo random number generated by a double.
 *
 * This is useful when attempting to hash parameter space to avoid excessive
 * re-calculation, or to generate random values based on the independent
 * variables to allow for unique smearing.
 */
uint64_t
HashValue64( double in )
{
  union bitconvert
  {
    uint64_t bits;
    double   input;
  };

  bitconvert conv;
  conv.input = in;
  uint64_t x = conv.bits;
  return HashValue64( x );
}

/**
 * @brief A 32-bit pseudo random number generated by a double
 *
 * This is useful when attempting to hash parameter space to avoid excessive
 * re-calculation, or to generate random values based on the independent
 * variables to allow for unique smearing. Notice that the algorithm generates
 * the 32 bit number by first truncating the double into a 32 bit float, so that
 * mathematically close value can be mapped to the same has value. The actually
 * randomization algorithm is based on the [xorshift
 * algorithm](https://en.wikipedia.org/wiki/Xorshift).
 */
uint32_t
HashValue32( const double in )
{
  union bitconvert
  {
    uint32_t bits;
    float    input;
  };

  const float trunc = in;
  bitconvert conv;
  conv.input = trunc;
  uint32_t x = conv.bits;

  return HashValue32( x );
}

/**
 * @brief Returning a joint hash value of two 32 bit hash values
 *
 * The algorithm is designed such that f(a,b) != f(b,a). Allowing for the
 * ordering of numerical values to matter. Using the 128 bits version of the
 * xorshift algorithm.
 */
uint64_t
Hash64Join( uint64_t x, uint64_t y )
{
  static const uint64_t mask32 = 0xFFFFFFFF;

  uint64_t t = y & mask32;// Last 32 bit of state
  uint64_t s = x >> 32;// First 32 bits of state

  // Bit rotation
  y = ( y >> 32 ) | ( x << 32 );
  x = ( x >> 32 ) | ( s << 32 );

  t ^= t << 11;
  t ^= t >> 8;
  s  = ( t ^s ^ ( s >> 19 ) ) & mask32;

  // Replacing first 32 bits
  x = ( s << 32 ) | ( x & mask32 );

  return x ^ y;
}


/**
 * @brief Returning a joint hash value of two 32 bit hash values
 *
 * The algorithm is designed such that f(a,b) != f(b,a). Allowing for the
 * ordering of numerical values to matter.
 */
uint32_t
Hash32Join( const uint32_t x, const uint32_t y )
{
  const uint64_t xx   = x;
  const uint64_t yy   = y;
  const uint64_t hash = HashValue64( xx << 32 | yy );
  return ( hash >> 32 ) ^ hash;
}

uint64_t
OrderedHash64( const std::vector<double>& vec )
{
  uint64_t ans = HashValue64( vec.size() );

  for( const double val : vec ){
    ans = Hash64Join( ans, usr::HashValue64( val ) );
  }

  return ans;
}

uint32_t
OrderedHash32( const std::vector<double>& vec )
{
  uint32_t ans = HashValue32( (double)vec.size() );

  for( const double val : vec ){
    ans = Hash32Join( ans, usr::HashValue32( val ) );
  }

  return ans;
}


/**
 * @brief Calculating the mean of a double vector
 *
 * Simple vector arithmetics using the std::algorithm methods
 */
double
Mean( const std::vector<double>& vec )
{
  return std::accumulate( vec.begin(), vec.end(), 0.0 ) / (double)vec.size();
}

/**
 * @brief Calculating the standard deviation of a double vector
 *
 * Simple vector arithmetics using the std::algorithm methods
 */
double
StdDev( const std::vector<double>& vec )
{
  const double mean = Mean( vec );
  std::vector<double> diff( vec.size() );

  std::transform( vec.begin(), vec.end(), diff.begin(),
    [mean]( const double x ){
      return x-mean;
    } );
  const double sq = std::inner_product( diff.begin(), diff.end(), diff.begin(), 0.0 );

  return std::sqrt( sq/vec.size() );
}

/**
 * @brief Rounding to closest multiple of target number
 *
 * The return value will be a multiple of the target number that is closest to
 * the original input.
 */
double
RoundTo( const double x, const double target )
{
  return target * std::round( x / target );
}

/**
 * @brief Rounding up to the closest multiple of target number
 *
 * The return value will be the minimal multiple of the target number that is
 * greater than the input x.
 */
double
RoundUp( const double x, const double target )
{
  return target * std::ceil( x/target );
}

/**
 * @brief Rounding down to the closest multiple of the target number.
 *
 * The return value will be the maximum multiple of the target number that is
 * smaller than the input x.
 */
double
RoundDown( const double x, const double target )
{
  return target * std::floor( x/target );
}

}/* usr */
