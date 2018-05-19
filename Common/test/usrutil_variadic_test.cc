/*******************************************************************************
*
*  Filename    : variadic_test.cc
*  Description : Testing variadic template functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "UserUtils/Common/interface/STLUtils.hpp"

#include <vector>
#include <iostream>

using namespace std;

int
main( int argc, char const* argv[] )
{
  vector<int> test = usr::MakeVector<int>(1,2,3,4,5);
  vector<int> test1 = usr::MakeVector<int>(1);
  vector<int> test2 = usr::MakeVector<int>(1,2);
  vector<int> test3 = usr::MakeVector<int>(1,2);

  cout << test.size() << endl;
  for( auto x : test ){
    cout << x << endl;
  }


  return 0;
}
