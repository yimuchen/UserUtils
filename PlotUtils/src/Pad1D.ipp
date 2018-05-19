/**
 * @file    PadFrame1D.ipp
 * @brief   implementation of variadic functions
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */
#ifdef USERUTILS_PLOTUTILS_PAD1D_HPP

#include "UserUtils/Common/interface/STLUtils/VectorUtils.hpp"

namespace usr  {

namespace plt  {

template<typename Ret, typename Arg>
Ret&
Pad1D::GenRoofitGraph( Arg& obj, RooLinkedList& arglist )
{
  RooPlot* test = obj.plotOn( &_frame, arglist );
  if( !test ){
    throw std::invalid_argument("Bad argument list, plotting failed" );
  }
  Ret& ans = *( dynamic_cast<Ret*>(
                  _frame.getObject( _frame.numItems() -1 ) ) );
  // Additional parsing required for the Plotted objects
  return ans;
}

/*----------------------------------------------------------------------------*/

template<typename ObjType, typename... Args>
ObjType& Roo1DFrame::MakeObj( Args... args )
{
  ObjType* ptr = new ObjType( args... );
  RooPlot::addObject( ptr );
  return *ptr;
}

/*----------------------------------------------------------------------------*/

template<typename ObjType>
ObjType& Roo1DFrame::MakeClone( ObjType& obj )
{
  TObject* ptr = obj.Clone();
  RooPlot::addObject(  ptr );
  return *dynamic_cast<ObjType*>( ptr );
}

}/* plt  */

}/* usr  */

#endif
