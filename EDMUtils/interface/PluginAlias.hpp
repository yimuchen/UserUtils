/**
 * @file    PluginAlias.hpp
 * @brief   virtual helper class to alias verbose 'consume' calls
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 *
 * Also includes other handy function to reduce verbosity of
 * EDMPlugins initialization.
 */

#ifndef USERUTILS_EDMUTILS_PLUGINALIAS_HPP
#define USERUTILS_EDMUTILS_PLUGINALIAS_HPP

#include "FWCore/Framework/interface/EDConsumerBase.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "TObject.h"

#include <string>

namespace usr {

class PluginAlias : public virtual edm::EDConsumerBase
{
public:
  PluginAlias( const edm::ParameterSet& config ) : _config( config ){}
  virtual
  ~PluginAlias(){}

protected:

  // Template functions for reducing verbosity of consume call.
  template<typename TYPE>
  inline edm::EDGetToken
  GetToken( const std::string& configtag )
  {
    return consumes<TYPE>( _config.getParameter<edm::InputTag>( configtag ) );
  }

  template<typename TYPE>
  inline edm::EDGetToken
  GetRunToken( const std::string& configtag )
  {
    return consumes<TYPE, edm::InRun>(
      _config.getParameter<edm::InputTag>( configtag ) );
  }

  // Function for getting single objects within a function.
  inline TObject*
  GetFileObj( const std::string& filetag, const std::string objtag )
  { return GetFileObj( _config, filetag, objtag ); }

  inline std::string
  GetFilePath( const std::string& filetag )
  { return GetFilePath( _config, filetag ); }

  static std::string GetFilePath(
    const edm::ParameterSet&,
    const std::string& );

  static TObject* GetFileObj(
    const edm::ParameterSet& config,
    const std::string&       file,
    const std::string&       objname );

private:
  const edm::ParameterSet& _config;

};

}/* usr */

#endif/* end of include guard: USERUTILS_EDMUTILS_PLUGINALIAS_HPP */
