/**
 * @file    argumentextender_test.cc
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/ArgumentExtender.hpp"
#else
#include "UserUtils/Common/ArgumentExtender.hpp"
#endif

int main(int argc, char* argv[])
{
  // usr::ArgumentExtender myarg("test.json"); // Read Error!
  //usr::ArgumentExtender myarg("name1.json");
  usr::ArgumentExtender myarg("name1.json","name2.json");

  usr::po::options_description test("test options");
  test.add_options()
    ("channel,c",usr::po::value<std::string>())
    ("era,e",usr::po::value<std::string>())
    ("fitfunc,f",usr::po::value<std::string>())
    ("combine,x",usr::po::value<std::string>())
  ;

  myarg.AddOptions(test);

  myarg.ParseOptions(argc,argv);

  return 0;
}
