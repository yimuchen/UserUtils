#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/RapidJson.hpp"
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"
#else
#include "UserUtils/Common/RapidJson.hpp"
#include "UserUtils/Common/STLUtils/StringUtils.hpp"
#endif

#include <fstream>
#include <sstream>
#include <string>

namespace usr {

JSONDocument
FromJSONFile( const std::string& filename )
{
  JSONDocument ans;
  std::ifstream input( filename );
  std::ostringstream ss;
  ss << input.rdbuf();

  rapidjson::ParseResult results = ans.Parse( ss.str().c_str() );

  if( !results ){
    throw std::invalid_argument(
      usr::fstr( "Error parsing json file \"%s\" (%s)", filename
               , rapidjson::GetParseErrorFunc( results.Code() ) ) );
  }

  return ans;
};

JSONDocument
FromJSONFiles( const std::vector<std::string>& files )
{
  if( files.size() == 0 ){
    JSONDocument ans;
    ans.Parse( "{}" );
    return ans;
  } else {
    JSONDocument first = FromJSONFile( files[0] );

    for( unsigned i = 1; i < files.size(); ++i ){
      JSONDocument temp = FromJSONFile( files[i] );
      MergeJSON( first, temp, temp.GetAllocator() );
    }

    // After the merging, print the first to a string then have
    // RapidJson pare the string again to avoid assertion errors
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer( buffer );
    first.Accept( writer );

    JSONDocument ans;
    rapidjson::ParseResult results = ans.Parse( buffer.GetString() );

    if( !results ){
      throw std::invalid_argument(
        usr::fstr( "Error Joining json files (%s)",
          rapidjson::GetParseErrorFunc( results.Code() ) ) );
    }

    return ans;
  }
}

/**
 * @brief
 *
 * Reference: https://stackoverflow.com/questions/40013355/how-to-merge-two-json-file-using-rapidjson
 * @param dstObject
 * @param srcObject
 * @param allocator
 * @return true
 * @return false
 */
bool
MergeJSON( JSONMap&                     dstObject,
           JSONMap&                     srcObject,
           JSONDocument::AllocatorType& allocator )
{
  for( auto srcIt = srcObject.MemberBegin();
       srcIt != srcObject.MemberEnd(); ++srcIt ){
    auto dstIt = dstObject.FindMember( srcIt->name );
    if( dstIt == dstObject.MemberEnd() ){
      rapidjson::Value dstName;
      dstName.CopyFrom( srcIt->name, allocator );
      rapidjson::Value dstVal;
      dstVal.CopyFrom( srcIt->value, allocator );

      dstObject.AddMember( dstName, dstVal, allocator );

      dstName.CopyFrom( srcIt->name, allocator );
      dstIt = dstObject.FindMember( dstName );
      if( dstIt == dstObject.MemberEnd() ){
        return false;
      }
    } else {
      auto srcT = srcIt->value.GetType();
      auto dstT = dstIt->value.GetType();
      if( srcT != dstT ){
        return false;
      }

      if( srcIt->value.IsArray() ){
        for( auto arrayIt = srcIt->value.Begin();
             arrayIt != srcIt->value.End(); ++arrayIt ){
          rapidjson::Value dstVal;
          dstVal.CopyFrom( *arrayIt, allocator );
          dstIt->value.PushBack( dstVal, allocator );
        }
      } else if( srcIt->value.IsObject() ){
        if( !MergeJSON( dstIt->value, srcIt->value, allocator ) ){
          return false;
        }
      } else {
        dstIt->value.CopyFrom( srcIt->value, allocator );
      }
    }
  }

  return true;
}


void
ExceptJSONList( const JSONMap& map, const std::string& index )
{
  if( !map.HasMember( index.c_str() ) ){
    throw std::invalid_argument(
      usr::fstr( "No member with index [%s] found in json", index ) );
  }
  if( !map[index.c_str()].IsArray() ){
    throw std::invalid_argument(
      usr::fstr( "Member of index [%s] is invalid type (not an array)"
               , index ) );
  }
}

void
ExceptJSONObj( const JSONMap& map, const std::string& index )
{
  if( !map.HasMember( index.c_str() ) ){
    throw std::invalid_argument(
      usr::fstr( "No member with index [%s] found in json", index ) );
  }
  if( !map[index.c_str()].IsObject() ){
    throw std::invalid_argument(
      usr::fstr( "Member of index [%s] is invalid type (not an object)"
               , index ) );
  }
}

// ------------------------------------------------------------------------------
#define MAKE_CONCRETE_JSONENTRY( TYPE, IS_FUNCTION, GET_FUNCTION )             \
  template<>                                                                   \
  void                                                                         \
  ExceptJSONEntry<TYPE>( const JSONMap& map, const std::string& index )        \
  {                                                                            \
    if( !map.HasMember( index.c_str() ) ){                                     \
      throw std::invalid_argument(                                             \
  usr::fstr( "No member with index [%s] found in json", index ) );             \
    }                                                                          \
    if( !map[index.c_str()].IS_FUNCTION() ){                                   \
      throw std::invalid_argument(                                             \
  usr::fstr( "Member with index [%s] is invalid type (expected %s)"            \
           , index, #TYPE  ) );                                                \
    }                                                                          \
  }                                                                            \
  template<>                                                                   \
  TYPE                                                                         \
  JSONEntry<TYPE>( const JSONMap& map, const std::string& index )              \
  {                                                                            \
    ExceptJSONEntry<TYPE>( map, index );                                       \
    return map[index.c_str()].GET_FUNCTION();                                  \
  }                                                                            \
  template<>                                                                   \
  TYPE                                                                         \
  JSONEntry<TYPE>( const JSONMap& map,                                         \
  const std::string& index,                                                    \
  const TYPE& default_return )                                                 \
  {                                                                            \
    if( map.HasMember( index.c_str() ) ){                                      \
      return JSONEntry<TYPE>( map, index );                                    \
    } else {                                                                   \
      return default_return;                                                   \
    }                                                                          \
  }

MAKE_CONCRETE_JSONENTRY( double,      IsNumber, GetDouble );
MAKE_CONCRETE_JSONENTRY( std::string, IsString, GetString );
MAKE_CONCRETE_JSONENTRY( bool,        IsBool,   GetBool   );

#undef MAKE_CONRETE_JSONENTRY



#define MAKE_CONCRETE_JSONLIST( TYPE, IS_FUNCTION, GET_FUNCTION )              \
  template<>                                                                   \
  std::vector<TYPE>                                                            \
  JSONList<TYPE>( const JSONMap& map, const std::string& index )               \
  {                                                                            \
    ExceptJSONList( map, index );                                              \
    std::vector<TYPE> ans;                                                     \
    for( const auto& val : map[index.c_str()].GetArray() ){                    \
      if( !val.IS_FUNCTION() ){                                                \
        throw std::invalid_argument( usr::fstr(                                \
  "Member in array [%s] is invalid type (expected %s)"                         \
                                              , index, #TYPE ).c_str() );      \
      }                                                                        \
      ans.push_back( val.GET_FUNCTION() );                                     \
    }                                                                          \
    return ans;                                                                \
  }                                                                            \
  template<>                                                                   \
  std::vector<TYPE>                                                            \
  JSONList<TYPE>( const JSONMap& map, const std::string& index,                \
  const std::vector<TYPE>& default_return  )                                   \
  {                                                                            \
    if( map.HasMember( index.c_str() ) ){                                      \
      return JSONList<TYPE>( map, index );                                     \
    } else {                                                                   \
      return default_return;                                                   \
    }                                                                          \
  }


MAKE_CONCRETE_JSONLIST( double,      IsNumber, GetDouble );
MAKE_CONCRETE_JSONLIST( std::string, IsString, GetString );
MAKE_CONCRETE_JSONLIST( bool,        IsBool,   GetBool   );

#undef MAKE_CONCRETE_JSONLIST

}/* usr */
