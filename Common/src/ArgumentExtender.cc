/*******************************************************************************
*
*  Filename    : ArgumentExtender.cc
*  Description : Implementations for ArgumentExtender class
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "UserUtils/Common/interface/ArgumentExtender.hpp"

#include <boost/exception/diagnostic_information.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/errors.hpp>

#include <algorithm>
#include <cstdlib>
#include <exception>
#include <iostream>

namespace pt  = boost::property_tree;
namespace opt = boost::program_options;

namespace usr {

/*-----------------------------------------------------------------------------
 *  Constructor and destructor -- "help" Argument is manditorily added.
   --------------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------------*/

ArgumentExtender::~ArgumentExtender(){}

/*-----------------------------------------------------------------------------
 *  Option loading function
   --------------------------------------------------------------------------*/
ArgumentExtender&
ArgumentExtender::AddOptions( const opt::options_description& desc )
{
  Description().add( desc );
  return *this;
}

/*----------------------------------------------------------------------------*/

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

    if( !CheckInput( optname ) ){
      std::cerr << "Missing options for [" << optname << "]" << std::endl
                << Description()
                << std::endl;
      BOOST_THROW_EXCEPTION( po::required_option( optname ) );
    }

    std::vector<std::string> validoptionlist;
    if( !CheckQuery( Tree(), optname, GetInput<std::string>( optname ) ) ){
      std::cerr << "Extended values for options for [" << optname << "] "
                << " with value [" << GetInput<std::string>( optname ) << "] "
                << " is not defined!"
                << std::endl
                << "Available values: " << std::flush;
      BOOST_FOREACH( const pt::ptree::value_type& subchild, child.second ){
        const std::string optvalue = subchild.first.data();
        std::cerr << "[" << optvalue << "] " << std::flush;
      }
      std::cerr << std::endl;

      throw std::invalid_argument( GetInput<std::string>( optname ) );
    }
  }

  if( CheckInput( "help" ) ){// Early exit in
    std::cerr << Description() << std::endl;
    std::exit( EXIT_SUCCESS );
  }
}

/*******************************************************************************
*   Input access functions
*******************************************************************************/
bool
ArgumentExtender::CheckInput( const std::string& opt ) const
{
  return Args().count( opt );
}

}/* usr */
