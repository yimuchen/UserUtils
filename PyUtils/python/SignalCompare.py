# This is a test
import ROOT

def SignalCompare1D(hist_config,sample_config):
  """
  Function for signal comparison plots.
  """
  print(hist_config,sample_config)
  print(ROOT.usr.plt)
  c = ROOT.usr.plt.Single1DCanvas()
  c.SaveAsPDF("MYTEST");


