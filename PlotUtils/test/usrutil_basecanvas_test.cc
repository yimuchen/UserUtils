/**
 * @file    singlecanvas_test.cc
 * @brief   Testing the default styling of single canvas and saving functions
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */

#include "UserUtils/PlotUtils/interface/Canvas.hpp"
#include "UserUtils/PlotUtils/interface/Pad1D.hpp"

#include "UserUtils/Common/interface/STLUtils.hpp"

#include "TH1D.h"
#include "TRandom3.h"
#include <iostream>

namespace plt = usr::plt;

int
main( int argc, char* argv[] )
{
  { // base functionality test
    plt::Canvas c( 600, 600 );
    auto& pad1 = c.Add<plt::PadBase>( plt::PadSize( 0, 0, 0.5, 0.5 ) );
    auto& pad2 = c.Add<plt::PadBase>( plt::PadSize( 0.4, 0.4, 1, 1 ) );

    TH1D hist( "h", "h", 21, -10, 10 );
    TRandom3 r;

    for( int i = 0; i < 5000; ++i ){
      hist.Fill( r.Gaus( 0, 2 ) );
    }

    pad1.PlotObj( hist, "HIST" );
    pad2.PlotObj( hist, "HIST" );

    c.SaveAsPDF( "testfig/basecanvas_test.pdf" );

    std::cout << c.Width() << " " << c.Height() << std::endl
              << pad1.RelWidth() << " " << pad1.AbsWidth() << std::endl
              << pad2.RelHeight() << " " << pad2.AbsHeight() << std::endl;
  }

  {// Canvas inheritence test
    class A : public plt::Canvas {
    public:
      A() : plt::Canvas(500,500){}
    };

    A a;
    a.Add<plt::PadBase>( plt::PadSize(0,0,1,1));
  }

  return 0;
}
