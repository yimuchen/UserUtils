/*******************************************************************************
*
*  Filename    : ArgumentExtender.ipp
*  Description : Implementation of template functions for options access
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef USERUTILS_COMMON_ARGUMENTEXTENDER_IPP
#define USERUTILS_COMMON_ARGUMENTEXTENDER_IPP

#include "UserUtils/Common/interface/BoostUtils/PTreeUtils.hpp"

/******************************************************************************/

template<typename T>
T
usr::ArgumentExtender::GetInput( const std::string& opt ) const
{
  return _argmap[opt].as<T>();
}

/******************************************************************************/

template<typename T>
std::vector<T>
usr::ArgumentExtender::GetInputList( const std::string& opt ) const
{
  return _argmap[opt].as<std::vector<T> >();
}

/******************************************************************************/

template<typename T>
T
usr::ArgumentExtender::GetExt( const std::string& opt, const std::string& exttag ) const
{
  return ExtQuery<T>( opt, GetInput<std::string>(opt), exttag );
}

/******************************************************************************/

template<typename T>
T
usr::ArgumentExtender::ExtQuery(
  const std::string& option,
  const std::string& optioninput,
  const std::string& tag ) const
{
  return usr::GetSingle<T>( Tree() ,option, optioninput, tag );
}

#endif/* end of include guard: USERUTILS_COMMON_ARGUMENTEXTENDER_IPP */
