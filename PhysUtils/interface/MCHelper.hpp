/*******************************************************************************
*
*  Filename    : MCHelper.hpp
*  Description : Helper functions and variables for MC truth matching
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef USERUTILS_PHYSUTILS_MCHELPER_HPP
#define USERUTILS_PHYSUTILS_MCHELPER_HPP

#include "DataFormats/Candidate/interface/Candidate.h"

namespace usr {

/*-----------------------------------------------------------------------------
 *  PDF ID definition
   --------------------------------------------------------------------------*/
enum PID
{
  DOWN_QUARK_ID    = 1,
  UP_QUARK_ID      = 2,
  STRANGE_QUARK_ID = 3,
  CHARM_QUARK_ID   = 4,
  BOTTOM_QUARK_ID  = 5,
  TOP_QUARK_ID     = 6,

  ELEC_ID    = 11,
  ELECNU_ID  = 12,
  MUON_ID    = 13,
  MUONNEU_ID = 14,
  TAU_ID     = 15,
  TAUNEU_ID  = 16,

  GLUON_ID       = 21,
  GAMMA_ID       = 22,
  Z_BOSON_ID     = 23,
  W_BOSON_ID     = 24,
  HIGGS_BOSON_ID = 25
};

/*******************************************************************************
*   * GetDirectMother( x , F ),
*   For an input particle x, it will trasverse the topology towards the parent
*   side, until
*     1. the parent particle has the target flavour F, in which case it returns
*        the pointer to the parent particle. Note that the absolue value will
*        be taken for the for the flavour.
*     2. The parent particle has a flavour different to x, in which case a null
*        pointer is return.
*   This function help find direct parent of a particle while avoid the
*   possibility of final state radiations,
*
*   Ex: for the topology
*   F->(x+g)->(x'+gg)->(x''+ggg)
*   The three function calls
*     - GetDirectMother( x, F )
*     - GetDirectMother( x', F )
*     - GetDirectMother( x'', F )
*   should all return the same pointer.
*
*   Topologies in MiniAOD also have some times have the strange topologies like:
*   x -> x' -> x'' which could be avoided by this function.
*******************************************************************************/
const reco::Candidate* GetDirectMother( const reco::Candidate*, int );

/*******************************************************************************
*   * GetDaughter( x, F)
*   Return the pointer to the daughter particle of x whose flavour is F.
*
*   For the decay x->F , and the strange single object decay F->F'->F''->xy ...
*   this function will return the last of the decay chain (F'') in our case.
*******************************************************************************/
const reco::Candidate* GetDaughter( const reco::Candidate*, int );

}/* usr */


#endif/* end of include guard: USERUTILS_PHYSUTILS_MCHELPER_HPP */
