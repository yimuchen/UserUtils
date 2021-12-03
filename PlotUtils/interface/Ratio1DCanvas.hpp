/**
 * @file    Ratio1DCanvas.hpp
 * @brief   Canvas containing two Pad1Ds, with the top for main plotting
 *          while the bottom is designed for ratio plotting.
 * @author  [Yi-Mu "Enoch" Chen](https://github.com/yimuchen)
 * @details
 *
 */

#ifndef USERUTILS_PLOTUTILS_RATIO1DCANVAS_HPP
#define USERUTILS_PLOTUTILS_RATIO1DCANVAS_HPP

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

/**
 * @brief Constructor container class for defining Pad dimension ratios
 */
struct PadRatio
{
  static float default_ratio;
  static float default_gap;
  PadRatio( const float _ratio = default_ratio,
            const float _gap   = default_gap ) :
    ratio( _ratio ),
    gap  ( _gap ){}
  const float ratio;
  const float gap;
};

/**
 * @brief Specialized Pad1D for additional axis/font settings.
 */
class Top1DPad : public Pad1D
{
public:
  // Constructors must be redeclared
  Top1DPad( Canvas* c, const PadSize& x, const RangeByVar& y )
    : Pad1D( c, x, y ){}
  Top1DPad( Canvas* c, const PadSize& x, const double y, const double z )
    : Pad1D( c, x, y, z ){}
  Top1DPad( Canvas* c, const PadSize& x )
    : Pad1D( c, x ){}
  virtual
  ~Top1DPad (){}

  virtual void SetAxisFont();
};

/**
 * @brief Specialized Pad1D for additional axis/font settings.
 */
class Bottom1DPad : public Pad1D
{
public:
  // Constructors must be redeclared
  Bottom1DPad( Canvas* c, const PadSize& x, const RangeByVar& y )
    : Pad1D( c, x, y ){}
  Bottom1DPad( Canvas* c, const PadSize& x, const double y, const double z )
    : Pad1D( c, x, y, z ){}
  Bottom1DPad( Canvas* c,  const PadSize& x )
    : Pad1D( c, x ){}
  virtual
  ~Bottom1DPad(){}

  virtual void SetAxisFont();
};

/*-----------------------------------------------------------------------------
 *  Main class
   --------------------------------------------------------------------------*/
class Ratio1DCanvas : public Canvas
{
public:
  static TH1D* ScaleDivide( const TH1D*  num,
                            const TH1D*  den,
                            const double cen = 1. );

  static TH1D* ScaleDivide( const TH1D*   num,
                            const TGraph* den,
                            const double  cen         = 1.,
                            const bool    extrapolate = false );

  static TGraphAsymmErrors* ScaleDivide( const TGraph* num,
                                         const TGraph* den,
                                         const double  cen         = 1.,
                                         const bool    extrapolate = false );

  static TGraphAsymmErrors* PullDivide( const TGraph* num,
                                        const TGraph* den,
                                        const double  cen = 0 );

  static length_t default_width;
  static length_t default_height;
  static FontSet  default_font;

public:
  Ratio1DCanvas( const length_t width  = default_width,
                 const length_t height = default_height,
                 const PadRatio&       = PadRatio(),
                 const FontSet&        = default_font );
  Ratio1DCanvas( const RangeByVar&,
                 const length_t width  = default_width,
                 const length_t height = default_height,
                 const PadRatio&       = PadRatio(),
                 const FontSet&        = default_font );
  virtual ~Ratio1DCanvas();

  /** @brief returning reference to top pad object */
  inline Top1DPad&
  TopPad(){ return GetPad<Top1DPad>( 0 ); }

  /** @brief returning reference to bottom pad object */
  inline Bottom1DPad&
  BottomPad(){ return GetPad<Bottom1DPad>( 1 ); }

  /**
   * @{
   * @brief 'plotting data on canvas' is actually plotting to Top pad on canvas.
   */
  template<typename ... Args>
  inline TH1D&
  PlotHist( TH1D& x, Args ... args )
  {
    auto& ans = TopPad().PlotHist( x, args ... );
    MakeBottomAxis();
    return ans;
  }


  template<typename ... Args>
  inline TH1D&
  PlotHist( TH1D* x, Args ... args )
  {
    auto& ans = TopPad().PlotHist( x, args ... );
    MakeBottomAxis();
    return ans;
  }


  template<typename ... Args>
  inline TGraph&
  PlotGraph( TGraph& x, Args ... args )
  {
    auto& ans = TopPad().PlotGraph( x, args ... );
    MakeBottomAxis();
    return ans;
  }


  template<typename ... Args>
  inline TGraph&
  PlotGraph( TGraph* x, Args ... args )
  {
    auto& ans = TopPad().PlotGraph( x, args ... );
    MakeBottomAxis();
    return ans;
  }


  template<typename ... Args>
  inline TGraph&
  PlotFunc( TF1& x, Args ... args )
  {
    auto& ans = TopPad().PlotFunc( x, args ... );
    MakeBottomAxis();
    return ans;
  }


  template<typename ... Args>
  inline TGraph&
  PlotFunc( TF1* x, Args ... args )
  {
    auto& ans = TopPad().PlotFunc( x, args ... );
    MakeBottomAxis();
    return ans;
  }


  template<typename ... Args>
  inline TEfficiency&
  PlotEff( TEfficiency& x, Args ... args )
  {
    auto& ans = TopPad().PlotEff( x, args ... );
    MakeBottomAxis();
    return ans;
  }


  template<typename ... Args>
  inline TEfficiency&
  PlotEff( TEfficiency* x, Args ... args )
  {
    auto& ans = TopPad().PlotEff( x, args ... );
    MakeBottomAxis();
    return ans;
  }


  template<typename ... Args>
  inline TGraphAsymmErrors&
  PlotData( RooAbsData& x, Args ... args )
  {
    auto& ans = TopPad().PlotData( x, args ... );
    MakeBottomAxis();
    return ans;
  }


  template<typename ... Args>
  inline TGraphAsymmErrors&
  PlotData( RooAbsData* x, Args ... args )
  {
    auto& ans = TopPad().PlotData( x, args ... );
    MakeBottomAxis();
    return ans;
  }


  template<typename ... Args>
  inline TGraph&
  PlotPdf( RooAbsPdf& x, Args ... args )
  {
    auto& ans = TopPad().PlotPdf( x, args ... );
    MakeBottomAxis();
    return ans;
  }


  template<typename ... Args>
  inline TGraph&
  PlotPdf( RooAbsPdf* x, Args ... args )
  {
    auto& ans = TopPad().PlotPdf( x, args ... );
    MakeBottomAxis();
    return ans;
  }


  /** @} */

  /**
   * @{
   * @brief Common plot styling functions are also setting top pad properties
   *        only.
   */
  template<typename ... Args>
  inline void DrawCMSLabel( Args ... args )
  { TopPad().DrawCMSLabel( args ... );}
  template<typename ... Args>
  inline void DrawLuminosity( Args ... args )
  { TopPad().DrawLuminosity( args ... ); }
  inline void SetLogy( const int x = 1 ){ TopPad().SetLogy( x );}
  /** @} */

  /**
   * @{
   * @brief plotting the scale division of two histogram onto bottom pad,
   * returning reference to the newly constructed histogram. Only providing
   * double reference and double pointer interfaces.
   */
  TH1D& PlotScale( const TH1D&, const TH1D&, const std::vector<RooCmdArg>& );
  inline TH1D&
  PlotScale( const TH1D& x, const TH1D& y ){ return PlotScale( x, y, {} ); }
  inline TH1D&
  PlotScale( const TH1D* x, const TH1D* y, const std::vector<RooCmdArg>& l )
  { return PlotScale( *x, *y, l ); }
  inline TH1D&
  PlotScale( const TH1D* x, const TH1D* y ){ return PlotScale( *x, *y, {} ); }
  template<typename ... Args>
  inline TH1D&
  PlotScale( const TH1D&      x,
             const TH1D&      y,
             const RooCmdArg& arg1,
             Args ... args )
  { return PlotScale( x, y, MakeVector<RooCmdArg>( arg1, args ... ) ); }
  template<typename ... Args>
  inline TH1D&
  PlotScale( const TH1D*      x,
             const TH1D*      y,
             const RooCmdArg& arg1,
             Args ... args )
  { return PlotScale( x, y, MakeVector<RooCmdArg>( arg1, args ... ) ); }
  /** @} */

  /**
   * @{
   * @brief plotting the scale division of a histogram and a graph onto bottom
   * pad, returning reference to the newly constructed histogram.
   */
  TH1D& PlotScale( const TH1D&, const TGraph&, const std::vector<RooCmdArg>& );
  inline TH1D&
  PlotScale( const TH1D& x, const TGraph& y ){ return PlotScale( x, y, {} ); }
  inline TH1D&
  PlotScale( const TH1D* x, const TGraph* y, const std::vector<RooCmdArg>& l )
  { return PlotScale( *x, *y, l ); }
  inline TH1D&
  PlotScale( const TH1D* x, const TGraph* y ){ return PlotScale( *x, *y, {} ); }
  template<typename ... Args>
  inline TH1D&
  PlotScale( const TH1D&      x,
             const TGraph&    y
             ,
             const RooCmdArg& arg1,
             Args ... args )
  { return PlotScale( x, y, MakeVector<RooCmdArg>( arg1, args ... ) ); }
  template<typename ... Args>
  inline TH1D&
  PlotScale( const TH1D*      x,
             const TGraph*    y
             ,
             const RooCmdArg& arg1,
             Args ... args )
  { return PlotScale( x, y, MakeVector<RooCmdArg>( arg1, args ... ) ); }
  /** @} */

  /**
   * @{
   * @brief plotting the scale division of two graphs onto bottom pad,
   * returning reference to the newly constructed histogram.
   */
  TGraphAsymmErrors& PlotScale( const TGraph&,
                                const TGraph&
                                ,
                                const std::vector<RooCmdArg>& );
  inline TGraphAsymmErrors&
  PlotScale( const TGraph& x, const TGraph& y ){ return PlotScale( x, y, {} ); }
  inline TGraphAsymmErrors&
  PlotScale( const TGraph* x, const TGraph* y, const std::vector<RooCmdArg>& l )
  { return PlotScale( *x, *y, l ); }
  inline TGraphAsymmErrors&
  PlotScale( const TGraph* x, const TGraph* y )
  {
    return PlotScale( *x, *y, {} );
  }
  template<typename ... Args>
  inline TGraphAsymmErrors&
  PlotScale( const TGraph&    x,
             const TGraph&    y
             ,
             const RooCmdArg& arg1,
             Args ... args )
  { return PlotScale( x, y, MakeVector<RooCmdArg>( arg1, args ... ) ); }
  template<typename ... Args>
  inline TGraphAsymmErrors&
  PlotScale( const TGraph*    x,
             const TGraph*    y
             ,
             const RooCmdArg& arg1,
             Args ... args )
  { return PlotScale( x, y, MakeVector<RooCmdArg>( arg1, args ... ) ); }
  /** @} */

  TGraphAsymmErrors& PlotPull( const TGraph&,
                               const TGraph&
                               ,
                               const std::vector<RooCmdArg>& );
  inline TGraphAsymmErrors&
  PlotPull( const TGraph& x, const TGraph& y ){ return PlotPull( x, y, {} ); }
  inline TGraphAsymmErrors&
  PlotPull( const TGraph* x, const TGraph* y, const std::vector<RooCmdArg>& l )
  { return PlotPull( *x, *y, l ); }
  inline TGraphAsymmErrors&
  PlotPull( const TGraph* x, const TGraph* y ){ return PlotPull( *x, *y, {} );}
  template<typename ... Args>
  inline TGraphAsymmErrors&
  PlotPull( const TGraph&    x,
            const TGraph&    y
            ,
            const RooCmdArg& arg1,
            Args ... args )
  { return PlotPull( x, y, MakeVector<RooCmdArg>( arg1, args ... ) ); }
  template<typename ... Args>
  inline TGraphAsymmErrors&
  PlotPull( const TGraph*    x,
            const TGraph*    y
            ,
            const RooCmdArg& arg1,
            Args ... args )
  { return PlotPull( x, y, MakeVector<RooCmdArg>( arg1, args ... ) ); }

  void SetTopMargin( const float x );
  void SetLeftMargin( const float x );
  void SetRightMargin( const float x );
  void SetBottomMargin( const float x );

  void MakeBottomAxis();

protected:
  float _splitNDC( const float ratio ) const;
  void  _init_margin( const float gap );
};

}/* plt  */

}/* usr  */

#endif/* end of include guard: USERUTILS_PLOTUTILS_RATIO1DCANVAS_HPP  */
