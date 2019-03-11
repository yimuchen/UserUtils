#include "UserUtils/PlotUtils/interface/PlotCommon.hpp"

#include <regex>

namespace usr {

namespace plt {

double
EstimateLatexWidth( const std::string& text, const unsigned fontsize )
{
  double ans;
  std::string un_text;

  const std::regex frac_regex( "#frac\s*\{(.*)\}\s*\{(.*)\}");
  const std::regex sub_regex( "\_\{(.*)\}");
  const std::regex sup_regex( "\^\{(.*)\}");
  const std::regex dec_regex( "#[a-zA-Z]+\{(.*)\}");
  const std::regex sch_regex( "#[a-zA-Z]+");


  while( un_text.length() ){


  }

  return ans;
}

double
EstimateLatexHeight( const std::string& text, const unsigned fontsize )
{
  double ans;
  std::string un_text;

  const std::regex frac_regex( "#frac\s*\{(.*)\}\s*\{(.*)\}");
  const std::regex int_regex( "#int");
  const std::regex sub_regex( "\_\{(.*)\}");
  const std::regex sup_regex( "\^\{(.*)\}");
  const std::regex sqrt_regex( "#sqrt\{(.*)\}");


  while( un_text.length() ){


  }
  return ans;
}

}

}

