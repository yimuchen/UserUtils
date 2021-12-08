/**
 * @file CommonXCanvas.hpp
 * @author [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 * @brief Canvas containing multiple pads with a common X axis
 */
#ifndef USERUTILS_PLOTUTILS_COMMONXCANVAS_HPP
#define USERUTILS_PLOTUTILS_COMMONXCANVAS_HPP

#ifdef CMSSW_GIT_HASH
#include "UserUtils/Common/interface/STLUtils/VectorUtils.hpp"
#include "UserUtils/PlotUtils/interface/Canvas.hpp"
#include "UserUtils/PlotUtils/interface/Pad1D.hpp"
#else
#include "UserUtils/Common/STLUtils/VectorUtils.hpp"
#include "UserUtils/PlotUtils/Canvas.hpp"
#include "UserUtils/PlotUtils/Pad1D.hpp"
#endif

namespace usr
{

namespace plt
{

class CommonXCanvas : public Canvas
{
public:
  static length_t default_width;
  static length_t default_height;
  static FontSet  default_font;
  CommonXCanvas( const unsigned number_of_pads,
                 const length_t width  = default_width,
                 const length_t height = default_height,
                 const FontSet&        = default_font );
  CommonXCanvas( const RangeByVar&,
                 const unsigned number_of_pad,
                 const length_t width  = default_width,
                 const length_t height = default_height,
                 const FontSet&        = default_font );
  ~CommonXCanvas();


  inline Pad1D& Pad( unsigned padid ){ return GetPad<Pad1D>( padid ); }
  inline Pad1D& FirsTPad_()           { return Pad( 0 ); }
  inline Pad1D& LasTPad_()            { return Pad( _padlist.size()-1 ); }

  template<unsigned padid>
  inline Pad1D& Pad(){ return Pad( padid ); }


  /**
   * @{
   * @brief passing through for Pad1D plotting function, The pad ID to plot on
   * is
   *        uses template argument to help keep a uniform interface.
   */
  template<unsigned pad_id, typename ...Args>
  inline TH1D&
  PlotHist( TH1D& obj, Args ... args )
  {
    CheckPadAxis( &obj );
    return Pad<pad_id>().PlotHist( obj, args ... );
  }


  template<unsigned pad_id, typename ...Args>
  inline TH1D&
  PlotHist( TH1D*obj, Args ... args )
  {
    CheckPadAxis( obj );
    return Pad<pad_id>().PlotHist( obj, args ... );
  }


  template<unsigned pad_id, typename ...Args>
  inline TGraph&
  PlotGraph( TGraph& obj, Args ... args  )
  {
    CheckPadAxis( &obj );
    return Pad<pad_id>().PlotGraph( obj, args ... );
  }


  template<unsigned pad_id, typename ...Args>
  inline TGraph&
  PlotGraph( TGraph*obj, Args ... args  )
  {
    CheckPadAxis( obj );
    return Pad<pad_id>().PlotGraph( obj, args ... );
  }


  template<unsigned pad_id, typename ...Args>
  inline TGraph&
  PlotFunc( TF1& obj, Args ... args  )
  {
    CheckPadAxis( &obj );
    return Pad<pad_id>().PlotFunc( obj, args ... );
  }


  template<unsigned pad_id, typename ...Args>
  inline TGraph&
  PlotFunc( TF1*obj, Args ... args  )
  {
    CheckPadAxis( obj );
    return Pad<pad_id>().PlotFunc( obj, args ... );
  }


  template<unsigned pad_id, typename ...Args>
  inline TGraphAsymmErrors&
  PlotData( RooAbsData& obj, Args ... args  )
  {
    CheckPadAxis( &obj );
    return Pad<pad_id>().PlotData( obj, args ... );
  }


  template<unsigned pad_id, typename ...Args>
  inline TGraphAsymmErrors&
  PlotData( RooAbsData*obj, Args ... args  )
  {
    CheckPadAxis( obj );
    return Pad<pad_id>().PlotData( obj, args ... );
  }


  template<unsigned pad_id, typename ...Args>
  inline TGraph&
  PlotPdf( RooAbsPdf& obj, Args ... args  )
  {
    CheckPadAxis( &obj );
    return Pad<pad_id>().PlotPdf( obj, args ... );
  }


  template<unsigned pad_id, typename ...Args>
  inline TGraph&
  PlotPdf( RooAbsPdf*obj, Args ... args  )
  {
    CheckPadAxis( obj );
    return Pad<pad_id>().PlotPdf( obj, args ... );
  }


  /** @} */

private:
  void CheckPadAxis( const TObject* );

  void _init_margin();
};

}

}


#endif
