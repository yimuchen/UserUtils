#ifdef CMSSW_GIT_HASH
//#include "UserUtils/PlotUtils/interface/PlotCommon.hpp"
#include "UserUtils/Common/interface/RootUtils.hpp"
#else
#include "UserUtils/Common/RootUtils.hpp"
// #include "UserUtils/PlotUtils/PlotCommon.hpp"
#endif

#include "TAttText.h"
#include "TAttLine.h"
#include "TAttFill.h"
#include "TAttMarker.h"

extern void SetTextAttr( TAttText& obj,
  const usr::RooArgContainer& args  );

extern void SetLineAttr( TAttLine& obj,
  const usr::RooArgContainer& args  );

extern void SetFillAttr( TAttFill& obj,
  const usr::RooArgContainer& args  );

extern void SetMarkAttr( TAttMarker& obj,
  const usr::RooArgContainer& args  );