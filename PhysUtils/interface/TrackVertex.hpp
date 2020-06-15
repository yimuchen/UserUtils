/**
 * @file ObjectExtendedVars.hpp
 * @author [Yi-Mu "Enoch" Chen](https://github.com/yimuchen/)
 * @brief additional calculation of track and vertex related values
 */
#ifndef USERUTILS_PHYSUTILS_TRACKVERTEX_HPP
#define USERUTILS_PHYSUTILS_TRACKVERTEX_HPP

#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"

#include "TLorentzVector.h"

/**
 * @addtogroup extendedvar
 * @{
 */
extern TLorentzVector TransientVertexP4( const TransientVertex&  );


/** @} */

#endif