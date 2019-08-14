#ifndef USERUTILS_COMMON_ROOTUTILS_HPP
#define USERUTILS_COMMON_ROOTUTILS_HPP

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

  template<typename TYPE>
  inline const TYPE
  Get() const { return TYPE( &Get( TYPE::CmdName ) ); }

  template<typename TYPE>
  inline bool
  Has() const { return Has( TYPE::CmdName ); }

  const RooCmdArg& Get( const std::string& name ) const;
  bool             Has( const std::string& name ) const;
  static bool      CheckList( const std::vector<RooCmdArg>&,
                             const std::string& name );
  RooLinkedList MakeRooList( const std::vector<std::string>& exclude = {} )
  const;
};

}// namespace

// MACRO FOR DEFINING A NEW COMMAND
#define USRUTILS_COMMON_REGISTERCMD( TYPE )              \
  const std::string TYPE::CmdName = #TYPE;               \
  static int __usrutils_common_register__dummy ## TYPE = \
     usr::RooArgContainer::RegistorCommand( #TYPE );

#endif
