/**
 * @file    ratiocanvas.cc
 * @brief   Testing the default value settings of the ratiocanvas class
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/MathUtils/interface/RooFitExt.hpp"
#include "UserUtils/PlotUtils/interface/CommonXCanvas.hpp"
#else
#include "UserUtils/MathUtils/RooFitExt.hpp"
#include "UserUtils/PlotUtils/CommonXCanvas.hpp"
#endif

#include "TGraph.h"
#include "TH1D.h"

int
main( int argc, char* argv[] )
{
  double x[10]  = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  double y1[10] = {5, 5, 5, 5, 5, 5, 5, 5, 5, 5};
  double y2[10] = {1, 2, 3, 4, 5, 5, 5, 5, 5, 5};
  TGraph g1( 10, x, y1 );
  TGraph g2( 10, x, y2 );
  TGraph g3( 10, x,  x );

  usr::plt::CommonXCanvas c( 3 );
  c.PlotGraph<0>( g1, usr::plt::TrackY(usr::plt::TrackY::both) );
  c.PlotGraph<1>( g2, usr::plt::TrackY(usr::plt::TrackY::both) );
  c.PlotGraph<2>( g3, usr::plt::TrackY(usr::plt::TrackY::both) );

  c.SaveAsPDF("commonxcanvas.pdf");
}
