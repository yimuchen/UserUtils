/**
 * @file    PluginAlias.cc
 * @brief   Implementation of non plugin alias functions
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */

#include "UserUtils/EDMUtils/interface/PluginAlias.hpp"

#include "FWCore/ParameterSet/interface/FileInPath.h"

#include "TFile.h"
#include <string>

namespace usr {

/*----------------------------------------------------------------------------*/

std::string
PluginAlias::GetFilePath(
  const edm::ParameterSet& config,
  const std::string&       filetag )
{
  return config.getParameter<edm::FileInPath>( filetag ).fullPath();
}

/*----------------------------------------------------------------------------*/

TObject*
PluginAlias::GetFileObj(
  const edm::ParameterSet& config,
  const std::string&       filetag,
  const std::string&       objtag )
{
  const std::string filename = GetFilePath( config, filetag );
  const std::string objname  = config.getParameter<std::string>( objtag );
  TFile* file                = TFile::Open( filename.c_str() );
  TObject* ans               = file->Get( objname.c_str() )->Clone();
  file->Close();
  return ans;
}

/*----------------------------------------------------------------------------*/

}/* usr */
