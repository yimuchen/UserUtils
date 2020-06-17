#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/RapidJson.hpp"
#else
#include "UserUtils/Common/RapidJson.hpp"
#endif

#include <iostream>

int main()
{
  usr::JSONDocument list1 = usr::FromJSONFiles( {"list1.json", "list2.json"} );

  assert( list1["list"].IsArray() ) ;
  std::cout << list1["list"].GetArray().Size()  << std::endl;
  std::cout << list1["list"][0]["name"].GetString() << std::endl;


  usr::JSONDocument test_json = usr::FromJSONFile( "test.json" );

  std::cout << usr::JSONEntry<double>( test_json, "Static1" ) << std::endl;
  std::cout << usr::JSONEntry<std::string>( test_json, "Static2" ) << std::endl;

  const auto parsed_list = usr::JSONList<double>( test_json["Instance1"],
                                                   "var1" );

}
