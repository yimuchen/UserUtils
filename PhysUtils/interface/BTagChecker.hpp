/*******************************************************************************
*
*  Filename    : BTagChecker.hpp
*  Description : Class for checking btag
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*  In complience with recommendations page:
*     https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation76X
*  Specialized for pfCombinedInclusiveSecondaryVertexV2BJetTags
*
*******************************************************************************/
#ifndef TSTARANALYSIS_BASELINESELECTOR_BTAGCHECKER_HPP
#define TSTARANALYSIS_BASELINESELECTOR_BTAGCHECKER_HPP

#include "CondFormats/BTauObjects/interface/BTagCalibration.h"
#include "CondTools/BTau/interface/BTagCalibrationReader.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include <string>

namespace usr {

class BTagChecker
{
public:
  static const double      LOOSEWP_VAL;
  static const double      MEDIUMWP_VAL;
  static const double      TIGHTWP_VAL;
  static const std::string DISCRIMTAG;

  BTagChecker( const std::string& tagger, const std::string& filename );
  virtual
  ~BTagChecker ();

  bool PassLoose( const pat::Jet& ) const;
  bool PassMedium( const pat::Jet& ) const;
  bool PassTight( const pat::Jet& ) const;

  double GetLooseScaleFactor( const pat::Jet& ) const;
  double GetLooseScaleFactorUp( const pat::Jet& ) const;
  double GetLooseScaleFactorDown( const pat::Jet& ) const;

  double GetMediumScaleFactor( const pat::Jet& ) const;
  double GetMediumScaleFactorUp( const pat::Jet& ) const;
  double GetMediumScaleFactorDown( const pat::Jet& ) const;

  double GetTightScaleFactor( const pat::Jet& ) const;
  double GetTightScaleFactorUp( const pat::Jet& ) const;
  double GetTightScaleFactorDown( const pat::Jet& ) const;

  double scalefactor( const pat::Jet&,
                      BTagEntry::OperatingPoint,
                      const std::string& sys ) const;

private:
  BTagCalibration _calib;
  std::map<BTagEntry::OperatingPoint, BTagCalibrationReader> _reader_map;

};

} /* usr */


#endif/* end of include guard: TSTARANALYSIS_BASELINESELECTOR */
