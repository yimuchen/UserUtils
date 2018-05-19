/*******************************************************************************
*
*  Filename    : ObjectExtendedVars.cc
*  Description : Extended variables for physical objects
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef USERUTILS_PHYSUTILS_OBJECTEXTENDEDVARS_HPP
#define USERUTILS_PHYSUTILS_OBJECTEXTENDEDVARS_HPP

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

#include "TLorentzVector.h"

namespace usr {

namespace mu { // muon functions, implemented in file MuonVar

/*******************************************************************************
*   Muon Particle flow isolation, ( with recommended working point values )
*   For definition, see the POG group :
*   https://twiki.cern.ch/twiki/bin/viewauth/CMS/SWGuideMuonIdRun2
*******************************************************************************/
extern const double PFISO_TIGHT;
extern const double PFISO_LOOSE;
extern double PfIso( const pat::Muon& );

/*******************************************************************************
*   Muon track objects isolation, ( with recommended working point values )
*   For definition, see the POG group :
*   https://twiki.cern.ch/twiki/bin/viewauth/CMS/SWGuideMuonIdRun2
*******************************************************************************/
extern const double TRACKISO_TIGHT;
extern const double TRACKISO_LOOSE;
extern double TrackIso( const pat::Muon& );

} /* mu */

/*******************************************************************************
*  Particle Flow MiniIsolation:
*  Code reference :
*    github.com/manuelfs/CfANtupler/
*    blob/master/minicfa/interface/miniAdHocNTupler.h#L54
*******************************************************************************/
extern double PFMiniIsolation(
  const edm::Handle<pat::PackedCandidateCollection>& pfcands,
  const reco::Candidate*                             ptcl,
  const double                                       r_iso_min,
  const double                                       r_iso_max,
  const double                                       kt_scale,
  const bool                                         charged_only
  );// in src/PFMiniIsolation.cc



/*-----------------------------------------------------------------------------
 * Storing and accessing additional Lorentz Vectors stored in a
 * pat::Object. Useful for the smearing of kinematic distributions.
 *
 * Lorentz Vectors are represented using the ROOT TLorentzVector class.
 * A string is used to identify the vector of interest.
 * In the case the the GetLorentzVector function doesn't not find the
 * corresponding vector, the original four momentumf of the pat::Object would
 * be returned.
 *
   --------------------------------------------------------------------------*/
template<typename T>
void
AddLorentzVector( T& obj, const TLorentzVector& vec, const std::string& name );

/*----------------------------------------------------------------------------*/

template<typename T>
bool
HasLorentzVector( const T& obj, const std::string& name );

/*----------------------------------------------------------------------------*/

template<class T>
TLorentzVector
GetLorentzVector( const T& obj, const std::string& name );

/*----------------------------------------------------------------------------*/

}/* usr */


#include "UserUtils/PhysUtils/src/ExtendedMomentum.ipp"

#endif/* end of include guard: USERUTILS_PHYSUTILS_OBJECTEXTENDEDVARS_HPP */
