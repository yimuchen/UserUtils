/*******************************************************************************
*
*  Filename    : MCHelper.cc
*  Description : Implementation for the MC helper functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/

#include "DataFormats/Candidate/interface/Candidate.h"

#include <queue>
using namespace std;

namespace usr {

const reco::Candidate*
GetDirectMother( const reco::Candidate* x, int target_ID )
{
  queue<const reco::Candidate*> bfs_queue;
  bfs_queue.push( x );

  while( !bfs_queue.empty() ){
    const reco::Candidate* temp = bfs_queue.front();
    bfs_queue.pop();
    if( abs( temp->pdgId() ) == abs( target_ID ) ){
      return temp;
    }

    for( unsigned i = 0; i < temp->numberOfMothers(); ++i ){
      if( temp->mother( i )->pdgId() == temp->pdgId() ){
        bfs_queue.push( temp->mother( i ) );
      } else if( abs( temp->mother( i )->pdgId() ) == abs( target_ID ) ){
        return temp->mother( i );
      }
    }
  }

  return NULL;
}

/******************************************************************************/

const reco::Candidate*
GetDaughter( const reco::Candidate* x, int target_ID )
{
  queue<const reco::Candidate*> bfs_queue;
  bfs_queue.push( x );

  while( !bfs_queue.empty() ){
    const reco::Candidate* temp = bfs_queue.front();
    bfs_queue.pop();
    if( abs( temp->pdgId() ) == abs( target_ID ) ){
      // Moving to bottom of single decay chain
      while( temp->numberOfDaughters() == 1 ){
        temp = temp->daughter( 0 );
      }

      return temp;
    }

    for( unsigned i = 0; i < temp->numberOfDaughters(); ++i ){
      bfs_queue.push( temp->daughter( i ) );
    }
  }

  return NULL;
}

}/* usr */
