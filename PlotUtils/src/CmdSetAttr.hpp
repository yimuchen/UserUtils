#ifndef USERUTILS_PLOTUTILS_SRC_CMDSETATTR_HPP
#define USERUTILS_PLOTUTILS_SRC_CMDSETATTR_HPP

#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/RootUtils/RooArgContainer.hpp"
#else
#include "UserUtils/Common/RootUtils/RooArgContainer.hpp"
#endif

#include "TAttFill.h"
#include "TAttLine.h"
#include "TAttMarker.h"
#include "TAttText.h"

extern void SetTextAttr( TAttText&                   obj,
                         const usr::RooArgContainer& args  );

extern void SetLineAttr( TAttLine&                   obj,
                         const usr::RooArgContainer& args  );

extern void SetFillAttr( TAttFill&                   obj,
                         const usr::RooArgContainer& args  );

extern void SetMarkAttr( TAttMarker&                 obj,
                         const usr::RooArgContainer& args  );

extern void CopyTextAttrTo( const TAttText& source,
                            TAttText&       target );
extern void CopyLineAttrTo( const TAttLine& source,
                            TAttLine&       target );
extern void CopyFillAttrTo( const TAttFill& source,
                            TAttFill&       target );
extern void CopyMarkAttrTo( const TAttMarker& source,
                            TAttMarker&       target );

#endif
