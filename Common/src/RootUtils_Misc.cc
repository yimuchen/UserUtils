/**
 * @file RootUtils_Misc.cc
 * @author Yi-Mu "Enoch" Chen
 * @brief Implementation of the ROOT object helper functions
 *
 */

#include <string>
#include <vector>

#include "TChain.h"
#include "TH1D.h"

namespace usr {

TChain*
MakeTChain( const std::string&              title,
            const std::vector<std::string>& inputfiles )
{
  TChain* tree = new TChain( title.c_str() );

  for( const auto file : inputfiles ){
    if( !file.empty() ){
      tree->Add( file.c_str() );
    }
  }

  return tree;
}

TH1D
MakeHist( const std::string&    name,
          const std::string&    title,
          const std::vector<double>& bins )
{
  return TH1D( name.c_str(), title.c_str(), bins.size() -1, bins.data() );
}

}
