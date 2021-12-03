#include "UserUtils/PhysUtils/interface/TrackVertex.hpp"

/**
 * @brief Returning the vertex 4 momentum by summing over all associated tracks,
 *
 * Assumes the resulting particles are massless.
 */
TLorentzVector
TransientVertexP4( const TransientVertex& vertex )
{
  TLorentzVector ans( 0, 0, 0, 0 );

  for( const auto ttrack : vertex.originalTracks() ){
    TLorentzVector tvec;
    tvec.SetPtEtaPhiM( ttrack.track().pt(),
                       ttrack.track().eta(),
                       ttrack.track().phi(),
                       0 );// Asserting the track to be massless for now

    ans += tvec;
  }

  return ans;

}
