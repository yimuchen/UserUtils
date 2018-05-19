/**
 * @file    argumentextender_test.cc
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */

#include "UserUtils/Common/interface/ArgumentExtender.hpp"

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
