#ifdef CMSSW_GIT_HASH
#include "UserUtils/MathUtils/interface/Measurement/Measurement.hpp"
#include "UserUtils/MathUtils/interface/StatisticsUtil.hpp"
#else
#include "UserUtils/MathUtils/Measurement/Measurement.hpp"
#include "UserUtils/MathUtils/StatisticsUtil.hpp"
#endif

// All namespace will be flattened and re-established in the python modules

// Redefining measurement type
typedef usr::Measurement Measurement;

// Flattening the namespace
inline double
NormalCDF( double x )
{ return usr::stat::NormalCDF( x ); }

inline double
GetConfidenceLevel( double x )
{ return usr::stat::GetConfidenceLevel( x ); }

inline double
GetSigmaInterval( double x )
{ return usr::stat::GetSigmaInterval( x ); }
