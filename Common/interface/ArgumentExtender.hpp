/**
 * @file    ArgumentExtender.hpp
 * @brief   Class for managing main function inputs.
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifndef USERUTILS_COMMON_ARGUMENTEXTENDER_HPP
#define USERUTILS_COMMON_ARGUMENTEXTENDER_HPP

#include <boost/program_options.hpp>

#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/BoostUtils/PTreeUtils.hpp"
#include "UserUtils/Common/interface/STLUtils/VectorUtils.hpp"
#else
#include "UserUtils/Common/BoostUtils/PTreeUtils.hpp"
#include "UserUtils/Common/STLUtils/VectorUtils.hpp"
#endif

namespace usr {

namespace po = boost::program_options;

class ArgumentExtender
{
public:

  /**
   * @brief initialization from single json file.
   * @details see, ArgumentExtender::_init() for more details.
   */
  ArgumentExtender( const std::string& filename )
  { _init( {filename} ); }

  /**
   * @brief initialization from multiple json files
   * @details see, ArgumentExtender::_init() for more details.
   */
  ArgumentExtender( const std::vector<std::string>& list )
  { _init( list ); }

  /**
   * @brief varidaic initialization for multiple json files.
   * @details see ArgumentExtender::_init() for more details.
   */
  template<typename ... TS>
  ArgumentExtender( const std::string& first, TS ... others );

  virtual
  ~ArgumentExtender ();

  ArgumentExtender& AddOptions( const po::options_description& optdesc );
  void              ParseOptions( int argc, char** argv );
  bool              CheckArg( const std::string& opt ) const;

  template<typename TYPE = std::string>
  TYPE Arg( const std::string& opt ) const;
  template<typename TYPE>
  std::vector<TYPE> ArgList( const std::string& opt ) const;
  template<typename TYPE>
  TYPE ArgExt( const std::string& opt, const std::string& exttag ) const;

  /** @brief Constant access to internal property tree instance. */
  inline const pt::ptree&
  Tree() const { return _exttree; }

  /** @brief Constant access to internal options description instance. */
  inline const po::options_description&
  Description() const { return _optdesc; }

  /** @brief Constant access to internal argument value map instance. */
  inline const po::variables_map&
  Args() const { return _argmap; }

protected:

  /** @brief Mutable access to internal property tree instance. */
  inline pt::ptree&
  Tree(){ return _exttree; }

  /** @brief Mutable access to internal options description instance. */
  inline po::options_description&
  Description(){ return _optdesc; }

  /** @brief Mutable access to internal argument value map instance. */
  inline po::variables_map&
  Args(){ return _argmap; }

private:
  pt::ptree _exttree;
  po::options_description _optdesc;
  po::variables_map _argmap;

  void _init( const std::vector<std::string>& filelist );
};


/*-----------------------------------------------------------------------------
 *  Template implementation
   --------------------------------------------------------------------------*/
template<typename ... TS>
ArgumentExtender::ArgumentExtender( const std::string& first, TS ... others )
{ _init( MakeVector<std::string>( first, others ... ) ); }


/**
 * @brief template function for getting the user input of an option.
 * @details the user is responsible for casting the input to an appropriate
 * type.
 */
template<typename T>
T
ArgumentExtender::Arg( const std::string& opt ) const
{ return _argmap[opt].as<T>(); }

/**
 * @brief template function for getting the user input list to an option.
 * @details Notice that this must be used together with a
 * boost::program_options' multitoken when defining the options description.
 * Again the user is responsible for providing the appropriate casting.
 */
template<typename T>
std::vector<T>
ArgumentExtender::ArgList( const std::string& opt ) const
{ return _argmap[opt].as<std::vector<T> >(); }

/**
 * @brief template function for getting the extend object to an options.
 * @details The user is responsible for providing the correct string tag, and
 *          providing the correct type to case the data in the json file.
 */
template<typename T>
T
ArgumentExtender::ArgExt( const std::string& opt, const std::string& exttag ) const
{
  return usr::GetSingle<T>( Tree(), opt, Arg<std::string>( opt ), exttag );
}


}/* usr */

#endif/* end of include guard: USERUTILS_COMMON_OPTSNAMER_HPP */
