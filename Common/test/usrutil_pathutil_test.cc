/**
 * @file    pathutil_test.cc
 * @brief   Unit test for the path utility functions
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 * @details Detailed descriptions
 */

#include "UserUtils/Common/interface/STLUtils/Filesystem.hpp"
#include "UserUtils/Common/interface/STLUtils.hpp"

#include <iostream>
#include <boost/format.hpp>
using namespace std;


int
main( int argc, char* argv[] )
{
  cout << usr::cmssw_src() << endl;
  cout << usr::pkgpath( "UserUtils" ) << endl;
  cout << usr::subpkgpath( "UserUtils", "UserUtils/Common" ) << endl;

  cout << usr::separator() << endl
       << "Local globbing test!" << endl;
  {
    vector<string> test_string_list = {
      "./",
      "./*",
      "./*.cc",
      "./*.json",
      "../*/*.cc",
      "../*/*.hpp"
    };

    for( const auto& test : test_string_list ){
      cout << boost::format("Testing [%s]:") % test << endl;

      for( const auto& output : usr::GlobLocal( test ) ){
        cout << boost::format( "\t%s" ) % output << endl;
      }
    }
  }

  usr::fs::path( "mytest" );

  cout << usr::separator() << endl
       << "Directory checking test!" << endl;
  {
    // usr::MakeParent("a/b/c/d");
    // usr::MakeParent("/home/yichen/a/b");
  }

  return 0;
}
