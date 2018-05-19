/**
 * @file    Canvas.ipp
 * @brief   Implementation of Template functions for the Canvas base
 * @author  Yi-Mu "Enoch" Chen (ensc@hep1.phys.ntu.edu.tw)
 */
#ifdef USERUTILS_PLOTUTILS_CANVAS_HPP

namespace usr  {

namespace plt  {

template<typename PadType, typename ... Args>
PadType&
Canvas::Add( const PadSize& padsize, Args ... args )
{
  TCanvas::cd();// Ensureing the new Pad object spawn under this Canvas

  _padlist.push_back( new PadType( padsize, args ... ) );
  _padlist.back()->Draw();// Drawing the Pad object immediately
                          // to make sure the relation is recorded.
  _padlist.back()->InitDraw(); // Allow for pad to predefine draw objects
  return *( dynamic_cast<PadType*>( _padlist.back() ) );
}

template<typename PadType>
PadType&
Canvas::GetPad( const unsigned i )
{
  return *( dynamic_cast<PadType*>( _padlist.at( i ) ) );
}

}/* plt  */

}/* usr  */

#endif
