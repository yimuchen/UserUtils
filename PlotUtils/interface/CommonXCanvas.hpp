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

namespace usr {

namespace plt {

class CommonXCanvas : public Canvas
{
public:
  static length_t default_width;
  static length_t default_height;
  static FontSet default_font;

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
  inline Pad1D& FirstPad()           { return Pad( 0 ); }
  inline Pad1D& LastPad()            { return Pad( _padlist.size()-1 ); }

  template<unsigned padid>
  inline Pad1D& Pad(){ return Pad( padid ); }


#define PASSTHROUGH_PLOTFUNC( FUNC_NAME, PLOT_TYPE, RET_TYPE ) \
  template<unsigned pad_id, typename ... Args>                 \
  inline RET_TYPE& FUNC_NAME( PLOT_TYPE& obj, Args ... args ){ \
    CheckPadAxis( &obj );                                  \
    return Pad<pad_id>().FUNC_NAME( obj, args ... );           \
  }                                                            \
  template<unsigned pad_id, typename ... Args>                 \
  inline RET_TYPE& FUNC_NAME( PLOT_TYPE* obj, Args ... args ){ \
    CheckPadAxis( obj );                                   \
    return Pad<pad_id>().FUNC_NAME( obj, args ... );           \
  }

  /**
   * @{
   * @brief passing through for Pad1D plotting function, The pad ID to plot on is
   *        uses template argument to help keep a uniform interface.
   */
  PASSTHROUGH_PLOTFUNC( PlotHist,  TH1D,       TH1D );
  PASSTHROUGH_PLOTFUNC( PlotFunc,  TF1,        TGraph );
  PASSTHROUGH_PLOTFUNC( PlotGraph, TGraph,     TGraph );
  PASSTHROUGH_PLOTFUNC( PlotData,  RooAbsData, TGraphAsymmErrors );
  PASSTHROUGH_PLOTFUNC( PlotPdf,   RooAbsPdf,  TGraph );
  /** @} */

#undef PASSTHROUGH_PLOTFUNC

private:
  void CheckPadAxis( const TObject* );

  void _init_margin();
};


}

}



#endif
