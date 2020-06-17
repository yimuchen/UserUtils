/*******************************************************************************
*
*  Filename    : ptree_test.cc
*  Description : Unit testing for pt::ptree utility functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/BoostUtils/PTreeUtils.hpp"
#include "UserUtils/Common/interface/STLUtils.hpp"
#else
#include "UserUtils/Common/BoostUtils/PTreeUtils.hpp"
#include "UserUtils/Common/STLUtils.hpp"
#endif

#include <iostream>

using namespace std;

/******************************************************************************/

int
main( int argc, char const* argv[] )
{
  cout << usr::separator() << endl
       << "test.json input" << endl;
  usr::pt::ptree tree = usr::FromJsonFile( "test.json" );

  usr::PrintPTree( tree );

  cout << usr::separator() << endl;

  cout << usr::GetSingle<string>( tree, "Static1" ) << endl;

  cout << usr::separator() << endl;

  for( const auto x : usr::GetList<int>( tree, "Instance1.var1" ) ){
    cout << x << endl;
  }

  cout << usr::separator() << endl;

  usr::pt::ptree subtree = usr::GetSubTree( tree, "Instance2" );
  usr::PrintPTree( subtree );

  cout << usr::separator() << endl
       << "name1.json input" << endl;

  usr::pt::ptree tree2 = usr::FromJsonFile( "name1.json" );
  subtree = usr::GetSubTree( tree2, "channel.MuonSignal" );
  cout << usr::CheckQuery( tree2, "channel.MuonSignal" ) << endl;
  cout << usr::CheckQuery( tree2, "channel.MuonSignal.Root Name" ) << endl;
  cout << usr::CheckQuery( tree2, "channel.Garbage" ) << endl;
  cout << usr::CheckQuery( subtree, "Root Name" ) << endl;
  usr::PrintPTree( subtree );


  std::cout <<  "Merging test" << std::endl;
  {
    usr::pt::ptree t1 = usr::FromJsonFile( "name1.json" );
    usr::pt::ptree t2 = usr::FromJsonFile( "name2.json" );

    usr::pt::ptree m1 = usr::MergeTrees( t1, t2 );
    usr::pt::ptree m2 = usr::FromJsonFiles( {"name1.json", "name2.json"} );

    usr::PrintPTree( m1 );
    usr::PrintPTree( m2 );
  }

  std::cout <<  "Merging test 2" << std::endl;
  {
    usr::pt::ptree t1 = usr::FromJsonFile( "list1.json" );
    usr::pt::ptree t2 = usr::FromJsonFile( "list2.json" );

    usr::pt::ptree m1 = usr::MergeTrees( t1, t2 );
    usr::pt::ptree m2 = usr::FromJsonFiles( {"list1.json", "list2.json"} );

    usr::PrintPTree( m1 );
    usr::PrintPTree( m2 );
  }


  return 0;
}
