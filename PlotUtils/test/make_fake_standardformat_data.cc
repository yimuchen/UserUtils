#include "TFile.h"
#include "TH1D.h"
#include "TRandom3.h"
#include "TTree.h"

int main( int argc, char** argv )
{
  TH1D JetPt( "JetPt", "", 40, 0, 400 );
  TH1D JetPtLepUp( "JetPtLepUp", "", 40, 0, 400 );
  TH1D JetPtLepDown( "JetPtLepDown", "", 40, 0, 400 );
  TH1D JetPtPileupUp( "JetPtPileupUp", "", 40, 0, 400 );
  TH1D JetPtPileupDown( "JetPtPileupDown", "", 40, 0, 400 );

  TRandom3 rand;

  // Fake data
  {
    TFile* file = TFile::Open( "data1.root", "RECREATE" );
    JetPt.Reset();

    for( unsigned i = 0; i < 5000; ++i ){
      JetPt.Fill( rand.Exp( 150 ) );
    }

    for( unsigned i = 0; i < 100; ++i ){
      JetPt.Fill( rand.Gaus( 100, 5 ) );
    }

    JetPt.Write();
    JetPt.SetDirectory( 0 );
    file->Close();
  }

  {
    TFile* file = TFile::Open( "data2.root", "RECREATE" );
    JetPt.Reset();

    for( unsigned i = 0; i < 10000; ++i ){
      JetPt.Fill( rand.Exp( 150 ) );
    }

    for( unsigned i = 0; i < 200; ++i ){
      JetPt.Fill( rand.Gaus( 100, 5 ) );
    }

    JetPt.Write();
    JetPt.SetDirectory( 0 );
    file->Close();
  }


  // Background stuff
  {
    TFile* file = TFile::Open( "YJets_10HT.root", "RECREATE" );
    JetPt.Reset();
    JetPtLepUp.Reset();
    JetPtLepDown.Reset();
    JetPtPileupUp.Reset();
    JetPtPileupDown.Reset();

    double total_event = 500000;
    TTree tree( "Count", "" );
    tree.Branch( "OriginalEvents", &total_event );
    tree.Fill();

    for( unsigned i = 0; i < 500000; ++i ){
      JetPt.Fill( rand.Exp( 100 ), rand.Gaus( 1, 0.1 ) );
      JetPtLepUp.Fill( rand.Exp( 100 ), rand.Gaus( 0.95, 0.1 ) );
      JetPtLepDown.Fill( rand.Exp( 100 ), rand.Gaus( 1.05, 0.1 ) );
      JetPtPileupUp.Fill( rand.Exp( 100.1 ), rand.Gaus( 1, 0.1 ) );
      JetPtPileupDown.Fill( rand.Exp( 99.9 ), rand.Gaus( 1, 0.1 ) );
    }

    JetPt.Write();
    JetPtLepUp.Write();
    JetPtLepDown.Write();
    JetPtPileupUp.Write();
    JetPtPileupDown.Write();
    tree.Write();
    JetPt.SetDirectory( 0 );
    file->Close();
  }

  {
    TFile* file = TFile::Open( "YJets_20HT.root", "RECREATE" );
    JetPt.Reset();
    JetPtLepUp.Reset();
    JetPtLepDown.Reset();
    JetPtPileupUp.Reset();
    JetPtPileupDown.Reset();

    double total_event = 500000;
    TTree tree( "Count", "" );
    tree.Branch( "OriginalEvents", &total_event );
    tree.Fill();

    for( unsigned i = 0; i < 500000; ++i ){
      JetPt.Fill( rand.Exp( 200 ) );
      JetPtLepUp.Fill( rand.Exp( 200 ), rand.Gaus( 0.95, 0.1 ) );
      JetPtLepDown.Fill( rand.Exp( 200 ), rand.Gaus( 1.05, 0.1 ) );
      JetPtPileupUp.Fill( rand.Exp( 200.2 ), rand.Gaus( 1, 0.1 ) );
      JetPtPileupDown.Fill( rand.Exp( 200.1 ), rand.Gaus( 1, 0.1 ) );
    }

    JetPt.Write();
    JetPtLepUp.Write();
    JetPtLepDown.Write();
    JetPtPileupUp.Write();
    JetPtPileupDown.Write();
    tree.Write();
    JetPt.SetDirectory( 0 );
    file->Close();
  }

  {
    TFile* file = TFile::Open( "rrbar.root", "RECREATE" );
    JetPt.Reset();
    JetPtLepUp.Reset();
    JetPtLepDown.Reset();
    JetPtPileupUp.Reset();
    JetPtPileupDown.Reset();

    double total_event = 500000;
    TTree tree( "Count", "" );
    tree.Branch( "OriginalEvents", &total_event );
    tree.Fill();

    for( unsigned i = 0; i < 50000; ++i ){
      JetPt.Fill( rand.Exp( 600  ) );
      JetPtLepUp.Fill( rand.Exp( 600 ), rand.Gaus( 0.95, 0.1 ) );
      JetPtLepDown.Fill( rand.Exp( 600 ), rand.Gaus( 1.05, 0.1 ) );
      JetPtPileupUp.Fill( rand.Exp( 600.1 ), rand.Gaus( 1, 0.1 ) );
      JetPtPileupDown.Fill( rand.Exp( 600.2 ), rand.Gaus( 1, 0.1 ) );
    }

    JetPt.Write();
    JetPtLepUp.Write();
    JetPtLepDown.Write();
    JetPtPileupUp.Write();
    JetPtPileupDown.Write();
    tree.Write();
    JetPt.SetDirectory( 0 );
    file->Close();
  }

  // Signal Stuff
  {
    TFile* file = TFile::Open( "sig.root", "RECREATE" );
    JetPt.Reset();

    double total_event = 50000;
    TTree tree( "Count", "" );
    tree.Branch( "OriginalEvents", &total_event );
    tree.Fill();

    for( unsigned i = 0; i < 5000; ++i ){
      JetPt.Fill( rand.Gaus( 95, 5  ) );
    }

    JetPt.Write();
    tree.Write();
    JetPt.SetDirectory( 0 );
    file->Close();
  }

}
