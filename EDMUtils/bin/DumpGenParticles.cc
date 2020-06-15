#include "UserUtils/Common/interface/ArgumentExtender.hpp"
#include "UserUtils/Common/interface/STLUtils/OStreamUtils.hpp"
#include "UserUtils/Common/interface/STLUtils/VectorUtils.hpp"
#include "UserUtils/EDMUtils/interface/ParseEDM.hpp"

#include "DataFormats/FWLite/interface/ChainEvent.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "TFile.h"

int
main( int argc, char* argv[] )
{
  usr::po::options_description desc(
    "Dumping the the generator particle dump to generation topology debugging" );
  desc.add_options()
    ( "input,i", usr::po::multivalue<std::string>(),
    "Input EDM Files (useful if you are not sure which file contains the event "
    "of interest)" )
    ( "tag", usr::po::defvalue<std::string>( "prunedGenParticles" ),
    "edm::InputTag of container of the Gen Particle list in the EDM file" )
    ( "event", usr::po::value<std::string>(),
    "event number in the format of \"LumiID:EventID\" or \"EventID\""
    "(the Run is always 1)" )
    ( "eventidx", usr::po::value<int>(),
    "The event to display as indexed in the file." )
    ( "status", usr::po::defmultivalue<int>( {} ),
    "Limit the output to particles of certain status codes. "
    "Leave empty to omit status selection." )
    ( "pdgid", usr::po::defmultivalue<int>( {} ),
    "Limit the output to particles of certain particle ids (signed). "
    "Leave emtpy to omit PDGID selection." )
  ;

  usr::ArgumentExtender args;
  args.AddOptions( desc );
  args.ParseOptions( argc, argv );

  // Setting up the variables
  const std::vector<std::string> input = args.ArgList<std::string>( "input" );
  const std::vector<int> status_list   = args.ArgList<int>( "status" );
  const std::vector<int> pdgid_list    = args.ArgList<int>( "pdgid" );
  const std::string tag                = args.Arg<std::string>( "tag" );
  const unsigned event_idx             = args.ArgOpt<int>( "eventidx", 0 );
  const edm::EventID event_id          = ParseEvent(
    args.ArgOpt<std::string>( "event", "" ) );

  fwlite::ChainEvent evt( input );
  fwlite::Handle<std::vector<reco::GenParticle> > genHandle;
  std::vector<const reco::GenParticle*> ptrList;


  // Moving to the event
  if( event_id.run() != 0 ){
    evt.to( event_id );
  } else {
    evt.to( event_idx );
  }


  usr::fout( "EventID [%u:%u:%u]\n"
           , evt.id().run()
           , evt.id().luminosityBlock()
           , evt.id().event() );

  genHandle.getByLabel( evt, tag.c_str() );

  // Making the pointer copy of the list for daughter matching
  for( const auto& gen : genHandle.ref() ){
    // Making sure this is the pointer of the object
    ptrList.push_back( &gen );
  }

  auto GetIndex = [&ptrList]( const reco::Candidate* ptr )->int {
                    auto it = std::find( ptrList.begin(), ptrList.end(), ptr );
                    return it - ptrList.begin();
                  };

  usr::fout(
    "%5s | "
    "%10s %5s | "
    "%10s %10s %10s %6s | "
    "%8s %8s %8s | "
    "[%4s]%10s( N) | "
    "%5s | [%4s]%10s [%4s]%10s\n"
           , "IDX"
           , "PDGID", "STAT"
           , "MASS", "PT", "ETA", "PHI"
           , "VX" , "VY", "VZ"
           , "IDX", "Mother"
           , "NDau"
           , "IDX", "Daughter0"
           , "IDX", "Daughter1" );


  for( unsigned gen_index = 0; gen_index < genHandle.ref().size(); ++gen_index ){
    const auto gen = genHandle.ref().at( gen_index );

    // Skipping particles
    if( status_list.size() > 0 && !usr::FindValue( status_list, gen.status() ) ){
      continue;
    }
    if( pdgid_list.size() > 0 && !usr::FindValue( pdgid_list, gen.pdgId() ) ){
      continue;
    }

    const auto daughter0
      = gen.numberOfDaughters() > 0 ? gen.daughter( 0 ) : nullptr;
    const auto daughter1
      = gen.numberOfDaughters() > 1 ? gen.daughter( 1 ) : nullptr;
    const auto mother
      = gen.numberOfMothers() > 0 ? gen.mother() : nullptr;

    usr::fout(
      "%5u | "
      "%10d %5d | "
      "%10.2lf %10.2lf %10.2lf %6.2lf | "
      "%8.2lf %8.2lf %8.2lf |"
      "[%4d]%10d(%2d) | "
      "%5d | [%4d]%10d [%4d]%10d\n"
             , gen_index
             , gen.pdgId(), gen.status()
             , gen.mass(), gen.pt(), gen.eta(), gen.phi()
             , gen.vertex().x(), gen.vertex().y(), gen.vertex().z()
             , mother ? GetIndex( mother ) : -1
             , mother ? mother->pdgId() : 0
             , gen.numberOfMothers()
             , gen.numberOfDaughters()
             , daughter0 ? GetIndex( daughter0 ) : -1
             , daughter0 ? daughter0->pdgId() : 0
             , daughter1 ? GetIndex( daughter1 ) : -1
             , daughter1 ? daughter1->pdgId() : 0
      );
  }

  return 0;
}
