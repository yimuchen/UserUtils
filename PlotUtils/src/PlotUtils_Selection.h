// Dummy file for required for relex generation
#include "UserUtils/PlotUtils/interface/Simple1DCanvas.hpp"
#include "UserUtils/PlotUtils/interface/Ratio1DCanvas.hpp"
#include "UserUtils/PlotUtils/interface/Flat2DCanvas.hpp"
#include "UserUtils/PlotUtils/interface/CommonXCanvas.hpp"
#include "UserUtils/PlotUtils/interface/Constants.hpp"

namespace UserUtils_PlotUtils {
struct dictionaryobjects
{
  usr::plt::Simple1DCanvas   usr_plt_Simple1DCanvas;
  usr::plt::Ratio1DCanvas    usr_plt_Ratio1DCanvas;
  usr::plt::Flat2DCanvas     usr_plt_Flat2DCanvas;
  usr::plt::CommonXCanvas    usr_plt_CommonXCanvas;

  usr::plt::Pad1D     usr_plt_Pad1D;
  usr::plt::Pad2DFlat usr_plt_Pad2DFlat;
  //edm::Wrapper<usr::Counter> usrct_wrp;
};
}

