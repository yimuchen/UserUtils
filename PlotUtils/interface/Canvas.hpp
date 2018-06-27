/**
 * @file    Canvas.hpp
 * @brief   An extension of the TCanvas and TPad classes for a better batch
 *          plotting experience.
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 */
#ifndef USERUTILS_PLOTUTILS_CANVAS_HPP
#define USERUTILS_PLOTUTILS_CANVAS_HPP

#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/STLUtils/Filesystem.hpp"
#include "UserUtils/Common/interface/STLUtils/StringUtils.hpp"
#include "UserUtils/PlotUtils/interface/Constants.hpp"
#else
#include "UserUtils/Common/STLUtils/Filesystem.hpp"
#include "UserUtils/Common/STLUtils/StringUtils.hpp"
#include "UserUtils/PlotUtils/Constants.hpp"
#endif


#include "TCanvas.h"
#include "TLatex.h"
#include "TPad.h"

#include <fstream>
#include <string>

namespace usr  {

namespace plt {

// forward declaration of the classes
class Canvas;
class PadBase;

/**
 * @brief A constructor helper class for Pad objects.
 * @details The doubles in the class correspond the the relative position of
 * the pad corners on the canvas object, just like the inputs of the original
 * of the @ROOT{TPad} object.
 */
struct PadSize
{
  double xmin;
  double ymin;
  double xmax;
  double ymax;
  PadSize(
    const double x_min = 0,
    const double y_min = 0,
    const double x_max = 1,
    const double y_max = 1 ) :
    xmin( x_min ),
    ymin( y_min ),
    xmax( x_max ),
    ymax( y_max ){}
};

/**
 * @brief Renaming the font class to make it look like a constructor wrapper.
 */
typedef font   FontSet;

/*-----------------------------------------------------------------------------
 *  Detailed documentation in doc/
   --------------------------------------------------------------------------*/
class PadBase : protected TPad
{
public:
  virtual
  ~PadBase ();
  friend class Canvas;

  PadBase()                 = delete;
  PadBase( const PadBase& ) = delete;

public:
  // Dimension access.
  double RelWidth() const;
  double RelHeight() const;
  double AbsWidth() const;
  double AbsHeight() const;

  /**
   * @{
   * @brief Pass throught for the margine setting and getting functions.
   */
  inline void
  SetTopMargin( const float x ){ TPad::SetTopMargin( x ); }
  inline void
  SetLeftMargin( const float x ){ TPad::SetLeftMargin( x ); }
  inline void
  SetRightMargin( const float x ){ TPad::SetRightMargin( x ); }
  inline void
  SetBottomMargin( const float x ){ TPad::SetBottomMargin( x ); }

  inline float
  GetTopMargin() const { return TPad::GetTopMargin(); }
  inline float
  GetLeftMargin() const { return TPad::GetLeftMargin(); }
  inline float
  GetRightMargin() const { return TPad::GetRightMargin(); }
  inline float
  GetBottomMargin() const { return TPad::GetBottomMargin(); }
  /** @} */

  float FontSize() const;
  short FontFace() const;
  float LineHeight() const;
  float RelTextHeight() const;
  float RelLineHeight() const;

  PadBase& SetTextCursor( const double, const double );
  PadBase& SetTextCursor( const double, const double, const font::align );
  PadBase& WriteLine( const std::string& );
  PadBase& WriteAtData( const double, const double, const std::string& );

protected:

  /** @brief Latex object used for text writing */
  TLatex _latex;

  /** @brief coordinates to plot the next line of latex text */
  float _latex_cursorx;
  /** @brief coordinates to plot the next line of latex text, automatically
   *  updates when WriteLine() is called. */
  float _latex_cursory;

  PadBase( const PadSize& );
  const Canvas& ParentCanvas() const;
  const FontSet& Font() const;
  virtual void InitDraw();
  virtual void Finalize();
  void         PlotObj( TObject&, Option_t* = "" );

  /** @brief pointer interface to PlotObj */
  inline void
  PlotObj( TObject* obj, Option_t* opt = "" ){ PlotObj( *obj, opt ); }

  /** @brief Getting the list of objects to be plotted on the pad */
  inline TList*
  GetListOfPrimitives() const { return TPad::GetListOfPrimitives(); }
};

/*-----------------------------------------------------------------------------
 *  Detailed documentation in doc/
   --------------------------------------------------------------------------*/
class Canvas : protected TCanvas
{
public:
  Canvas (
    const length_t width,
    const length_t height,
    const FontSet& = FontSet()
    );
  virtual
  ~Canvas ();

  /**
   * Creating a pad under the ownership of the canvas. Note that these pads
   * will be destroyed when the canvas goes out of scope. Immediately after
   * the pads are constructed, the virtual method PadBase::InitDraw() would be
   * called.
   * @tparam PadType Any children class of PadBase is allowed.
   * @tparam Args    Any additional types needed for the constructor is allowed
   * @param  padsize The use of a PadSize for the construction of a Pad is
   *                 mandatory for all PadBase children classes
   * @param  args    Any additional arguments that might be needed for the
   *                 pad constructor
   * @return         Returns a reference to the newly constructed argument.
   */
  template<typename PadType, typename ... Args>
  PadType&
  Add( const PadSize& padsize, Args ... args )
  {
    TCanvas::cd();// Ensureing the new Pad object spawn under this Canvas

    _padlist.push_back( new PadType( padsize, args ... ) );
    _padlist.back()->Draw();// Drawing the Pad object immediately
                            // to make sure the relation is recorded.
    _padlist.back()->InitDraw();// Allow for pad to predefine draw objects
    return *( dynamic_cast<PadType*>( _padlist.back() ) );
  }

  /**
   * Getting the reference to a pad owned by the canvas by index. Automatic
   * casting is also available.
   */
  template<typename PadType = PadBase>
  PadType&
  GetPad( const unsigned i )
  {
    return *( dynamic_cast<PadType*>( _padlist.at( i ) ) );
  }

  /**
   * @brief A more comprehensive interface to the canvas dimensions.
   */
  inline unsigned
  Width() const { return TCanvas::GetWw(); }

  /**
   * @brief A more comprehensive interface ot the canvas dimensions.
   */
  inline unsigned
  Height() const { return TCanvas::GetWh(); }

  inline const FontSet& Font() const { return _fontset; }

  void SaveAsPDF( const fs::path& );
  void SaveAsPNG( const fs::path&, const unsigned dpi = 300 );
  void SaveAsCPP( const fs::path& );
  void SaveToROOT( const fs::path&, const std::string& name );


protected:
  void     Finalize( const fs::path& );
  fs::path SaveTempPDF( const fs::path& );

protected:
  /**
   * @brief a copy of the font setting configuration used in the constructor.
   *
   * Since there are not direct objects in the canvas object, this is how
   * the font configuration would be stored.
   */
  const FontSet _fontset;

  /**
   * @brief List of pointers to the created PadBase objects.
   *
   * We must use raw points, as the `new` operator doesn't play well with
   * non-public inheritance of objects that already have its `new` operator
   * overloaded.
   */
  std::vector<PadBase*> _padlist;
};

}/* plt */

}/* usr  */

#endif/* end of include guard: USERUTILS_PLOTUTILS_CANVAS_HPP  */
