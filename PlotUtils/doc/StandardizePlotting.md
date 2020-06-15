@defgroup StandardizedPlot Standardized Plotting routine
@ingroup PlotUtils
@brief Standardized plotting routine for analysis presentation

In high energy physics, we will have some form of standardized plotting used
throughout the analysis. A set of common variable histograms are created for each
simulated and data samples, weighted according to cross sections and various
correction factors, the background will typically be a stacked histogram with
uncertainties, the data is plotted as scatter plots.

This part of the library aims to allow the user the list the processes as in a
JSON file, and allow the generation of the standardized plots with each MC sample
properly weighted according to their cross section and have their common
uncertainties properly tallied. This allows for consistencies between the various
plots that are displayed in the analysis, as well as attempting to segregate the
code needed for calculating analysis specific processes, and the common aesthetic
and simple error propagation routines.

## The construction of standardized plot request

```json
{
  "plots" : [
    {
      "name": "JetPt",
      "xaxis" : "Jet p_{T}",
      "xunit" : "GeV",
      "yaxis" : "Number of Jets",
      "filekey": "JetPtSmeared",
    }
  ],
  "uncertainties" : [
    "ELEC", "MUON", "PILEUP",
  ],
  "signals" : [
        {
          "display": "t*#bar{t}*",
          "latex": "\\t^{*}\\bar{t}^{*} TESTING",
          "cross section": [ 48.91 , 0.072, 0.072 ],
          "scaling": 1.0,
          "generator": "\\POWHEG",
          "cross section source": "NNLO",
          "file": "tstar_*.root"
        },
  ],
  "Background" : [
    {
      "display": "W+Jets",
      "latex": "\\PW+Jets",
      "color": "cyan",
      "processes" : [
        {
          "display": "W+jets ( 200 #leq H_{T} #leq 400)",
          "latex": "\\PW+Jets, \\HT$\\in$ [200,400)\\GeVc",
          "cross section": [ 0.2043,0.0020,0.0020 ],
          "scaling": 1.0,
          "generator": "\\MADGRAPH",
          "cross section source": "LO",
          "file": "WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_*.root"
        },
        {
          "display": "W+jets (400 #leq H_{T} #leq 600)",
          "latex": "\\PW+Jets, \\HT$\\in$ [400,600)\\GeVc",
          "cross section": [ 48.91 , 0.072, 0.072 ],
          "scaling": 1.0,
          "generator": "\\MADGRAPH",
          "cross section source": "LO",
          "file": "WJetsToLNu_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_*.root"
        }
     ]
    },{
      "display": "t#bar{t}+Jets",
      "latex": "\\ttbar+Jets",
      "color": "cyan",
      "processes" : [
        {
          "display": "t#bar{t}+jets",
          "latex": "\\PW+Jets, \\HT$\\in$ [400,600)\\GeVc",
          "cross section": [ 48.91 , 0.072, 0.072 ],
          "scaling": 1.0,
          "generator": "\\POWHEG",
          "cross section source": "NNLO",
          "file": "ttbar_*.root"
        }
      ]
    }
  ]
}
```
