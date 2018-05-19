/*******************************************************************************
*
*  Filename    : ObjectExtendedMomentum.hpp
*  Description : Implementation of template functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifdef USERUTILS_PHYSUTILS_OBJECTEXTENDEDVARS_HPP

#include "TLorentzVector.h"
#include <string>

namespace usr {

template<class T>
void
AddLorentzVector( T& obj, const TLorentzVector& vec, const std::string& name )
{
  static const bool overwrite = true;
  obj.addUserFloat( name+"pt",  vec.Pt(),     overwrite );
  obj.addUserFloat( name+"eta", vec.Eta(),    overwrite );
  obj.addUserFloat( name+"phi", vec.Phi(),    overwrite );
  obj.addUserFloat( name+"e",   vec.Energy(), overwrite );
}

/*----------------------------------------------------------------------------*/

template<class T>
bool
HasLorentzVector( const T& obj, const std::string& name )
{
  if( obj.hasUserFloat( name+"pt" )
      && obj.hasUserFloat( name+"eta" )
      && obj.hasUserFloat( name+"phi" )
      && obj.hasUserFloat( name+"e" ) ){
    return true;
  } else {
    return false;
  }

}

/*----------------------------------------------------------------------------*/

template<class T>
TLorentzVector
GetLorentzVector( const T& obj, const std::string& name )
{
  TLorentzVector ans;
  double pt;
  double eta;
  double phi;
  double e;
  if( HasLorentzVector( obj, name ) ){
    pt  = obj.userFloat( name+"pt" );
    eta = obj.userFloat( name+"eta" );
    phi = obj.userFloat( name+"phi" );
    e   = obj.userFloat( name+"e" );
  } else {
    pt  = obj.pt();
    eta = obj.eta();
    phi = obj.phi();
    e   = obj.energy();
  }
  ans.SetPtEtaPhiE( pt, eta, phi, e );
  return ans;
}

}/* usr */

#endif
