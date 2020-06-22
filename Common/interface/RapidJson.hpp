#ifndef USERUTILS_COMMON_RAPIDJSON_HPP
#define USERUTILS_COMMON_RAPIDJSON_HPP
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include <string>
#include <vector>

namespace usr {

/**
 * @defgroup rapidjson RapidJSON helper functions
 * @ingroup Common
 * @brief Function for typical use cases of read-only JSON manipulations
 * @{
 */

typedef rapidjson::Document JSONDocument;
typedef rapidjson::Value    JSONMap;

extern JSONDocument FromJSONFile( const std::string& filename );
extern JSONDocument FromJSONFiles( const std::vector<std::string>& filename );


extern bool MergeJSON( JSONMap&                     first,
                       JSONMap&                     second,
                       JSONDocument::AllocatorType& allocator );

extern void ExceptJSONList( const JSONMap&     map,
                            const std::string& index );

extern void ExceptJSONObj( const JSONMap&     map,
                           const std::string& index );


/**
 * @brief For a immediate entry of a JSON map, ensure the entry exists and is a
 * the correct variable type. Raises an exception if any of the criteria is not
 * met.
 */
template<typename TYPE>
extern void ExceptJSONEntry( const JSONMap&     map,
                             const std::string& index );

/**
 * @brief For an immedidate entry of a JSON map, if the entry exists and is of
 * the correct variable type, return a properly casted results. Raises and
 * exception otherwise.
 */
template<typename TYPE>
extern TYPE JSONEntry( const JSONMap& map,  const std::string& index );

/**
 * @brief For a immediate entry of a JSON map, return the entry of a JSON map
 * given a specific type. If the entry doesn't exists, return a default result.
 * If the entry exists but doesn't match to correct type, raise an excpetion.
 */
template<typename TYPE>
extern TYPE JSONEntry( const JSONMap&     map,
                       const std::string& index,
                       const TYPE&        default_return );


/**
 * @brief For an immediate list entry of a JSON map, ensure the entry exists and
 * that all entries in the list is of the same time, return the results as a
 * std::vector. If any of the criteria are not met, raise an exception.
 */
template<typename TYPE>
extern std::vector<TYPE> JSONList( const JSONMap&     map,
                                   const std::string& index );

/**
 * @brief For an immediate list entry of a JSON map, return the entry as a
 * std::vector. If the entry doesn't exists, return a default value. If the entry
 * exists but has issues casting to the request data type. Raise an exception.
 */
template<typename TYPE>
extern std::vector<TYPE> JSONList( const JSONMap&           map,
                                   const std::string&       index,
                                   const std::vector<TYPE>& default_return );

/** @} */

}/* usr */

#endif
