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
#include "UserUtils/Common/interface/STLUtils/VectorUtils.hpp"
#include "UserUtils/PlotUtils/interface/Constants.hpp"
#include "UserUtils/PlotUtils/interface/PlotCommon.hpp"
#else
#include "UserUtils/Common/STLUtils/Filesystem.hpp"
#include "UserUtils/Common/STLUtils/StringUtils.hpp"
#include "UserUtils/Common/STLUtils/VectorUtils.hpp"
#include "UserUtils/PlotUtils/Constants.hpp"
#include "UserUtils/PlotUtils/PlotCommon.hpp"
#endif

#include "TCanvas.h"
#include "TLatex.h"
#include "TPad.h"

#include <fstream>
#include <memory>
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
typedef font FontSet;

/*-----------------------------------------------------------------------------
 *  Detailed documentation in doc/
   --------------------------------------------------------------------------*/
class PadBase
{
public:
  virtual ~PadBase ();
  friend class Canvas;

  PadBase()                 = delete;
  // PadBase(){}
  PadBase( const PadBase& ) = delete;

public:
  // Dimension access.
  double RelWidth() const;
  double RelHeight() const;
  double AbsWidth() const;
  double AbsHeight() const;

  float FontSize() const;
  short FontFace() const;
  float LineHeight() const;
  float RelTextHeight() const;
  float RelLineHeight() const;

  PadBase& SetTextCursor( const double, const double );
  PadBase& SetTextCursor( const double, const double, const font::align );
  PadBase& SetTextAlign( const font::align );

  PadBase& WriteLine( const std::string&,
                      const std::vector<RooCmdArg>& );
  inline PadBase&
  WriteLine( const std::string& str )
  { return WriteLine( str, {} ); }
  template<typename ... Args>
  inline PadBase&
  WriteLine( const std::string& str,
             const RooCmdArg& arg1, Args ... args )
  { return WriteLine( str, usr::MakeVector<RooCmdArg>( arg1, args ... ) ); }


  PadBase& WriteAtData( const double, const double, const std::string&,
                        const std::vector<RooCmdArg>& );
  inline PadBase&
  WriteAtData( const double x, const double y, const std::string& str )
  { return WriteAtData( x, y, str, {} ); }
  template<typename ... Args>
  inline PadBase&
  WriteAtData( const double x, const double y, const std::string& str,
               const RooCmdArg& arg1, Args ... args )
  {
    return WriteAtData( x, y, str,
      usr::MakeVector<RooCmdArg>( arg1, args ... ) );
  }

  void PlotObj( TObject&, Option_t* = "" );

  /** @brief pointer interface to PlotObj */
  inline void
  PlotObj( TObject* obj, Option_t* opt = "" ){ PlotObj( *obj, opt ); }

  inline void
  PlotObject( TObject* obj, const std::string& opt = "" )
  { PlotObject( obj, opt.c_str() ); }

  inline void
  PlotObject( TObject& obj, const std::string& opt = "" )
  { PlotObject( obj, opt.c_str() ); }

  bool HasObject( const TObject& ) const;
  inline bool
  HasObject( const TObject* obj ) const { return HasObject( *obj ); }

  /**
   * Making a Root object under the ownership of the RooPlot object.
   * Using the variadic interface to allow for any sort of declaration type.
   * @tparam ObjType The type of object you which to create (must be explicitly
   *                 specified)
   * @tparam Args    the arguments could be of any type necessary.
   * @param  args    any arguments required for a TObject inherited object.
   * @return         Reference to the newly created object.
   */
  template<typename ObjType, typename ... Args>
  ObjType&
  MakeObj( Args ... args )
  {
    _generated_objects.emplace_back( new ObjType( args ... ) );
    return *dynamic_cast<ObjType*>( _generated_objects.back() );
  }

  void ClaimObject( TObject* );

  // Changing plot sequence.
  bool MoveTargetToBefore( const TObject& target, const TObject& before );

  /**
   * @{
   * @brief Pass-through interface for Margin control
   */
  inline float
  GetTopMargin() const { return _pad->GetTopMargin(); }
  inline float
  GetLeftMargin() const { return _pad->GetLeftMargin(); }
  inline float
  GetRightMargin() const { return _pad->GetRightMargin(); }
  inline float
  GetBottomMargin() const { return _pad->GetBottomMargin(); }

  inline void SetTopMargin( const float x )   {  _pad->SetTopMargin( x ); }
  inline void SetLeftMargin( const float x )  {  _pad->SetLeftMargin( x ); }
  inline void SetRightMargin( const float x ) {  _pad->SetRightMargin( x ); }
  inline void SetBottomMargin( const float x ){  _pad->SetBottomMargin( x ); }
  /** @} */


  inline void
  SetFillColorAlpha( const int x, const float y )
  {
    _pad->SetFillColorAlpha( x, y );
  }


  // Access to Parent
  const Canvas& ParentCanvas() const;
  Canvas&       ParentCanvas();



  // Access to the base object
  inline const TPad&
               TPad_() const { return *( _pad ); }
  inline TPad& TPad_()       { return *( _pad ); }

protected:

  /** @brief Latex object used for text writing */
  TLatex _latex;

  /** @brief coordinates to plot the next line of latex text */
  float _latex_cursorx;
  /** @brief coordinates to plot the next line of latex text, automatically
   *  updates when WriteLine() is called. */
  float _latex_cursory;

  PadBase( Canvas*, const PadSize& );
  const FontSet& Font() const;
  virtual void   InitDraw();
  virtual void   Finalize();

  /** @brief a lot of objects for plotting will be dynamically generated (ex.
   * text objects ) The Pad object will then claim ownership of these objects
   * and will be cleaned when the object is deleted.
   */
  std::vector<TObject*> _generated_objects;

  /** @brief Getting the list of objects to be plotted on the pad */
  inline TList*
  GetListOfPrimitives() const { return _pad->GetListOfPrimitives(); }

  Canvas* _parentcanvas;
  TPad* _pad;
};

/*-----------------------------------------------------------------------------
 *  Detailed documentation in doc/
   --------------------------------------------------------------------------*/
class Canvas
{
public:
  Canvas (
    const length_t width,
    const length_t height,
    const FontSet& = FontSet() );
  virtual ~Canvas ();

  template<typename PadType, typename ... Args>
  PadType&
  Add( const PadSize& padsize, Args ... args )
  {
    _canvas->cd();// Ensureing the new Pad object spawn under this Canvas
    _padlist.emplace_back( new PadType( this, padsize, args ... ) );
    _padlist.back()->_pad->Draw();// Drawing the Pad object immediately
                                  // to make sure the relation is recorded.
    _padlist.back()->InitDraw();// Allow for pad to predefine draw objects
    return *( dynamic_cast<PadType*>( _padlist.back().get() ) );
  }

  /**
   * Getting the reference to a pad owned by the canvas by index. Automatic
   * casting is also available.
   */
  template<typename PadType = PadBase>
  PadType&
  GetPad( const unsigned i )
  {
    return *( dynamic_cast<PadType*>( _padlist.at( i ).get() ) );
  }

  template<typename PadType = PadBase>
  const PadType&
  GetPad( const unsigned i ) const
  {
    return *( dynamic_cast<PadType*>( _padlist.at( i ).get() ) );
  }

  /**
   * @brief A more comprehensive interface to the canvas dimensions.
   */
  inline unsigned
  Width() const { return TCanvas_().GetWw(); }

  /**
   * @brief A more comprehensive interface ot the canvas dimensions.
   */
  inline unsigned
  Height() const { return TCanvas_().GetWh(); }

  inline const FontSet&
  Font() const { return _fontset; }

  void SaveAsPDF( const fs::path& );
  void SaveAsPNG( const fs::path&, const unsigned dpi = 300 );
  void SaveAsCPP( const fs::path& );
  void SaveToROOT( const fs::path&, const std::string& name );

  // String based interface for reflection
  // inline void SaveAsPDF( const std::string& x )
  // { SaveAsPDF( fs::path( x ) ); }
  // inline void SaveAsPNG( const std::string& x, const unsigned d  )
  // { SaveAsPNG( fs::path( x ), d ); }
  // inline void SaveAsCPP( const std::string& x )
  // { SaveAsCPP( fs::path( x ) ); }
  // inline void SaveToROOT( const std::string& x, const std::string& name )
  // { SaveToROOT( fs::path( x ), name ); }

  // Char based interface since now overloading is ambiguous with string literals
  // inline void SaveAsPDF( const char* x )
  // { SaveAsPDF( fs::path( x ) ); }
  // inline void SaveAsPNG( const char* x, const unsigned d  )
  // { SaveAsPNG( fs::path( x ), d ); }
  // inline void SaveAsCPP( const char* x )
  // { SaveAsCPP( fs::path( x ) ); }
  // inline void SaveToROOT( const char* x, const std::string& name )
  // { SaveToROOT( fs::path( x ), name ); }

  // string interface to allow for python interfacing
  // void SaveAsPDF( const std::string& s ){ SaveAsPDF( fs::path( s ) ); }

  inline void
  Clear(){ TCanvas_().Clear(); }


  inline const TCanvas&
                  TCanvas_() const { return *( _canvas ); }
  inline TCanvas& TCanvas_()       { return *( _canvas ); }

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
  std::vector<std::unique_ptr<PadBase> > _padlist;
  TCanvas* _canvas;
};

}/* plt */

}/* usr  */

#endif/* end of include guard: USERUTILS_PLOTUTILS_CANVAS_HPP  */
