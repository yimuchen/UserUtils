@defgroup RootFit RootFit

@details The fitting routine in vanilla ROOT is rather unwieldy, with the use of
non-descriptive single character based string literals used to specify the fit
routine, while also having the fitting routines for different classes use
different strings, all in all making the code overly non-verbose. The functions
provided here wrap up the vanilla ROOT fitting routines into a More RooFit like
experience with a the fitting routines specified via list of arguments.

A few general notes:

- All functions will return the fit results in the form of a TFitResult object.
  This avoids ambiguity with vanilla ROOT, where a global object is used to keep
  track of the latest fit result.
- Options for storing the fit function into the data object will not be exposed,
  as this should be something to avoided in favor of the better plotting
  interface provided in the PlotUtils subpackage.
- The function will always expect the fit functions to be provided in the form a
  TF1 derived object, as defining functions with string literals is very
  problematic and difficult to debug. This package always wants to promote good
  coding styling rather than quick-and-dirty methods used for one-off scripts.
- Functions will all use reference interfaces, as this is typically a better way
  to write modern C++ code.