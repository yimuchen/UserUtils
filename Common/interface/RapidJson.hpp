#ifndef USERUTILS_COMMON_RAPIDJSON_HPP
#define USERUTILS_COMMON_RAPIDJSON_HPP
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <string>
#include <vector>

namespace usr {

typedef rapidjson::Document JSONDocument;
typedef rapidjson::Value    JSONMap;

extern JSONDocument FromJSONFile( const std::string& filename );
extern JSONDocument FromJSONFiles( const std::vector<std::string>& filename );


extern bool MergeJSON( JSONMap&                     first,
                       JSONMap&                     second,
                       JSONDocument::AllocatorType& allocator );

// Defining the template here, all implementation will be require concrete types
// to be placed in .cpp file.
template<typename TYPE>
extern void ExceptJSONEntry( const JSONMap&     map,
                             const std::string& index );

template<typename TYPE>
extern TYPE JSONEntry( const JSONMap& map,  const std::string& index );

template<typename TYPE>
extern TYPE JSONEntry( const JSONMap&     map,
                       const std::string& index,
                       const TYPE&        default_return );


// This doesn't need to be a template!
extern void ExceptJSONList( const JSONMap&     map,
                            const std::string& index );

extern void ExceptJSONObj( const JSONMap&     map,
                           const std::string& index );

template<typename TYPE>
extern std::vector<TYPE> JSONList( const JSONMap&     map,
                                   const std::string& index );

template<typename TYPE>
extern std::vector<TYPE> JSONList( const JSONMap&           map,
                                   const std::string&       index,
                                   const std::vector<TYPE>& default_return );

} /* usr */

#endif