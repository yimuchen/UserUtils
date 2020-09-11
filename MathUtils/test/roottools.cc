/**
 * @file    roofit_templatefit.cc
 * @brief   Unit test for template fit function
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/MathUtils/interface/RootMathTools/DefaultEngines.hpp"
#else
#include "UserUtils/MathUtils/RootMathTools/DefaultEngines.hpp"
#endif

#include "Math/Functor.h"

int
main()
{
  usr::DefaultSolver1D solver;

  auto lambdaf = []( double x ){
                   return x*x-0.5;
                 };
  ROOT::Math::Functor1D f( lambdaf );

  std::cout << solver.SolveF( f, -1, 1 ) << std::endl;
  std::cout << solver.SolveForY( f, 0.5, 0, 1 ) << std::endl;
}
