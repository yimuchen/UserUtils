/*******************************************************************************
*
*  Filename    : BTagChecker.cc
*  Description : Implementation of BTag Checker
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "UserUtils/PhysUtils/interface/BTagChecker.hpp"

using namespace std;

namespace usr {
const double BTagChecker::LOOSEWP_VAL     = 0.5426;
const double BTagChecker::MEDIUMWP_VAL    = 0.8484;
const double BTagChecker::TIGHTWP_VAL     = 0.9535;
const std::string BTagChecker::DISCRIMTAG =
  "pfCombinedInclusiveSecondaryVertexV2BJetTags";

/*******************************************************************************
*   Constructor and destructor
*******************************************************************************/
BTagChecker::BTagChecker( const string& tagger, const string& filename ) :
  _calib( tagger, filename )
{
  const static auto OP_LIST = {BTagEntry::OP_LOOSE,
                               BTagEntry::OP_MEDIUM,
                               BTagEntry::OP_TIGHT};

  for( const auto WP : OP_LIST ){
    _reader_map[WP] = BTagCalibrationReader(
      WP,// operating point
      "central",// central sys type
      {"up", "down"}
      );// other sys types
    _reader_map[WP].load(
      _calib,
      BTagEntry::FLAV_B,
      "comb"
      );
  }
}

BTagChecker::~BTagChecker()
{

}

/*******************************************************************************
*   B-tag working point functions
*******************************************************************************/
bool
BTagChecker::PassLoose( const pat::Jet& jet ) const
{
  return jet.bDiscriminator( DISCRIMTAG ) > LOOSEWP_VAL;
}

bool
BTagChecker::PassMedium( const pat::Jet& jet ) const
{
  return jet.bDiscriminator( DISCRIMTAG ) > MEDIUMWP_VAL;
}

bool
BTagChecker::PassTight( const pat::Jet& jet ) const
{
  return jet.bDiscriminator( DISCRIMTAG ) > TIGHTWP_VAL;
}

/*******************************************************************************
*   Scale factor funtions
*******************************************************************************/
double
BTagChecker::GetLooseScaleFactor( const pat::Jet& jet ) const
{ return scalefactor( jet, BTagEntry::OP_LOOSE, "central" ); }
double
BTagChecker::GetLooseScaleFactorUp( const pat::Jet& jet ) const
{ return scalefactor( jet, BTagEntry::OP_LOOSE, "up" ); }
double
BTagChecker::GetLooseScaleFactorDown( const pat::Jet& jet  ) const
{ return scalefactor( jet, BTagEntry::OP_LOOSE, "down" ); }

/******************************************************************************/

double
BTagChecker::GetMediumScaleFactor( const pat::Jet& jet ) const
{ return scalefactor( jet, BTagEntry::OP_MEDIUM, "central" ); }
double
BTagChecker::GetMediumScaleFactorUp( const pat::Jet& jet ) const
{ return scalefactor( jet, BTagEntry::OP_MEDIUM, "up" ); }
double
BTagChecker::GetMediumScaleFactorDown( const pat::Jet& jet ) const
{ return scalefactor( jet, BTagEntry::OP_MEDIUM, "down" ); }

/******************************************************************************/

double
BTagChecker::GetTightScaleFactor( const pat::Jet& jet ) const
{ return scalefactor( jet, BTagEntry::OP_TIGHT, "central" ); }
double
BTagChecker::GetTightScaleFactorUp( const pat::Jet& jet ) const
{ return scalefactor( jet, BTagEntry::OP_TIGHT, "up" ); }
double
BTagChecker::GetTightScaleFactorDown( const pat::Jet& jet ) const
{ return scalefactor( jet, BTagEntry::OP_TIGHT, "down" ); }

/*******************************************************************************
*   Hidden helper functions
*******************************************************************************/
double
BTagChecker::scalefactor(
  const pat::Jet&           jet,
  BTagEntry::OperatingPoint op,
  const std::string&        sys
  ) const
{
  return _reader_map.at( op ).eval_auto_bounds(
    sys,
    BTagEntry::FLAV_B,
    jet.eta(),
    jet.pt()
    );
}

}/* usr */
