@defgroup EDMUtils EDMUtils
@brief EDMUtils

A bunch of functions for interactions with EDM files and enhancing the use of EDM
function and classes in EDM analysis general. These include:

- new exectuable function for ease-of-access to the information in the EDM
  files. This includes:
  - [DumpLHERunInfo](@ref DumpLHERunInfo.cc) Dumping the generation
      information on screen for user inspection.
  - [DumpLumiCount.py](@ref DumpLumiCount.py) Dumping the event, lumisection
      and run count of a list of edm files.
  - [DumpGenParticles.cc](@ref DumpGenParticles.cc) Dumping the generator
    topology of a certain event.

- classes aimed to enhance the usability of edm plugins, including
  - usr::Counter: A new type to be understood by the framework to allow for run
    level variable caching
  - usr::PluginAlias: An intermediate class aiming to reduce the verbosity of
    plugin methods.

- Standalone plugins for common calculation independent of analysis needs,
  such as weight summing. Including:
  - GenWeightCounter: for summing and caching generation uncertainty weights.
  - WeightProdSum: for calculating the product of a set of weights in an
    event and taking the sum for a run level cache.

As the package name implies, these features require the EDM framework to
function, and thus will not be compiled outside of the CMSSW environment.
