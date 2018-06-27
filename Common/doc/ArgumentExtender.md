@class usr::ArgumentExtender
@ingroup Common
@details
Often in analysis programs, we need a short string for process indentification,
but a longer string for the publication/presentation outputs or change a
parameter in calculations calculations. An example of this case would be when
an analysis is split into muon/electron channels, where the calculations are
nearly entirely the same, but require the plotting to put in unique labels
for the two channel, while also potentially using slightly different numbers
in calculation (scale factors, integrated luminosity... etc).  
Such behaviour could be listed in a json file as something like:
something like:

```json
{
  channel:{
    mu:{
      fullname:"#mu channel"
      plotname:"#mu+jets"
      lumi: 1050
    },
    el: {
      fullname:"#it{e} channel"
      plotname: "#it{e}+jets"
      lumi: 1000
    }
  }
}
```

Our class would need to first read the input arguments, and be able to get the
extended processes in a simple method. In the example above, we would be able
to call the program with the options "Options1":

```sh
./my_analysis_program --channel mu
```

And in the analysis code, the extended values could be obtained as:

```cpp
lumi     = args.ArgExt<double>("channel", "lumi"); // 1050
plotname = args.ArgExt<string>("channel","plotname"); // "@mu+jets"
arg     = args.Arg<string>("channel"); // "mu"
```

---

The ArgumentExtender class is a wrapper for:

- `boost::property_tree` to store the json file structure described above
  file.
- `boost::program_options::options_description` instance for defining the
  the the options to be parsed.
- `boost::program_options::variables_map` instance for storing the results
  of the program input options.

Functions are added to simplify the argument parsing process, as well as
getting the extended argument values. Additional parsing rules will be added to
help ensure that the json file formats between options are in sync.

In general, the typical usage of the class should be something like this:

1. Defining the class instance with json file(s). During this step, the parser
   also checks to see that the json file structure is consistent for each of the
   potential options (i.e. they have the same extended variables defined in the
   file.)
2. Defining the options_description for defining the options.
3. Take in the program inputs. During this step, the class will also check to
   see if the option is consistent with the json file (i.e. if the user input
   has its entry corresponding entry in the json file). The options defined
   in the json file will be assumed to be madatory argument.
4. Throughout the program, call the class to get the program inputs and
   extended variables.

For the use-case above. It would look something like:

```cpp
  usr::po::options_description desc("test options");
  desc.add_options()
    ("channel,c",usr::po::value<std::string>())
  ;

  usr::ArgumentExtender myargs("name1.json","name2.json");
  myargs.AddOptions(desc);
  myargs.ParseOptions(argc,argv);

  myargs.ArgExt<double>("channel","lumi");
```
