/**
 * @file RootUtils_Misc.cc
 * @author Yi-Mu "Enoch" Chen
 * @brief Implementation of the ROOT object helper functions
 *
 */

#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"
#else
#include "UserUtils/Common/STLUtils/StringUtils.hpp"
#endif


#include <string>
#include <vector>

#include "TChain.h"
#include "TH1D.h"
#include "TMatrixD.h"

namespace usr {

/**
 * @brief A nicer interface for initializing a TChain using std containers.
 */
TChain*
MakeTChain( const std::string&              title,
            const std::vector<std::string>& inputfiles )
{
  TChain* tree = new TChain( title.c_str() );

  for( const auto& file : inputfiles ){
    if( !file.empty() ){
      tree->Add( file.c_str() );
    }
  }

  return tree;
}

/**
 * @brief A nicer interface of making a TH1D using `std::vector`s
 */
TH1D
MakeHist( const std::string&         name,
          const std::string&         title,
          const std::vector<double>& bins )
{
  return TH1D( name.c_str(), title.c_str(), bins.size() -1, bins.data() );
}

std::string
str( const TMatrixD& m, const unsigned format )
{
  if( format == 0 ){
    std::string ans = "\\begin{pmatrix}\n";

    for( int i = 0; i < m.GetNrows(); ++i ){
      for( int j = 0; j < m.GetNcols(); ++j ){
        ans += usr::fstr( "%10.8lf ", m( i, j ) );
        if( j == m.GetNcols() - 1 && i != m.GetNrows() -1 ){
          ans += "\\\\\n";
        } else {
          ans += " & ";
        }
      }
    }

    ans += "\n\\end{pmatrix}";

    return ans;
  } else {
    return "";
  }
}

}
