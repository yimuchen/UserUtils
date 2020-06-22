/**
 * @file StandardFormat_Table.cc
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2020-06-15
 *
 * @copyright Copyright (c) 2020
 *
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/Maths.hpp"
#include "UserUtils/Common/interface/STLUtils.hpp"
#include "UserUtils/MathUtils/interface/Measurement/Format.hpp"
#include "UserUtils/PlotUtils/interface/StandardPlotFormat.hpp"
#else
#include "UserUtils/Common/Maths.hpp"
#include "UserUtils/Common/STLUtils.hpp"
#include "UserUtils/MathUtils/Measurement/Format.hpp"
#include "UserUtils/PlotUtils/StandardPlotFormat.hpp"
#endif

// ------------------------------------------------------------------------------

namespace usr {

namespace plt {

namespace fmt {

void BatchRequest::GenerateSimulationTable( std::ostream& stream ) const
{
  static const std::string pre_column_format
    = "%%%ds & %%%ds & %%7s & %%10s & %%20s  \\\\\n";
  static const std::string h_line = "\\hline";

  auto MakeLine = []( const std::string& column_format,
                      const Process& process )->std::string {
                    return usr::fstr( column_format
                                    , process.latex_name
                                    , usr::fmt::decimal( process.cross_section )
                                    , "(" + process.cross_section_source + ")"
                                    , process.generator
                                    , process.effective_luminosity );
                  };

  // Additional string processing
  unsigned max_name_length = 0;
  unsigned max_xsec_length = 0;

  for( const auto& process : signallist ){
    max_name_length
      = usr::max( max_name_length, process.latex_name.length() );
    max_xsec_length
      = usr::max( max_xsec_length
                , usr::fmt::decimal( process.cross_section ).str().length() );
  }

  for( const auto& group : background ){
    for( const auto& process : group ){
      max_name_length
        = usr::max( max_name_length, process.latex_name.length() );
      max_xsec_length
        = usr::max( max_xsec_length
                  , usr::fmt::decimal( process.cross_section ).str().length() );
    }
  }

  const std::string column_format = usr::fstr( pre_column_format
                                             , max_name_length+1
                                             , max_xsec_length+1 );

  // Generating the output
  stream << h_line << std::endl;

  for( const auto& process : signallist ){
    stream << MakeLine( column_format, process );
  }

  for( const auto& group : background ){
    stream << h_line << std::endl;

    for( const auto process : group ){
      stream << MakeLine( column_format, process );
    }
  }

  stream <<  h_line << std::endl;
}

void BatchRequest::GenerateSimulationSummary( std::ostream& stream ) const
{
  static const std::string pre_column_format
    = "%%%ds & %%%ds & %%7s & %%12s  \\\\\n";
  static const std::string h_line = "\\hline";

  auto MakeLineP
    = []( const std::string& column_format,
          const Process& process )->std::string {
        return usr::fstr( column_format
                        , process.latex_name
                        , usr::fmt::decimal( process.cross_section )
                        , "(" + process.cross_section_source + ")"
                        , process.generator );
      };

  auto MakeLineG
    = []( const std::string& column_format,
          const ProcessGroup& group )->std::string {
        usr::Measurement total_xsec( 0, 0, 0 );
        double max_xsec = 0;
        std::string xsource;
        std::string xother = "";
        std::string gen;
        std::string gen_other = "";

        for( const auto& process : group ){
          total_xsec += process.cross_section;
          if( max_xsec < process.cross_section.CentralValue() ){
            xsource = process.cross_section_source;
            gen     = process.generator;
          }
        }

        for( const auto& process : group ){
          if( xsource != process.cross_section_source ){
            xother = "+\\ldots";
          }
          if( gen != process.generator ){
            gen_other = "(+\\ldots)";
          }
        }

        return usr::fstr( column_format
                        , group.latex_name
                        , usr::fmt::decimal( total_xsec )
                        , "(" + xsource + xother + ")"
                        , gen + gen_other  );
      };

  // Additional string processing
  unsigned max_name_length = 0;
  unsigned max_xsec_length = 0;

  for( const auto& process : signallist ){
    max_name_length
      = usr::max( max_name_length, process.latex_name.length() );
    max_xsec_length
      = usr::max( max_xsec_length
                , usr::fmt::decimal( process.cross_section ).str().length() );
  }

  for( const auto& group : background ){
    max_name_length
      = usr::max( max_name_length, group.latex_name.length() );

    for( const auto& process : group ){
      max_xsec_length
        = usr::max( max_xsec_length
                  , usr::fmt::decimal( process.cross_section ).str().length() );
    }
  }

  const std::string column_format = usr::fstr( pre_column_format
                                             , max_name_length+1
                                             , max_xsec_length+2 );

  // Generating the output
  stream << h_line << std::endl;

  for( const auto& process : signallist ){
    stream << MakeLineP( column_format, process );
  }

  for( const auto& group : background ){
    stream << h_line << std::endl;
    stream << MakeLineG( column_format, group );
  }

  stream <<  h_line << std::endl;
}

void BatchRequest::GenerateDataTable( std::ostream& stream ) const
{
  static const std::string h_line = "\\hline";

  auto MakeLine
    = []( const Process& process )->std::string {
        return usr::fstr( "%50s & %8s--%8s & %s \\\\\n",
          process.latex_name,
          usr::fmt::base::decimal( process.run_range_min ).spacestr( "\\,," ),
          usr::fmt::base::decimal( process.run_range_max ).spacestr( "\\,," ),
          usr::fmt::base::decimal( process.effective_luminosity, 2 ) );
      };


  stream << h_line << std::endl;

  for( const auto& d : data ){
    stream << MakeLine( d );
  }

  stream << h_line << std::endl;
}

}

}

}