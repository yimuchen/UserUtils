/**
 * @file StandardPlotFormat.hpp
 * @author Yi-Mu "Enoch Chen"
 * @brief Definition of all standardized plotting related classes.
 */
#ifndef USERUTILS_PLOTUTILS_SDPFORMAT_HPP
#define USERUTILS_PLOTUTILS_SDPFORMAT_HPP

#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/RapidJson.hpp"
#include "UserUtils/MathUtils/interface/Measurement/Measurement.hpp"
#include "UserUtils/PlotUtils/interface/Pad1D.hpp"
#else
#include "UserUtils/Common/RapidJson.hpp"
#include "UserUtils/MathUtils/Measurement/Measurement.hpp"
#include "UserUtils/PlotUtils/Pad1D.hpp"
#endif

#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"

#include <memory>

namespace usr {

namespace plt {

namespace fmt {

/**
 * @addtogroup StandardizedPlot
 * @{
 */


class Process;
class ProcessGroup;
class Uncertainty;
class IOSetting;
class HistRequest;
class BatchRequest;

/**
 * @class Process
 * @brief Constainer of a single process information.
 */
class Process
{
public:
  friend ProcessGroup;
  friend BatchRequest;

  std::string name;
  std::string latex_name;
  std::string generator;
  std::string cross_section_source;
  usr::Measurement cross_section;
  std::string file;
  std::string color;
  std::string key_prefix;
  double scale;
  double effective_luminosity;
  unsigned run_range_min;
  unsigned run_range_max;
  float transparency;

private:
  Process( const usr::JSONMap& map, const BatchRequest* parent );

  TFile* _file;
  const BatchRequest* parent;

  void OpenFile();
  inline const BatchRequest&
  Parent() const {return *parent;}

  bool        CheckKey( const std::string& ) const;
  std::string MakeKey( const std::string& ) const;
  TH1D*       GetNormalizedClone( const std::string& ) const;
  TH1D*       GetScaledClone( const std::string&, const double ) const;
  TH1D*       GetClone( const std::string& ) const;
  TH2D*       Get2DClone( const std::string& ) const;
};


/**
 * @class ProcessGroup
 * @brief Container for a group of processes that will share a common display
 * information
 */
class ProcessGroup : public std::vector<Process>
{
public:
  friend BatchRequest;

  std::string name;
  std::string latex_name;
  std::string color;
  bool is_data;

private:
  ProcessGroup();
  ProcessGroup( const usr::JSONMap& map, const BatchRequest* parent );
};

/**
 * @class HistRequest
 * @brief Container for a histogram request
 */
class HistRequest
{
public:
  friend BatchRequest;
  std::string name;
  std::string xaxis;
  std::string units;
  std::string yaxis;
  std::string filekey;
  std::string type;
  bool logy;

private:
  HistRequest( const usr::JSONMap& map );
};

/**
 * @class Uncertainty
 * @brief Container for an uncertainty
 */
class Uncertainty
{
public:
  friend BatchRequest;

  std::string name;
  std::string key;
  usr::Measurement norm_uncertainty;

private:
  Uncertainty( const usr::JSONMap& map );
};

/**
 * @class IOSetting
 * @brief Definition of a custom io settings
 */
class IOSetting
{
public:
  friend BatchRequest;
  std::string input_prefix;
  std::string key_prefix;
  std::string output_prefix;
  std::string output_postfix;

private:
  IOSetting( const usr::JSONMap& map );
  IOSetting();
};

/**
 * @class BatchRequest
 * @brief Class defining a request to run some standardized plotting routine.
 */
class BatchRequest
{
public:
  std::vector<HistRequest> histlist;
  std::vector<ProcessGroup> background;
  ProcessGroup data;
  std::vector<Process> signallist;
  std::vector<Uncertainty> uncertainties;

  BatchRequest( const std::string& jsonfile );
  BatchRequest( const std::vector<std::string>& jsonfiles );
  BatchRequest( const usr::JSONMap& map );

  void GeneratePlots();
  void GenerateSampleComparePlot();
  void GenerateSimulationTable( std::ostream& stream )  const;
  void GenerateSimulationSummary( std::ostream& stream ) const;
  void GenerateDataTable( std::ostream& ) const;
  void Generate2DComaprePlot();

  void UpdateInputPrefix( const std::string& );
  void UpdateKeyPrefix( const std::string& );
  void UpdateOutputPrefix( const std::string& );
  void UpdateoutputPostfix( const std::string& );

private:
  friend Process;
  void initialize( const usr::JSONMap& map );

  IOSetting iosetting;

  // Temporary variables for generating plots:
  std::vector<std::unique_ptr<TH1D> > _background_stack;
  std::unique_ptr<TH1D> _background_stat;
  std::unique_ptr<TH1D> _background_sys;
  std::unique_ptr<TH1D> _data_hist;

  double _total_luminosity;

  void GenerateBackgroundObjects( const HistRequest& );
  void GenerateData( const HistRequest& );
  void PlotOnPad( const HistRequest& histrequest, Pad1D& pad );
};

/** @} */


}// fmt

}// plt

}// usr



#endif
