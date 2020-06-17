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

The idea of the standardized plotting procedure is as such: for each physics
“process” used in the analysis, their background, signal or data, will generate a
bunch of histograms based on some common analysis algorithm, saved in similar
formats with in different files. The plotting routine then simply looks into a
configuration file, retrieves the required histograms required a certain plot,
run some basic calculations such as rescaling, and plot the background, signal
and data together in a plot. A “process” to this routine is then just a file
containing the relevant histogram objects, and a bunch of variables adjusting how
the histogram should be scaled and how to present the data (in the legend). The
user will also need to define which histograms should be used in which
processes.

Having this picture in mind, the construction of a standardized plots in the form
of a `usr::plt::fmt::BatchRequest` class includes the following ingredients:

- A list of histogram requests
- A set of processes as backgrounds sorted into groups
- A list of processes as data (can be omitted)
- A list of processes as signal to be displayed (can be omitted)
- A list of uncertainties to use for plotting uncertainties.
- A set of IO options, such as where the read the files, where to place the output
  file) ( can be omitted )

This is defined using an input JSON file, using the following format:

```json
{
  "Plot List": [ {HistRequest:1}, {HistRequest:2} ],
  "Uncertainties": [ {Uncertainty:1}, {Uncertainty:2} ],
  "Data sets" : {
    "Display": "Data",
    "processes":[{process: data1},{ process: data2}]
  },
  "Signal Processes": [ {process:signal1} ,{process:signal2} ],
  "Background Processes": [
    {group:background_group1},
    {group:background_group2}
  ]
}
```

Each of the object describe in a parenthesis matches an object in the C++ class:

### Defining a process (`usr::plt::fmt::Process`)

A stated to before, the process class defines the file in which to retrieve
histogram files, some flags to define how the additional calculation is being
processes (think the cross section of the process and additional scale factors),
and how the data should be presented on the graph (labels, color and such).

And example of the constructing a simulated process in a JSON format is as followed:

```json
{
  "display": "t*#bar{t}* (M_{t*}=100TeV)",
  "latex": "\tstarpair (M_\tstar = 100\TeV)",
  "cross section": [ 10, 0.072, 0.072 ],
  "scaling": 1.0,
  "generator": "\\POWHEG",
  "cross section source": "NNLO",
  "luminosity" : 1000,
  "key prefix": "signal_",
  "File": "sig.root",
  "Color": "navy",
}
```

The `display` string is the ROOT flavored Latex string that is used to display in
the legend, of the plot, while the `latex` string is simply latex commands that
want in a standard latex table generation. The cross section are in units of
picobarns, the usual standard in CMS analysis. It is worth noting that the
luminosity here refers to the **effective luminosity** of the sample used to
produce these histograms (i.e. the luminosity needed to reach to total
(unfiltered) number of events used to generate the histogram). Apart from
explicitly specifying the effective luminosity here, the program also looks for a
@ROOT{TTree} of the name `Count` in the same file, and looks for a leaf entry
called `OriginalEvents` to calculate the effective luminosity. To use this
functionality, omit the `luminosity` entry in the json file. For more detail
which entries can be omitted from the declaration, see the constructor for the
class usr::plt::fmt::Process.

Another example of defining a process is that for a data process, which will look
something like:

```json
{
  "display": "Run2077M",
  "latex": "/SingleHiggs/Run2077D/03Feb2017-v1",
  "luminosity": 12.34,
  "run range" : [100000,120000],
  "file": "data1.root"
}
```

In this use case, the luminosity is the actual luminosity corresponding to the
data process of interest.

### Defining a group of processes (`usr::plt::fmt::ProcessGroup`)

When defining background processes, it is usually too verbose to display all
processes individually (think a single background process split up into multiple
HT binned samples to increase the number of events in the analysis). The same
goes for data: data for different eras might need to be processed slightly
differently, so it makes sense to split them into different files, yet typically
we want to display the data as a single entry of in the plot. For this, we define
the `usr::plt::fmt::ProcessGroup` class, which essentially a group of processes
is tallied up essentially as a single entity, sharing the same plotting style and
group and entry name. To define a process group would look something like:

```json
{
  "display": "Y+Jets",
  "latex": "Y+Jets",
  "color": "#FF0000",
  "processes": [
    {
      "display": "Y+Jets (H_{T} < 10TeV)",
      "latex": "Y+Jets, \\HT$\\in$ [0,10)\\TeV",
      "cross section": [ 200,   2.2, 2.3],
      "scaling": 1.21,
      "generator": "\\MADGRAPH",
      "cross section source": "LO",
      "file": "YJets_10HT.root"
    },
    {
      "display": "Y+jets (10 #leq H_{T} #leq 20 TeV)",
      "latex": "\\Y+Jets, \\HT$\\in$ [10,20)\\TeV",
      "cross section": [ 120, 5.5, 6.0],
      "scaling": 1.0,
      "generator": "\\MADGRAPH",
      "cross section source": "LO",
      "file": "YJets_20HT.root"
    }
  ]
}
```

Notice that the color defined here will proceed any color defined in an
individual process within a process group. If you look closely the in the first
JSON fragment, you will see the declaration of the data processes in the master
`BatchRequest` construction is in fact the declaration of a `ProcessGroup`.

### Defining the Histograms to be plotted (`usr::plt::fmt::HistRequest`)

The definition of the histogram to plot is simply:

```json
{
  "name": "JetPt",
  "xaxis": "Jet p_{T}",
  "units": "GeV",
  "yaxis": "Number of Jets",
  "filekey": "JetPt",
  "logy" : false
}
```

Here we can see the have the ability to reformat the plot entries such as the x
and y-axis names into something more human-readable. We also have the option to
generate a semi-log plot. Unfortunately, the binning cannot be changed at this
stage. Notice that the corresponding output file will be saved to `name` entry
for a histogram request, by default the histogram extracted from each root file
will have the corresponding `filekey` entry, which will match the `name` entry if
not specified.

---

As this point we can get a simple overview of the what a typical standard
plotting process will do, First for a single histogram entry (let say,
name=`JetPt`) a bunch of histograms is defined:

```c++
signal_hist = signal_file->Get("JetPt")->Scale(...);

background_1   = background_group1_process1->Get("JetPt")->Scale(...);
background_1->Add( background_group1_process2->Get("JetPt")->Scale(...) );
background_1->Add( background_group1_process3->Get("JetPt")->Scale(...) );
background_1->Add( background_group1_process4->Get("JetPt")->Scale(...) );
...
background_2   = background_group1_process1->Get("JetPt")->Scale(...);
background_2->Add( background_group1_process2->Get("JetPt")->Scale(...) );
background_2->Add( background_group1_process3->Get("JetPt")->Scale(...) );
background_2->Add( background_group1_process4->Get("JetPt")->Scale(...) );

...

data = data_1->Get("JetPt");
data->Add( data_2->Get("JetPt") );
```

For the plotting the various background histograms are plotted as a stacked
histogram *in reverse order*, as the background processes are expected to be
listing in most-to-least-significant ordering. Then the signal histogram is
plotted as a simple histogram super imposed over the data. The data histogram is
finally plotted as a standard scatter plot. After plotting this sequence, the
data/background ratio plot is also plotted on a separate pad. In terms of the
objects in our custom plotting library, the plotting procedure would be
something like:

```c++
usr::plt::Ratio1DCanvas c;
c.PlotHist( background_4,
            usr::plt::PlotType( usr::plt::stack ),
            usr::plt::FillColor( background_group4.color ),
            usr::plt::EntryText( background_group4.display ) ) ;
c.PlotHist( background_3,
            usr::plt::PlotType( usr::plt::stack ),
            usr::plt::FillColor( background_group3.color ),
            usr::plt::EntryText( background_group3.display ) ) ;
...

c.PlotHist( signal_hist,
            usr::plt::PlotType( usr::plt::hist ),
            usr::plt::LineColor( signal.color ),
            usr::plt::EntryText( signal.display ) ) ;
c.PlotHist( data,
            usr::plt::PlotType( usr::plt::scatter ),
            usr::plt::EntryText( data.display ) ) ;

c.PlotRatio( data, background_total );
```

The program is designed to automatically handle the scaling of the various
histograms given the user input in the JSON file, as well as ensuring uniform
data styling according to typical publication recommendation. One thing not yet
address the inclusion of uncertainty which we will introduce now:

## Defining uncertainties for background plotting

Typically, uncertainties are split into two types: shaped and normalization. Here
we are unifying them into simply 1 type: shaped uncertainties. Essentially what
we do here is for each source of uncertainty, we generate a different background
template and use this to calculate help calculate the total uncertainty.
Declarations of uncertainty sources will look something like:

```json
"Uncertainties": [
  {
    "name": "Lepton Weighting",
    "Key name": "Lep"
  },
  {
    "name" : "Luminosity",
    "Norm uncertainty" : [0.025,0.026]
  }
]
```

In the case where a `key name` is provided for an uncertainty source, like with
the lepton weighting uncertainty above, the plotting algorithm will look for
histograms with the names `histogram_name + uncertainty_key + "Up"` and
`histogram_name + uncertainty_key + "Down"`, and use the histograms to estimate
the uncertainty of bin contents. If only the `norm uncertainty` is tag provided,
then alternative histogram templates are generated simply by scaling the central
histogram by the normalization uncertainties given. The calculation of the
uncertainties displayed are calculated bin-by-bin. Uncertainties of the same
source across different processes are assumed to have 100% correlation.
Uncertainties from different sources are assumed to have no correlation, and for
asymmetric uncertainty numbers are calculated using the function provided in the
MathUtils package.

## Standard binary

If you are not going to customize the plotting procedure yourself, a standard
binary is provided in the form of `usr_PlotStandard` by the package. Additional
plotting routines are also possible and see the documenation of the binary file
and the BatchRequest class for more documentation. An example of dummy files
generate for plotting tests can be found in the testing binaries. And below is an
example plot generated using the standard binary:

<div class="plot_example">
<img src="image/std_plot.png"/>

<ul>
  <li><a href="image/std_plot.png">72dpi output</a></li>
  <li><a href="image/std_plot_highres.png">300dpi output</a></li>
  <li><a href="image/std_plot.pdf">PDF output</a></li>
  </ul>
</div>

---

## DISCLAIMER

This package is currently very much in flux, and is required to

