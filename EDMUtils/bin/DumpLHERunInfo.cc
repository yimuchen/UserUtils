/*******************************************************************************
*
*  Filename    : DumpLHEContent.cc
*  Description : Simple script for dumping LHE config file content
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/Run.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "TFile.h"

#include <iostream>
using namespace std;

int
main( int argc, char* argv[] )
{
  fwlite::Run run( TFile::Open( argv[1] ) );
  fwlite::Handle<LHERunInfoProduct> lhehandle;

  lhehandle.getByLabel( run, "externalLHEProducer" );

  for( auto iter = lhehandle.ref().headers_begin();
       iter != lhehandle.ref().headers_end(); iter++ ){
    const auto& lines = iter->lines();

    for( const auto line : lines ){
      std::cout << "[" << iter->tag()  << "] " << line << flush;
    }
  }
}
