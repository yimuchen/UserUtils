/**
 * @file    PadBase.cc
 * @brief   Implementation of memebers of PadBase class
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifdef CMSSW_GIT_HASH
#include "UserUtils/PlotUtils/interface/Canvas.hpp"
#else
#include "UserUtils/PlotUtils/Canvas.hpp"
#endif

namespace usr  {

namespace plt  {

/**
 * @brief Construction of the new pad object requires the new wrapper class
 *        PadSize.
 *
 * PadSize defines the dimension in the same order as the original constructor
 * of the @ROOT{TPad} object (x_min, y_min, x_max, y_max relative to the parent
 * canvas.) Additional settings include adding the tick settings of tick on both
 * sides, and the initialization of the internal TLatex object.
 *
 * The name used to constructed the pad would be a random string to avoid name
 * collision.
 */
PadBase::PadBase( const PadSize& size ) :
  TPad( RandomString( 12 ).c_str(), "",
        size.xmin, size.ymin, size.xmax, size.ymax )
{
  TPad::SetTicks( 1, 1 );

  _latex.SetTextFont( FontFace() );
  _latex.SetTextSize( FontSize() );
  _latex.SetTextAlign( font::top_left );
}

/**
 * Nothing particular to do with the destructor.
 */
PadBase::~PadBase()
{
  // TODO: Horrible hack to stop programs from running into double free errors.
  // Must try and find a proper solution.
  TPad::fCollideGrid = nullptr;
}

/**
 * @brief Returning referece to parent canvas.
 *
 * @details Using C style type casting on the TPad method, as dynamic cast
 * requires public inheritance.
 */
const Canvas&
PadBase::ParentCanvas() const
{
  return *( (Canvas*)( TPad::GetMother() ) );
}

/**
 * @brief Returning reference to the font settings stored in the parent canvas.
 */
const FontSet&
PadBase::Font() const
{ return ParentCanvas().Font(); }

/**
 * @{
 * @brief Dimensions relative to the parent canvas.
 */
double
PadBase::RelWidth() const { return TPad::GetWNDC(); }

double
PadBase::RelHeight() const { return TPad::GetHNDC(); }
/** @} */

/**
 * @{
 * @brief Absolute dimensions of the pad.
 */
double
PadBase::AbsWidth() const { return ParentCanvas().Width() * RelWidth(); }

double
PadBase::AbsHeight() const { return ParentCanvas().Height() * RelHeight(); }
/** @} */


/**
 * @{
 * @brief Access to the font setting in the parent canvas.
 */
float
PadBase::FontSize() const { return ParentCanvas().Font().size(); }

short
PadBase::FontFace() const { return ParentCanvas().Font().fontface(); }

float
PadBase::LineHeight() const { return ParentCanvas().Font().lineheight(); }
/** @} */

/**
 * @{
 * @brief font settings relative to the pad dimensions.
 */
float
PadBase::RelTextHeight() const { return FontSize()/AbsHeight(); }

float
PadBase::RelLineHeight() const { return LineHeight()/AbsHeight(); }
/** @} */


/**
 * @brief Writing latex text at data point.
 */
PadBase&
PadBase::WriteAtData( const double x, const double y, const std::string& line )
{
  TPad::cd();
  _latex.DrawLatex( x, y, line.c_str() );
  return *this;
}

/**
 * @{
 * @brief Writing text a designated cursor position.
 *
 * The write line can be chained to write multiple lines of text:
 */
PadBase&
PadBase::WriteLine( const std::string& line )
{
  TPad::cd();
  _latex.DrawLatexNDC( _latex_cursorx, _latex_cursory, line.c_str() );
  _latex_cursory -= RelLineHeight();
  return *this;
}

PadBase&
PadBase::SetTextCursor( const double x, const double y, const font::align a )
{
  _latex.SetTextAlign( a );
  return SetTextCursor( x, y );
}

PadBase&
PadBase::SetTextCursor( const double x, const double y )
{
  _latex_cursorx = x;
  _latex_cursory = y;
  return *this;
}
/** @} */

/**
 * @brief Interface to superseed the `TObject::Draw()` for generating plots.
 *
 * This functions is simple a guarantee that this specific @ROOT{TPad} is used
 * to call the `TObject::Draw()` function, and the options is also directly
 * passed to the `Draw()` without any additional parsing. This method is made
 * to be protected to only be used by child classes for plot specialization.
 */
void
PadBase::PlotObj( TObject& obj, Option_t* opt )
{
  TPad::cd();
  obj.Draw( opt );
  TPad::Update();
}

bool
PadBase::HasObject( const TObject& obj ) const
{
  return TPad::FindObject(obj.GetName())  == &obj;
}

/**
 * @brief Additional initializations to be performed by the pad after it has
 * been spawned on the canvas.
 *
 * Leaving blank for base class.
 */
void
PadBase::InitDraw()
{}

/**
 * @brief Additional steps to be performed by the pad before the canvas saving
 * functions are called.
 *
 * Leaving blank for base class.
 */
void
PadBase::Finalize()
{}

}/* plt  */

}/* usr  */
