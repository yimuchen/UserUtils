import ROOT
import cppyy
import os

if "CMSSW_BASE" not in os.environ:  # This is a standalone compile
  file_path = os.path.abspath(__file__)
  base_path = os.path.dirname(os.path.dirname(os.path.dirname(file_path)))
  cppyy.load_library(base_path + "/lib/libPlotUtils.so")
  cppyy.load_reflection_info(base_path + "/python/libpyPlotUtils.so")

## Renaming all the stuff to use a flatter interface
# Unfortunately, the contents of cppyy userspace is dynamically generated, so
# there is not easy way for programmatically running this renaming scheme.
Simple1DCanvas = cppyy.gbl.usr.plt.Simple1DCanvas
Ratio1DCanvas = cppyy.gbl.usr.plt.Ratio1DCanvas
Flat2DCanvas = cppyy.gbl.usr.plt.Flat2DCanvas
CommonXCanvas = cppyy.gbl.usr.plt.CommonXCanvas

Pad1D = cppyy.gbl.usr.plt.Pad1D
Pad2DFlat = cppyy.gbl.usr.plt.Pad2DFlat

PlotUnder = cppyy.gbl.usr.plt.PlotUnder
ExtryText = cppyy.gbl.usr.plt.EntryText
TextColor = cppyy.gbl.usr.plt.TextColor
TextSize = cppyy.gbl.usr.plt.TextSize
TextAngle = cppyy.gbl.usr.plt.TextAngle
TextAlign = cppyy.gbl.usr.plt.TextAlign
LineColor = cppyy.gbl.usr.plt.LineColor
LineStyle = cppyy.gbl.usr.plt.LineStyle
LineWidth = cppyy.gbl.usr.plt.LineWidth
FillColor = cppyy.gbl.usr.plt.FillColor
FillStyle = cppyy.gbl.usr.plt.FillStyle
MarkerColor = cppyy.gbl.usr.plt.MarkerColor
MarkerStyle = cppyy.gbl.usr.plt.MarkerStyle
MarkerSize = cppyy.gbl.usr.plt.MarkerSize
PlotType = cppyy.gbl.usr.plt.PlotType
Plot2DF = cppyy.gbl.usr.plt.Plot2DF
ScaleY = cppyy.gbl.usr.plt.ScaleY

VisualizeError = cppyy.gbl.usr.plt.VisualizeError
ExtrapolateInRatio = cppyy.gbl.usr.plt.ExtrapolateInRatio

plottype = cppyy.gbl.usr.plt.plottype
plot2df = cppyy.gbl.usr.plt.plot2df

col = cppyy.gbl.usr.plt.col
length =  cppyy.gbl.usr.plt.len
sty = cppyy.gbl.usr.plt.sty

internal = cppyy.gbl.usr.plt # Adding an internal reference to

