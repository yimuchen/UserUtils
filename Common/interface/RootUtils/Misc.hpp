/**
 * @file Misc.hpp
 * @author Yi-Mu "Enoch" Chen
 * @brief Miscellaneous root object helper functions.
 */

#ifndef USERUTILS_COMMON_ROOTUTILS_MISC_HPP
#define USERUTILS_COMMON_ROOTUTILS_MISC_HPP

#include <string>
#include <vector>

#include "TChain.h"
#include "TH1D.h"
#include "TMatrixD.h"

namespace usr {

extern TChain* MakeTChain( const std::string&              title,
                           const std::vector<std::string>& inputfiles );

extern TH1D MakeHist( const std::string&         name,
                      const std::string&         title,
                      const std::vector<double>& bins );

extern std::string str( const TMatrixD&, const unsigned format=0 );

}

#endif
