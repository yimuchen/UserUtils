#include "UserUtils/EDMUtils/interface/ParseEDM.hpp"

#include <regex>

edm::EventID
ParseEvent( const std::string& input  )
{
  static const edm::EventID defret( 0, 0, 0 );
  static const std::regex   run_format( "^([0-9]+):([0-9]+):([0-9]+)$" );
  static const std::regex   lumi_format( "^([0-9]+):([0-9]+)$" );
  static const std::regex   evt_format( "^([0-9]+)$" );

  std::smatch match;

  if( std::regex_match( input, match, run_format ) ){
    if( match.size() == 4 ){
      const std::string run  = match[1];
      const std::string lumi = match[2];
      const std::string evt  = match[3];
      return edm::EventID( std::stoi( run )
                           ,
                           std::stoi( lumi )
                           ,
                           std::stoi( evt ) );
    } else {
      return defret;
    }
  } else if( std::regex_match( input, match, lumi_format ) ){
    if( match.size() == 3 ){
      const std::string lumi_str = match[1];
      const std::string evt_str  = match[2];
      return edm::EventID( 1
                           ,
                           std::stoi( lumi_str )
                           ,
                           std::stoi( evt_str ) );
    } else {
      return defret;
    }
  } else if( std::regex_match( input, match, evt_format ) ){
    if( match.size() == 2 ){
      const std::string evt_str = match[1];
      return edm::EventID( 1, 0, std::stoi( evt_str ) );
    } else {
      return defret;
    }
  } else {
    return defret;
  }
}
