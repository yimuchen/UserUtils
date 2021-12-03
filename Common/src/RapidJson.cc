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

namespace usr
{

/**
 * @addtogroup rapidjson
 * @details
 *
 * [RapidJSON](https://rapidjson.org/) is a fast and robust json parsing
 * library.
 * With a smaller footprint, faster parsing, and more concrete JSON specific
 * features than the JSON parser available in the boost library (see boost's
 * [`property
 * tree`](https://www.boost.org/doc/libs/1_73_0/doc/html/property_tree.html)),
 * and since RapidJSON is a header only library, this is decided as the external
 * dependency used for JSON file parsing.
 *
 * Note that RapidJSON essentially treats the results of JSON parsing as a map.
 * So for a JSON file like:
 *
 * ```json
 * {
 *  "title": "JSON TEST",
 *  "list" : [ 1,2,3 ],
 *  "object" : {
 *      "name" : "OJBECT TEST",
 *      "list" : [4,5,6]
 *   },
 *   "object_list" : [
 *      { "name": "name_1" },
 *      {"name": "name_2"}
 *   ]
 * }
 * ```
 *
 * The resulting parsed map will be have the following properties:
 *
 * ```cpp
 * usr::JSONDocument json = FromJSONFile( "json.json" );
 * json["title"].GetString(); //  "JSON TEST"
 * json["list"][0].GetDouble() ; // 1.0
 * json["list"][2].GetDouble() ; // 2.0
 * json["object"]["name"].GetString() ; // "OBJECT TEST"
 * json["object list"][1]["name"].GetString() ; "name_2"
 * ```
 *
 * A major focus of this library is to reduce verbosity in reading concrete
 * types. For example, we can simply have:
 *
 * ```cpp
 * std::string title = usr::JSONEntry<std::string>(json, "title");
 * std::vector<double> list1 = usr::JSONList<double>(json, "list");
 * std::vector<double> list2 = usr::JSONList<double>(json["object"], "list");
 * ```
 *
 * which will automatically detect the entry if it exists, and raise suitable
 * exception messages otherwise. This library only implements the function
 * required for concrete JSON types: `std::string`, `double`, `bool`, and their
 * array counter parts. For parsing JSON object into custom C++ classes,
 * something that looks like
 *
 * ```cpp
 * MyClass instance = usr::JSONEntry<MyClass>("json", "object");
 * ```
 * in the above example, you will need to specialize the corresponding template
 * function.
 */


/**
 * @brief Getting a json document from a single input files
 *
 * This function is basically a wrap for using the standard
 * `rapidjson::Document::Parse` function, for using a C++ string to identify the
 * file to open.
 */
JSONDocument
FromJSONFile( const std::string& filename )
{
  JSONDocument       ans;
  std::ifstream      input( filename );
  std::ostringstream ss;
  ss << input.rdbuf();

  rapidjson::ParseResult results = ans.Parse( ss.str().c_str() );

  if( !results ){
    throw std::invalid_argument(
            usr::fstr( "Error parsing json file \"%s\" (%s)",
                       filename
                       ,
                       rapidjson::GetParseErrorFunc( results.Code() ) ) );
  }

  return ans;
}


/**
 * @brief Getting the joint JSON document from multiple inputs.
 *
 * For each JSON in the entry, the JSON file is joint using the `usr::MergeJSON`
 * method to the previous files. To ensure memory ownership, the final merge
 * results is passed to a string output processes and the return object is a
 * standalone JSON map instance that parses the generated string.
 */
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
    rapidjson::StringBuffer                    buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer( buffer );
    first.Accept( writer );

    JSONDocument           ans;
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
 * @brief Merging of two json maps
 *
 * This follows the the answer given on StackOverflow (see reference below).
 * Essentially: for the latter JSON map: any leaf entry that matches a path with
 * the original JSON map will superced the original. If a leaf entry doesn't
 * have
 * a corresponding entry in the original map, it is created. If two arrays have
 * the same path, the arrays will be concatinated together.
 *
 * Reference:
 * https://stackoverflow.com/questions/40013355/how-to-merge-two-json-file-using-rapidjson
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


/**
 * @brief Check that an immediate entry of a JSON map exists and is a List.
 *
 * In case that the either of the criteria is not met, the function raises an
 * exception.
 */
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
                       ,
                       index ) );
  }
}


/**
 * @brief Check that an immediate entry of a JSON map exists and is an object.
 *
 * In case that either of the criteria is not met, the function raises and
 * exception.
 */
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
                       ,
                       index ) );
  }
}


// ------------------------------------------------------------------------------
#define MAKE_CONCRETE_JSONENTRY( TYPE, IS_FUNCTION, GET_FUNCTION )              \
  template<>                                                                    \
  void                                                                          \
  ExceptJSONEntry<TYPE>( const JSONMap& map, const std::string& index )         \
  {                                                                             \
    if( !map.HasMember( index.c_str() ) ){                                      \
      throw std::invalid_argument(                                              \
              usr::fstr( "No member with index [%s] found in json", index ) );  \
    }                                                                           \
    if( !map[index.c_str()].IS_FUNCTION() ){                                    \
      throw std::invalid_argument(                                              \
              usr::fstr( "Member with index [%s] is invalid type (expected %s)" \
                         ,                                                      \
                         index,                                                 \
                         #TYPE  ) );                                            \
    }                                                                           \
  }                                                                             \
  template<>                                                                    \
  TYPE                                                                          \
  JSONEntry<TYPE>( const JSONMap& map, const std::string& index )               \
  {                                                                             \
    ExceptJSONEntry<TYPE>( map, index );                                        \
    return map[index.c_str()].GET_FUNCTION();                                   \
  }                                                                             \
  template<>                                                                    \
  TYPE                                                                          \
  JSONEntry<TYPE>( const JSONMap& map,                                          \
                   const std::string& index,                                    \
                   const TYPE& default_return )                                 \
  {                                                                             \
    if( map.HasMember( index.c_str() ) ){                                       \
      return JSONEntry<TYPE>( map, index );                                     \
    } else {                                                                    \
      return default_return;                                                    \
    }                                                                           \
  }

MAKE_CONCRETE_JSONENTRY( double,      IsNumber, GetDouble );
MAKE_CONCRETE_JSONENTRY( std::string, IsString, GetString );
MAKE_CONCRETE_JSONENTRY( bool,        IsBool,   GetBool   );

#undef MAKE_CONRETE_JSONENTRY


#define MAKE_CONCRETE_JSONLIST( TYPE, IS_FUNCTION, GET_FUNCTION )                           \
  template<>                                                                                \
  std::vector<TYPE>                                                                         \
  JSONList<TYPE>( const JSONMap& map, const std::string& index )                            \
  {                                                                                         \
    ExceptJSONList( map, index );                                                           \
    std::vector<TYPE> ans;                                                                  \
    for( const auto& val : map[index.c_str()].GetArray() ){                                 \
      if( !val.IS_FUNCTION() ){                                                             \
        throw std::invalid_argument( usr::fstr(                                             \
                                       "Member in array [%s] is invalid type (expected %s)" \
                                       ,                                                    \
                                       index,                                               \
                                       #TYPE ).c_str() );                                   \
      }                                                                                     \
      ans.push_back( val.GET_FUNCTION() );                                                  \
    }                                                                                       \
    return ans;                                                                             \
  }                                                                                         \
  template<>                                                                                \
  std::vector<TYPE>                                                                         \
  JSONList<TYPE>( const JSONMap& map,                                                       \
                  const std::string& index,                                                 \
                  const std::vector<TYPE>& default_return  )                                \
  {                                                                                         \
    if( map.HasMember( index.c_str() ) ){                                                   \
      return JSONList<TYPE>( map, index );                                                  \
    } else {                                                                                \
      return default_return;                                                                \
    }                                                                                       \
  }


MAKE_CONCRETE_JSONLIST( double,      IsNumber, GetDouble );
MAKE_CONCRETE_JSONLIST( std::string, IsString, GetString );
MAKE_CONCRETE_JSONLIST( bool,        IsBool,   GetBool   );

#undef MAKE_CONCRETE_JSONLIST

}/* usr */
