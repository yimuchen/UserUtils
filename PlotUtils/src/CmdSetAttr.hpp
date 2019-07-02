#ifdef CMSSW_GIT_HASH
#include "UserUtils/PlotUtils/interface/PlotCommon.hpp"
#else
#include "UserUtils/PlotUtils/PlotCommon.hpp"
#endif

#include "TAttText.h"
#include "TAttLine.h"
#include "TAttFill.h"
#include "TAttMarker.h"

extern void SetTextAttr( TAttText& obj,
  const usr::plt::RooArgContainer& args  );

extern void SetLineAttr( TAttLine& obj,
  const usr::plt::RooArgContainer& args  );

extern void SetFillAttr( TAttFill& obj,
  const usr::plt::RooArgContainer& args  );

extern void SetMarkAttr( TAttMarker& obj,
  const usr::plt::RooArgContainer& args  );