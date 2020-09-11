/**
 * @file    roofit_templatefit.cc
 * @brief   Unit test for template fit function
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/MathUtils/interface/RootMathTools/TemplateFit.hpp"
#else
#include "UserUtils/MathUtils/RootMathTools/TemplateFit.hpp"
#endif

#include "TH1D.h"
#include "TRandom3.h"

int
main()
{
  TRandom3 rand;

  TH1D h1( "h1", "", 100, -10, 10 );
  TH1D h2( "h2", "", 100, -10, 10 );
  TH1D h3( "h3", "", 100, -10, 10 );

  for( unsigned i = 0; i < 1000000; ++i ){
    h1.Fill( rand.Gaus( -2, 1 ) );
    h2.Fill( rand.Gaus( 2, 1.5 ) );
    if( i% 100 == 0 ){
      h3.Fill( rand.Gaus( -2, 1 ) );
    }
    if( i% 200 == 0 ){
      h3.Fill( rand.Gaus( 2, 1.5 ) );
    }
  }

  const auto fit_total = usr::TemplateFit::SimpleFit( &h3, {&h1, &h2}, false );
  const auto fit_frac  = usr::TemplateFit::SimpleFit( &h3, {&h1, &h2}, true );


  std::cout << fit_total.size() << std::endl;
  for( const auto p : fit_total ){
    std::cout << p.CentralValue() << " "
              << p.AbsUpperError() << " "
              << p.AbsLowerError() << std::endl ;
  }
  std::cout << std::endl;

  std::cout << fit_frac.size() << std::endl;
  for( const auto p : fit_frac ){
    std::cout << p.CentralValue() << " "
              << p.AbsUpperError() << " "
              << p.AbsLowerError() << std::endl ;
  }
  std::cout << std::endl;

  return 0;
}
