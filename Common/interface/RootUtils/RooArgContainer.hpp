/**
 * @file RooArgContainer.hpp
 * @author Yi-Mu "Enoch" Chen
 * @brief RooArgContainter declaration class
 */

#ifndef USERUTILS_COMMON_ROOTUTILS_ROOARGCONTAINER_HPP
#define USERUTILS_COMMON_ROOTUTILS_ROOARGCONTAINER_HPP

#include <string>
#include <vector>

#include <RooCmdArg.h>

namespace usr
{

/**
 * @brief Simple container for helping with RooCmdArg parsing.
 */
class RooArgContainer : public std::vector<RooCmdArg>
{
public:
  static std::vector<std::string> CustomCommandList;
  static unsigned RegistorCommand( const std::string& );

  RooArgContainer( const std::vector<RooCmdArg>& arg_list,
                   const std::vector<RooCmdArg>& default_list = {} );
  virtual ~RooArgContainer();

  const RooCmdArg& Get( const std::string& name ) const;
  bool             Has( const std::string& name ) const;
  static bool      CheckList( const std::vector<RooCmdArg>&,
                              const std::string& name );

  // Directly getting the Cmd stored arguments
  int            GetInt( const std::string&, const unsigned index    = 0 ) const;
  double         GetDouble( const std::string&, const unsigned index = 0 ) const;
  std::string    GetStr( const std::string&, const unsigned index    = 0 ) const;
  const TObject& GetObj( const std::string&, const unsigned index    = 0 ) const;


  RooLinkedList MakeRooList( const std::vector<std::string>& exclude = {} )
  const;
};

}// namespace


#define USERUTILS_COMMON_REGISTERCMD( TYPE )              \
  static int __usrutils_common_register__dummy ## TYPE = \
    usr::RooArgContainer::RegistorCommand( #TYPE );

#endif
