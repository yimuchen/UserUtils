# Standardized Plotting binary file

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
          "display": "t*#bar{t}* ",
          "latex": "\\PW+Jets, \\HT$\\in$ [400,600)\\GeVc",
          "cross section": [ 48.91 , 0.072, 0.072 ],
          "scaling": 1.0,
          "generator": "\\POWHEG",
          "cross section source": "NNLO",
          "file": "ttbar_*.root"
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
