#include "UserUtils/EDMUtils/interface/EDHistogramAnalyzer.hpp"

namespace usr
{

EDHistogramAnalyzer::EDHistogramAnalyzer( const edm::ParameterSet& config  ) :
  PluginAlias( config )
{
  usesResource( "TFileService" );
}


EDHistogramAnalyzer::~EDHistogramAnalyzer()
{}

}
