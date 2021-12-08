#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "TFile.h"
#include <iostream>
#include <vector>

int
main( int argc, char*argv[] )
{
  std::cout << argv[1];
  fwlite::Event                                   event( TFile::Open( argv[1] ) );
  fwlite::Handle<std::vector<PileupSummaryInfo> > puHandle;

  for( event.toBegin(); !event.atEnd(); ++event ){
    puHandle.getByLabel( event, "mixData" );
    std::cout << std::endl << puHandle.ref().size() << " ";

    for( const auto& pu : puHandle.ref() ){
      if( pu.getBunchCrossing() != 0 ){ continue; }
      std::cout << "(" << pu.getBunchCrossing() << "," <<
      pu.getPU_zpositions().size() << ") ";
    }
  }
}
