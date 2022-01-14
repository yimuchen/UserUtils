/**
 * @file    PluginAlias.hpp
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 * @brief   Defining alias functions for EDM plugins via intermediate virtual
 *          class
 */

#ifndef USERUTILS_EDMUTILS_PLUGINALIAS_HPP
#define USERUTILS_EDMUTILS_PLUGINALIAS_HPP

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/one/EDFilter.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "TFile.h"
#include "TObject.h"

#include <string>

namespace usr
{

/**
 * @class PluginAlias
 * @ingroup EDMUtils
 * @brief class for reducing the verbosity of the various edm plugin calls.
 * @details One I find immensely annoying to code is the `consume` calls to
 *          create an EDMToken object. What makes it worse is that the consumes
 *          call is an protected member function of the edm plugin's base class.
 *          Writing an alias to the function, therefore, requires all future
 *          plugins to inherit from this alias class.
 */
template<typename EDMPLUGIN>
class PluginAlias : public EDMPLUGIN
{
public:
  /**
   * Like all EDM plugins, this requires a parameterset to initialize. A
   * reference to the parameterset is stored for less verbose parsing.
   */
  PluginAlias( const edm::ParameterSet& config ) :
    _config( config ){}
  virtual ~PluginAlias(){}

protected:
  /**
   * @brief template class for getting the EdmToken from the initializer
   *        configuration parameter set.
   *
   * @tparam TYPE  the type of object attempting to be consumed must be user
   *               specified.
   */
  template<typename TYPE>
  inline edm::EDGetToken
  GetToken( const std::string& configtag )
  {
    return edm::EDConsumerBase::consumes<TYPE>(
      _config.getParameter<edm::InputTag>( configtag ) );
  }


  /**
   * @brief Same template function as GetToken() except for getting Run level
   *        objects.
   */
  template<typename TYPE>
  inline edm::EDGetToken
  GetRunToken( const std::string& configtag )
  {
    return edm::EDConsumerBase::consumes<TYPE, edm::InRun>(
      _config.getParameter<edm::InputTag>( configtag ) );
  }


  /**
   * @brief Construction for a handle.
   */
  template<typename TYPE>
  inline edm::Handle<TYPE>
  MakeHandle( const edm::Event& event, const edm::EDGetToken tok )
  {
    edm::Handle<TYPE> handle;
    event.getByToken( tok, handle );
    assert( handle.isValid() );
    return handle;
  }


  /**
   * @brief getting a clone of an object described in a file. See static
   *        function for full documentation.
   */
  inline TObject*GetFileObj( const std::string& filetag,
                             const std::string  objtag )
  { return GetFileObj( _config, filetag, objtag ); }

  /**
   * @brief getting the full path to a file. See static function for full
   *        documentation.
   */
  inline std::string GetFilePath( const std::string& filetag )
  { return GetFilePath( _config, filetag ); }

  /**
   * @brief returing the string to a path in the parameterset if an
   *        EDM::FileInPath was used.
   */
  static std::string
  GetFilePath( const edm::ParameterSet& config,
               const std::string&       filetag )
  {
    return config.getParameter<edm::FileInPath>( filetag ).fullPath();
  }


  /**
   * @brief Given a file path in the form of an EDM::FileInPath, and a object
   * key
   *        as a parameter set string.
   *
   * This function return a clone to the object stored in a file for analysis
   * use.
   */
  static TObject*
  GetFileObj( const edm::ParameterSet& config,
              const std::string&       filetag,
              const std::string&       objtag )
  {
    const std::string filename = GetFilePath( config, filetag );
    const std::string objname  = config.getParameter<std::string>( objtag );
    TFile*            file     = TFile::Open( filename.c_str() );
    TObject*          ans      = file->Get( objname.c_str() )->Clone();

    // ans->SetDirectory(0); // Detach object from parent directory.
    file->Close();
    return ans;
  }

private:
  const edm::ParameterSet& _config;
};

typedef PluginAlias<edm::one::EDAnalyzer<edm::one::SharedResources> >
  EDAnalyzer;
typedef PluginAlias<edm::one::EDFilter<edm::one::SharedResources> >
  EDFilter;

}/* usr */

#endif/* end of include guard: USERUTILS_EDMUTILS_PLUGINALIAS_HPP */
