/**
 * @file    unit_test.cc
 * @brief   Test for unit conversion
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/STLUtils.hpp"
#include "UserUtils/PlotUtils/interface/Constants.hpp"
#else
#include "UserUtils/Common/STLUtils.hpp"
#include "UserUtils/PlotUtils/Constants.hpp"
#endif

#include <iostream>
using namespace std;

int main( int argc, char* argv[] )
{
  cout << usr::separator() << endl
       << "Length conversion and constants" << endl;
  {
    cout << usr::plt::len::a4paperwidth() << endl
         << usr::plt::len::a4textheight_default() << endl;

    cout << usr::plt::len::a4textwidth_default() << endl
         << usr::plt::len::a4textheight_default() << endl;
  }

  cout << usr::separator() << endl
       << "Color int value and string conversion" << endl;
  {
    std::cout << usr::plt::col::gray << std::endl;
    auto test_col = []( const std::string& x ){
                      usr::fout( "[%s] -> %d\n", x, usr::plt::col::color( x ) );
                    };
    test_col( "#abcdef" );
    test_col( "#AAA" );
    test_col( "#FFF" );
    test_col( "dark gray" );
    test_col( "darkgray" );
    test_col( "darkGray" );
    test_col( "ddarkGray" );
  }

  return 0;
}
