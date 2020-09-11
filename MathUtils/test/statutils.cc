/*******************************************************************************
*
*  Filename    : computing_test.cc
*  Description : Unit testing functions for functions defined in Compute.hpp
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/STLUtils.hpp"
#include "UserUtils/MathUtils/interface/Measurement/Format.hpp"
#include "UserUtils/MathUtils/interface/StatisticsUtil.hpp"
#else
#include "UserUtils/Common/STLUtils.hpp"
#include "UserUtils/MathUtils/Measurement/Format.hpp"
#include "UserUtils/MathUtils/StatisticsUtil.hpp"
#endif

#include <cmath>
#include <cstdlib>
#include <iostream>

#include "Math/Functor.h"

using namespace std;
using namespace usr;
int
main( int argc, char* argv[] )
{
  {// Constants checking
    cout << separator() << endl
         << "Confidence level from sigma " << endl
         << fmt::decimal( stat::GetConfidenceLevel( 1 ), 8 )<< endl
         << fmt::decimal( stat::GetConfidenceLevel( 2 ), 8 )<< endl;

    cout << separator() << endl
         << "Sigma value from confidence level" << endl
         << fmt::decimal( stat::GetSigmaInterval( 0.68 ), 8 ) << endl
         << fmt::decimal( stat::GetSigmaInterval( 0.95 ), 8 ) << endl
         << fmt::decimal( stat::GetSigmaInterval( stat::onesigma_level ), 8 ) << endl
         << fmt::decimal( stat::GetSigmaInterval( stat::twosigma_level ), 8 ) << endl;
  }

  {// 1D minor error test
    cout << separator() << endl
         << "1D Minos Error testing" << endl;

    const double mean = 1;
    const double sigma = 2;
    auto nll = [mean, sigma]( const double x )->double {
                 return ( x-mean )*( x-mean )/( 2.0*sigma*sigma );
               };
    ROOT::Math::Functor1D fnll( nll );
    double mymean = 0, errup = 10, errlo = -10;
    usr::stat::MinosError( fnll, mymean, errlo, errup );
    cout << mean << " "<< errup  << " " << errlo << endl;
    cout << nll( mean ) << endl;
    cout << nll( errup ) << endl;
    cout << nll( errlo ) << endl;
  }

  {// 2D minos error test
    cout << separator() << endl
         <<"2D minos error test" << endl;
    const double mean1 = 3;
    const double sig1  = 0.5;
    const double mean2 = 5;
    const double sig2  = 1;

    auto nll = [mean1, sig1, mean2, sig2]( const double* x )->double {
                 return ( x[0]-mean1 )*( x[0]-mean1 )/( 2.*sig1*sig1 )
                        +( x[1]-mean2 )*( x[1]-mean2 )/( 2.*sig2*sig2 );
               };
    auto var = []( const double* x )->double {
                 return x[0] + x[1];
               };
    ROOT::Math::Functor fnll( nll, 2 );
    ROOT::Math::Functor fvar( var, 2 );

    double init[2] = {2.5, 5.5};
    double central, up, down;
    usr::stat::MinosError( fnll, fvar, init, central, up, down );
    cout << central << "  " << up << " " << down << endl;
  }
  {// 2D minos error test
    cout << separator() << endl
         <<"2D minos error test: Poisson sum" << endl;
    const auto Poisson1 = usr::stat::PoissonNLL(30);
    const auto Poisson2 = usr::stat::PoissonNLL(70);

    auto nll = [&Poisson1,&Poisson2]( const double* x )->double {
                return Poisson1( x[0] ) + Poisson2( x[1] );
               };
    auto var = []( const double* x )->double {
                 return x[0] + x[1];
               };
    ROOT::Math::Functor fnll( nll, 2 );
    ROOT::Math::Functor fvar( var, 2 );

    double init[2] = {30, 70};
    double central, up, down;
    usr::stat::MinosError( fnll, fvar, init, central, up, down );
    cout << central << "  " << up << " " << down << endl;
  }

  {// Predefined NLL test
    cout << separator() << endl
         <<"Pre-defined NLL test" << endl;

    double a = 1;
    double b = -100;
    double c = 100;
    usr::stat::MinosError( usr::stat::GaussianNLL( 5, 2 ), a, b, c );
    cout << "Gaussian" << endl;
    cout << fmt::decimal( b, 8 ) << " "
         << fmt::decimal( a, 8 ) << " "
         << fmt::decimal( c, 8 ) <<endl;

    {
      const double passed = 0.01;
      const double total  = 200;
      a = passed/total;
      b = usr::eff_machine_epsilon;
      c = 1-b;
      usr::stat::MinosError( usr::stat::BinomialNLL( passed, total ), a, b, c );
      cout << "Binomial" << endl;
      cout << fmt::decimal( b, 8 ) << " "
           << fmt::decimal( a, 8 ) << " "
           << fmt::decimal( c, 8 ) <<endl;
    }
    {
      const double passed    = 1;
      const double total     = 2000000;
      a    = passed/total;
      b    = usr::eff_machine_epsilon;
      c    = 1-b;
      usr::stat::MinosError( usr::stat::BinomialNLL( passed, total ), a, b, c );
      cout << "Binomial" << endl;
      cout << fmt::decimal( b, 8 ) << " "
           << fmt::decimal( a, 8 ) << " "
           << fmt::decimal( c, 8 ) <<endl;
    }
    {
      double obs = 0.05;
      a = obs;
      b = usr::eff_machine_epsilon;
      c = a + a*a +  1;
      usr::stat::MinosError( usr::stat::PoissonNLL( obs ), a, b, c );
      cout << "Poisson" << endl;
      cout << fmt::decimal( b, 8 ) << " "
           << fmt::decimal( a, 8 ) << " "
           << fmt::decimal( c, 8 ) <<endl;
    }
    {
      double obs = 1e6;
      a = obs;
      b = usr::eff_machine_epsilon;
      c = a + a*a +  1;
      usr::stat::MinosError( usr::stat::PoissonNLL( obs ), a, b, c );
      cout << "Poisson" << endl;
      cout << fmt::decimal( b, 8 ) << " "
           << fmt::decimal( a, 8 ) << " "
           << fmt::decimal( c, 8 ) <<endl;
    }
    {
      double obs = 100;
      a = obs;
      b = usr::eff_machine_epsilon;
      c = a + a*a +  1;
      usr::stat::MinosError( usr::stat::PoissonNLL( obs ), a, b, c );
      cout << "Poisson" << endl;
      cout << fmt::decimal( b, 8 ) << " "
           << fmt::decimal( a, 8 ) << " "
           << fmt::decimal( c, 8 ) <<endl;
    }
  }

  return 0;
}
