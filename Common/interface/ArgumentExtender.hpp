/**
 * @file    ArgumentExtender.hpp
 * @brief   Class for managing main function inputs, basically a wrapper class
 *          for the boost/program_options classes. Extended naming of options
 *          is also handled.
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 * @details Often in HEP programs, we need a short string for process
 * indentification. But a longer string for the publication/presentation
 * outputs or potentially for calculations. This could be listed in a
 * json file as something like:
 * ```
 * Options1:
 *    proc1:
 *      fullname:"process_{1}: using procedure A"
 *      plotname:"process_{1} (A)"
 *      varval: 1000
 *    proc2:
 *      fullname:"process_{2}:  using prcedure B"
 *      plotname: "process_{2}: B"
 *      varval: 2000
 * ```
 * The ArgumentExtender class is a wrapper for a
 *  - boost::program_options::options_description instance
 *  - boost::program_options::variables_map instance
 *  - boost::property_tree to store the json file.
 * Functions are added to simplify the argument parsing process, as well as
 * getting the extended argument values. Additional parsing rules will be
 * added.
 */
#ifndef USERUTILS_COMMON_ARGUMENTEXTENDER_HPP
#define USERUTILS_COMMON_ARGUMENTEXTENDER_HPP

#include <boost/program_options.hpp>

#include "UserUtils/Common/interface/BoostUtils/PTreeUtils.hpp"
#include "UserUtils/Common/interface/STLUtils/VectorUtils.hpp"

namespace usr {

namespace po = boost::program_options;

class ArgumentExtender
{

public:
  // Must initialize with option naming json file.
  // The constructor will ensure that the json structure for each entry
  // under a given options is uniformed in the json file.
  // As options listed in the json file would be considered required
  // arguments, the class support multiple json file inputs. The
  // final property tree would be merged. Exception would be thrown if
  // Multiple options existed.
  ArgumentExtender( const std::string& filename ){ _init( {filename} ); }
  ArgumentExtender( const std::vector<std::string>& list ){_init( list ); }
  // Variadic interface
  template<typename ... TS>
  ArgumentExtender( const std::string& first, TS ... others )
  {_init( MakeVector<std::string>( first, others ... ) );}

  virtual
  ~ArgumentExtender ();

  // Add additional boost program options description object into master
  // description object (master would only contain the "help" option)
  ArgumentExtender& AddOptions( const po::options_description& optdesc );

  // Parse the program input given the standard argc, argv interface.
  // Throw exception when parse error or exit the program if "help" options
  // is recieved.
  // All functions listed under the json files would be listed as mandetory.
  // and a parse error would be thrown if input is not detected.
  void ParseOptions( int argc, char** argv );

  // Basic access functions to option input
  bool CheckInput( const std::string& opt ) const;

  // Template functions for getting raw input values
  template<typename T>
  T GetInput( const std::string& opt ) const;

  // Getting list of raw input objects, needs to be used in conjunction with
  // boost program options' multitoken object
  template<typename T>
  std::vector<T> GetInputList( const std::string& opt ) const;

  // Template functions for getting extended values from json file
  template<typename T>
  T GetExt( const std::string& opt, const std::string& extag ) const;

  // template functions for general PTree query
  template<typename T>
  T ExtQuery(
    const std::string& option,
    const std::string& optioninput,
    const std::string& tag ) const;

  // Container access functions
  inline const pt::ptree&
  GetTree() const { return _exttree; }

  inline const po::options_description&
  GetDescription() const { return _optdesc; }

  inline const po::variables_map&
  GetArgs() const { return _argmap;  }

protected:
  // Constant access functions
  inline const pt::ptree&
  Tree() const { return _exttree; }

  inline const po::options_description&
  Description() const { return _optdesc; }

  inline const po::variables_map&
  Args() const { return _argmap;  }

  // Mutable access functions
  inline pt::ptree&
  Tree(){ return _exttree; }

  inline po::options_description&
  Description(){ return _optdesc; }

  inline po::variables_map&
  Args(){ return _argmap;  }

private:
  pt::ptree _exttree;
  po::options_description _optdesc;
  po::variables_map _argmap;

  // Helper function for constructors
  void _init( const std::vector<std::string>& filelist );
};

}/* usr */

#include "UserUtils/Common/src/ArgumentExtender.ipp"

#endif/* end of include guard: USERUTILS_COMMON_OPTSNAMER_HPP */
