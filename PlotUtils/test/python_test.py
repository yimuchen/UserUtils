#!/bin/env python
from UserUtils.PlotUtils import plt
import ROOT
#print(UserU)
c = plt.Simple1DCanvas()

hist = ROOT.TH1D("mytest","mytest",1,0,1)
c.Pad().PlotHist(hist,plt.PlotType(plt.hist),plt.EntryText("mytest"))
c.SaveAsPDF("MYTEST.pdf")
# SignalCompare1D(1,2);
#c.SaveAsPDF("MYTEST.pdf")