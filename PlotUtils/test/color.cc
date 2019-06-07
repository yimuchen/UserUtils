#ifdef CMSSW_GIT_HASH
#include "UserUtils/PlotUtils/interface/Constants.hpp"
#else
#include "UserUtils/PlotUtils/Constants.hpp"
#endif

#include "TCanvas.h"
#include "TColor.h"
#include "TLine.h"
#include <iostream>

int main( int argc, char* argv[] )
{
  TCanvas c("c","c" );

  std::cout << TColor::DefinedColors() << std::endl;

  TLine l(0.1,0.1,0.9,0.9);
  l.SetLineColor( usr::plt::col::darkgray );
  l.Draw();
  std::cout << l.GetLineColor() << std::endl;

  c.SaveAs("col_test.pdf");

  return 0;
}