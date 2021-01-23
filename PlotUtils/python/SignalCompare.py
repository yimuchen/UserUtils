import ROOT
from . import *


def SignalCompare1D(sample_config, hist_config):
  """
    Here we are defining the samples to be python list of python dictionaries
    that each represents a sample. The sample dictionary should be in the form:
    {
      "title" : str("Sample title that should appear in the legend"),
      "file" : str("Single ROOT file that contains the histogram"),
      "color": str("sample color")
    },
    For histograms, we should expect the sample to also be a list of histograms,
    that defines the following format:
    {
      "key": "Object key to extract the histogram in each sample file"
      "xtitle" : "Title of the X axis",
      "ytitle" : "Overwriting the title"
    }
    """
  for hist in hist_config:
    canvas = plt.Simple1DCanvas()

    for sample in sample_config:
      hist = ROOT.TFile(sample['file']).Get(hist['key'])
      canvas.Pad().PlotObj(hist, "HIST SAME")
    canvas.SaveAsPDF('MYTEST.pdf')
