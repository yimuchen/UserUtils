/**
 * @file    ArgumentExtender.cc
 * @brief   Implementations for the ArgumentExtender class
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/ArgumentExtender.hpp"
#else
#include "UserUtils/Common/ArgumentExtender.hpp"
#endif

#include <boost/algorithm/string.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/errors.hpp>

#include <algorithm>
#include <cstdlib>
#include <exception>
#include <iostream>

namespace pt  = boost::property_tree;
namespace opt = boost::program_options;
namespace fs  = std::experimental::filesystem;

namespace usr {

/**
 * @brief Construction of the class property tree with a list of files.
 *
 * The constructor will ensure that the json structure for each entry under a
 * given options is uniformed in the json file. Notice that the options listed
 * in the json file will be considered a required arguments (hence the support
 * for multiple json files).
 *
 * An execption would be thrown if any parsing error occurs. Multiple options
 * with the same name would also be considered an exception.
 *
 * After sucessfully parsing the json file. The option `help/h` would be added
 * to for use (if used, the class will diplay the defined options and exit).
 */
void
ArgumentExtender::_init( const std::vector<std::string>& filelist )
{
  _exttree = FromJsonFiles( filelist );

  // Parsing the format of each options listed in ext trees
  BOOST_FOREACH( const pt::ptree::value_type& child, Tree() ){
    const std::string optname = child.first.data();
    if( child.second.size() == 0 ){
      std::cerr << "Bad format for option ["<< optname << "]" << std::endl;
      throw std::invalid_argument( optname );
    }

    std::vector<std::string> exttaglist;

    BOOST_FOREACH( const pt::ptree::value_type& subchild, child.second ){
      const std::string optval = subchild.first.data();
      if( subchild.second.size() == 0 ){
        std::cerr << "Bad format for option ["<< optname << "] "
                  << "with value [" << optval << "]" << std::endl;
        throw std::invalid_argument( optval );
      }

      std::vector<std::string> this_exttaglist;
      BOOST_FOREACH( const pt::ptree::value_type& leafp, subchild.second ){
        this_exttaglist.push_back( leafp.first.data() );
      }
      std::sort( this_exttaglist.begin(), this_exttaglist.end() );
      if( this_exttaglist.size() == 0 ){
        std::cerr << "No extended values for option ["<< optname << "] "
                  << "with value [" << optval << "]" << std::endl;
        throw std::invalid_argument( optval );
      }

      if( exttaglist.size() == 0  ){// Do not compare if this is empty
        exttaglist = this_exttaglist;
      } else {

        if( exttaglist.size() > this_exttaglist.size() ){
          for( const auto& tag : exttaglist ){
            if( !FindValue( this_exttaglist, tag ) ){
              std::cerr << "Missing extended value [" << tag <<"] "
                        << "for option ["<< optname << "] "
                        << "with value [" << optval << "]" << std::endl;
              throw std::invalid_argument( tag );
            }
          }
        } else if( exttaglist.size() < this_exttaglist.size() ){
          for( const auto& tag : this_exttaglist ){
            if( !FindValue( this_exttaglist, tag ) ){
              std::cerr << "Missing extended value [" << tag <<"] "
                        << "for option ["<< optname << "] "
                        << "with value [" << optval << "]" << std::endl;
              throw std::invalid_argument( tag );
            }
          }
        } else {
          for( size_t i = 0; i < exttaglist.size(); ++i ){
            if( exttaglist.at( i ) != this_exttaglist.at( i ) ){
              const std::string misval = exttaglist.at( i );
              std::cerr << "Undefined extended value [" << misval <<"] "
                        << "for option ["<< optname << "] "
                        << "with value [" << optval << "]" << std::endl;
              throw std::invalid_argument( misval );
            }
          }
        }
      }
    }
  }

  // Adding default arguments.
  Description().add_options()
    ( "help,h", "print help options and exit program" )
  ;
}

/**
 * @brief default destructor (nothing, relying on member auto cleaning)
 */
ArgumentExtender::~ArgumentExtender(){}

/**
 * @brief Adding more options definition to the class.
 *
 * Currently the only way of adding more options is to manually define
 * a new options_description instance, and adding that to the class.
 * The internal options_description instance would only directly contain
 * the options definition of "help/h".
 */
ArgumentExtender&
ArgumentExtender::AddOptions( const opt::options_description& desc )
{
  Description().add( desc );
  return *this;
}

// Parse the program input given the standard argc, argv interface.
// Throw exception when parse error or exit the program if "help" options
// is recieved.
// All functions listed under the json files would be listed as mandetory.
// and a parse error would be thrown if input is not detected.

/**
 * @brief parsing the rpgoram input with the standard argc, argv interface.
 *
 * Raising exception if there is any error in parsing. Additional criterias
 * that counts as an error in parsing includes:
 * 1. The json file contains an options not listed in the final
 *     options_description instance.
 * 2. A option listed in the json does not have a corresponding user input.
 * 3. The user input for an option listed in the json file doesn't match
 *    any of inputs listed in the json file.
 *
 * After all parsing is complete, the function will check for the "help/h"
 * input. If it exists, the function will print the options_description and
 * terminate the program.
 */
void
ArgumentExtender::ParseOptions( int argc, char** argv )
{
  try {
    opt::store( opt::parse_command_line( argc, argv, Description() ), Args() );
    opt::notify( Args() );
  } catch( boost::exception& e ){
    std::cerr << "Error parsing command!" << std::endl
              << boost::diagnostic_information( e )
              << Description()
              << std::endl;
    throw;// Continue throwing exception
  }

  // Checking that each of the options listed in the json files
  // are present. Will need to
  BOOST_FOREACH( const pt::ptree::value_type& child, Tree() ){
    const std::string optname = child.first.data();

    try {
      Description().find( optname, true );
    } catch( boost::exception& e ){
      std::cerr << "Options [" << optname << "] defined in file"
                << "does not have description defined!" << std::endl;
      throw;
    }

    if( !CheckArg( optname ) ){
      std::cerr << "Missing options for [" << optname << "]" << std::endl
                << Description()
                << std::endl;
      BOOST_THROW_EXCEPTION( po::required_option( optname ) );
    }

    std::vector<std::string> validoptionlist;
    if( !CheckQuery( Tree(), optname, Arg<std::string>( optname ) ) ){
      std::cerr << "Extended values for options for [" << optname << "] "
                << " with value [" << Arg<std::string>( optname ) << "] "
                << " is not defined!"
                << std::endl
                << "Available values: " << std::flush;
      BOOST_FOREACH( const pt::ptree::value_type& subchild, child.second ){
        const std::string optvalue = subchild.first.data();
        std::cerr << "[" << optvalue << "] " << std::flush;
      }
      std::cerr << std::endl;

      throw std::invalid_argument( Arg<std::string>( optname ) );
    }
  }

  if( CheckArg( "help" ) ){// Early exit in case of help option
    std::cerr << Description() << std::endl;
    std::exit( EXIT_SUCCESS );
  }
}

/**
 * @brief Checking if the user input exists for an option.
 * @details Should be called after the ParseOptions() method has been called.
 */
bool
ArgumentExtender::CheckArg( const std::string& opt ) const
{
  return Args().count( opt );
}

void
ArgumentExtender::SetFilePrefix( const fs::path pre )
{
  _prefix = pre;
}

void
ArgumentExtender::SetDirScheme( const PathScheme& newscheme )
{
  _dirscheme = newscheme;
}

void
ArgumentExtender::AddDirScheme( const ArgPathScheme& arg )
{
  _dirscheme.push_back( arg );
}

void
ArgumentExtender::AddDirScheme( const PathScheme& newscheme )
{
  for( const auto& x : newscheme ){
    _dirscheme.push_back( x );
  }
}

void
ArgumentExtender::SetNameScheme( const PathScheme& newscheme )
{
  _namescheme = newscheme;
}

void
ArgumentExtender::AddNameScheme( const ArgPathScheme& arg )
{
  _namescheme.push_back( arg );
}

void
ArgumentExtender::AddNameScheme( const PathScheme& newscheme )
{
  for( const auto& x : newscheme ){
    _namescheme.push_back( x );
  }
}

fs::path
ArgumentExtender::MakeFile(
  const std::string& nameprefix,
  const std::string& ext ) const
{
  fs::path dirname  = _prefix;
  fs::path filename = nameprefix;

  for( const auto& x : _dirscheme ){
    dirname /= genPathString( x );
  }

  for( const auto& x : _namescheme ){
    filename += "_" + genPathString( x );
  }

  filename += "." + ext;

  return fs::path( dirname / filename );
}

fs::path
ArgumentExtender::MakePDFFile( const std::string& x ) const
{
  return MakeFile( x, "pdf" );
}

fs::path
ArgumentExtender::MakePNGFile( const std::string& x ) const
{
  return MakeFile( x, "png" );
}

fs::path
ArgumentExtender::MakeTXTFile( const std::string& x ) const
{
  return MakeFile( x, "txt" );
}

fs::path
ArgumentExtender::MakeTEXFile( const std::string& x ) const
{
  return MakeFile( x, "tex" );
}

std::string
ArgumentExtender::genPathString( const ArgPathScheme& x ) const
{
  if( !CheckArg( x.option ) ){ return ""; }

  boost::format genfmt( "%s%s" );
  boost::format intfmt( "%d" );
  boost::format fltfmt( "%lg" );
  const std::string optstring = x.pathstring;

  std::string inputstring;

  if( inputstring == "" ){
    try {
      inputstring = Arg<std::string>( x.option );
    } catch( ... ){
    }
  }

  if( inputstring == "" ){
    try {
      inputstring = ( intfmt % Arg<int>( x.option ) ).str();
    } catch( ... ){
    }
  }

  if( inputstring == "" ){
    try {
      inputstring = ( fltfmt % Arg<float>( x.option ) ).str();
      boost::replace_all( inputstring, ".", "p" );
    } catch( ... ){
    }
  }

  return ( genfmt % optstring % inputstring ).str();
}

}/* usr */
