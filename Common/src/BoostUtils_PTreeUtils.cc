/*******************************************************************************
*
*  Filename    : BoostUtils_PTreeUtils.cc
*  Description : Implementation of non template functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*
*******************************************************************************/
#include "UserUtils/Common/interface/BoostUtils/PTreeUtils.hpp"
#include "UserUtils/MathUtils/interface/Measurement/Measurement.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>
#include <queue>

namespace usr {

/*-----------------------------------------------------------------------------
 *  Construction from Json files.
   --------------------------------------------------------------------------*/
pt::ptree
FromJsonFile( const std::string& filename )
{
  pt::ptree ans;
  pt::read_json( filename, ans );
  return ans;
}

/*----------------------------------------------------------------------------*/

pt::ptree
FromJsonFiles( const std::vector<std::string>& filelist )
{
  pt::ptree ans;

  for( const auto& file : filelist ){
    pt::ptree tmp;
    pt::read_json( file, tmp );
    ans = MergeTrees( ans, tmp );
  }

  return ans;
}

/*-----------------------------------------------------------------------------
 *  Merging
   --------------------------------------------------------------------------*/
pt::ptree
MergeTrees( const pt::ptree& first, const pt::ptree& second )
{
  // From https://paste.tbee-clan.de/TX2Vm
  pt::ptree merged = first;

  // Traversing over second tree using BFS
  std::queue<pt::ptree> qvalues;
  qvalues.push( second );
  std::queue<std::string> qkeys;// Keep track of keys for pt::ptree query

  while( !qvalues.empty() ){
    pt::ptree current = qvalues.front();
    qvalues.pop();
    std::string keychain = "";
    if( !qkeys.empty() ){
      keychain = qkeys.front();
      qkeys.pop();
    }

    BOOST_FOREACH( const pt::ptree::value_type& child, current ){
      if( child.second.size() == 0 ){// Leaf
        // No "." for first level entries
        const std::string s =
          keychain == "" ? child.first.data() :
          keychain + "." + child.first.data();
        merged.put( s, child.second.data() );
      } else {// Subtree
        // Put keys (identifiers of subtrees) and all of its parents
        // aside for later iteration. Keys on first level have no parents
        const std::string newkey
          = keychain == "" ? child.first.data() :
            keychain + "." + child.first.data();
        qkeys.push( newkey );
        qvalues.push( child.second );
      }
    }// -- End of BOOST_FOREACH
  }// --- End of while

  return merged;
}

/*----------------------------------------------------------------------------*/

void
PrintPTree( const pt::ptree& tree, unsigned level )
{
  for( const auto& it : tree ){
    for( unsigned i = 0; i < level; ++i ){
      std::cout << "\t" << std::flush;
    }

    std::cout << it.first << ": "
              << it.second.get_value<std::string>() << std::endl;
    PrintPTree( it.second, level + 1 );
  }
}

/*-----------------------------------------------------------------------------
 *  Basic query functions
   --------------------------------------------------------------------------*/
std::string
MakeQueryString( const std::vector<std::string>& list )
{
  return boost::join( list, "." );
}

/*----------------------------------------------------------------------------*/

bool
CheckQuery( const pt::ptree& tree, const std::string& query )
{
  try {
    tree.get_child(query);
    return true;
  } catch( boost::exception& e ){
    return false;
  }
}

/*----------------------------------------------------------------------------*/

pt::ptree
GetSubTree( const pt::ptree& tree, const std::string& query )
{
  return tree.get_child( query );
}

}/* usr */
