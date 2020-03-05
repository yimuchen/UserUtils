#ifndef USERUTILS_PLOTUTILS_SDPFORMAT_HPP
#define USERUTILS_PLOTUTILS_SDPFORMAT_HPP

#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/BoostUtils/PTreeUtils.hpp"
#include "UserUtils/MathUtils/interface/Measurement/Measurement.hpp"
#include "UserUtils/PlotUtils/interface/Pad1D.hpp"
#else
#include "UserUtils/Common/BoostUtils/PTreeUtils.hpp"
#include "UserUtils/MathUtils/Measurement/Measurement.hpp"
#include "UserUtils/PlotUtils/Pad1D.hpp"
#endif

#include "TFile.h"
#include "TH1D.h"

#include <memory>

namespace usr {

namespace plt {

namespace fmt {

struct Process;
struct ProcessGroup;
struct HistRequest;
struct BatchRequest;

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
  std::string key_prefix ;
  double scale;
  double effective_luminosity;
  unsigned run_range_min;
  unsigned run_range_max;

private:
  Process( const usr::pt::ptree& tree );

  TFile* _file;

  bool  CheckKey( const std::string& ) const;
  std::string MakeKey( const std::string&) const ;
  TH1D* GetNormalizedClone( const std::string& ) const ;
  TH1D* GetScaledClone( const std::string&, const double ) const;
  TH1D* GetClone( const std::string& ) const;
};


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
  ProcessGroup( const usr::pt::ptree& tree );
};

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
  HistRequest( const usr::pt::ptree& tree );
};

class Uncertainty
{
public:
  friend BatchRequest;

  std::string name;
  std::string key;
  usr::Measurement norm_uncertainty;

private:
  Uncertainty( const usr::pt::ptree& tree );
};

class BatchRequest
{
public:
  std::vector<HistRequest> histlist;
  std::vector<ProcessGroup> background;
  ProcessGroup data;
  std::vector<Process> signallist;
  std::vector<Uncertainty> uncertainties;

  BatchRequest( const std::string& jsonfile );
  BatchRequest( const usr::pt::ptree& tree );

  void GeneratePlots();

  void GenerateSampleComparePlot() ;

  void GenerateSimulationTable( std::ostream& stream )  const;
  void GenerateSimulationSummary( std::ostream& stream ) const;
  void GenerateDataTable( std::ostream& ) const;

private:
  void initialize( const usr::pt::ptree& tree );

  // Temporary variables for generating plots:
  std::vector<std::unique_ptr<TH1D> > _background_stack;
  std::unique_ptr<TH1D> _background_stat;
  std::unique_ptr<TH1D> _background_sys;
  std::unique_ptr<TH1D> _data_hist;

  double _total_luminosity;

  // Generating background objects
  void GenerateBackgroundObjects( const HistRequest& );

  // Generate Data Results
  void GenerateData( const HistRequest& );

  // Running on Pad1D objects
  void PlotOnPad( const HistRequest& histrequest, Pad1D& pad );
};

}// fmt

}// plt

}// usr

#endif
