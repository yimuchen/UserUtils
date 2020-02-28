# Standardized Plotting binary file

```json
{
  "Plot List" : [
    {
      "name": "JetPt",
      "xaxis" : "Jet p_{T}",
      "xunit" : "GeV",
      "yaxis" : "Number of Jets",
      "filekey": "JetPtSmeared",
    }
  ],
  "Shape Uncertainties" : [
    "ELEC", "MUON", "PILEUP",
  ],
  "Signal Processes" : [
        {
          "Display": "t*#bar{t}* ",
          "Latex": "\\PW+Jets, \\HT$\\in$ [400,600)\\GeVc",
          "Cross Section": [ 48.91 , 0.072, 0.072 ],
          "Scaling": 1.0,
          "Generator": "\\POWHEG",
          "Cross Section Source": "NNLO",
          "File": "ttbar_*.root"
        },
  ],
  "Background Processes" : [
    {
      "Display": "W+Jets",
      "Latex": "\\PW+Jets",
      "Color": "cyan",
      "processes" : [
        {
          "Display": "W+jets ( 200 #leq H_{T} #leq 400)",
          "Latex": "\\PW+Jets, \\HT$\\in$ [200,400)\\GeVc",
          "Cross Section": [ 0.2043,0.0020,0.0020 ],
          "Scaling": 1.0,
          "K Factor":   1.21,
          "Generator": "\\MADGRAPH",
          "Cross Section Source": "LO",
          "File": "WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_*.root"
        },
        {
          "Display": "W+jets (400 #leq H_{T} #leq 600)",
          "Latex": "\\PW+Jets, \\HT$\\in$ [400,600)\\GeVc",
          "Cross Section": [ 48.91 , 0.072, 0.072 ],
          "Scaling": 1.0,
          "K Factor":   1.21,
          "Generator": "\\MADGRAPH",
          "Cross Section Source": "LO",
          "File": "WJetsToLNu_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_*.root"
        }
     ]
    },{
      "Display": "t#bar{t}+Jets",
      "Latex": "\\ttbar+Jets",
      "Color": "cyan",
      "processes" : [
        {
          "Display": "t#bar{t}+jets",
          "Latex": "\\PW+Jets, \\HT$\\in$ [400,600)\\GeVc",
          "Cross Section": [ 48.91 , 0.072, 0.072 ],
          "Scaling": 1.0,
          "Generator": "\\POWHEG",
          "Cross Section Source": "NNLO",
          "File": "ttbar_*.root"
        }
      ]
    }
  ]
}
```
