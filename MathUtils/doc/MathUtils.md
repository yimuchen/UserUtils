@defgroup MathUtils MathUtils
@brief Collection of common mathematical calculation routines used in HEP
       analysis.

This is a collection of functions and classes used for the mathematical routines
commonly used in the HEP analysis that are not immediately available in existing
libraries, the main ones being the calculation of uncertainties of measurements
with arithmetics, and the Kolmogorov-Smirnov test weighted data sets. This
library uses the ROOT::Math classes for running the mathematical function, which
allows for object-like extensions for mathematical functional objects (instead of
pure function pointer interfaces like GSL), while keeping away from the ambiguity
of and compile-time unsafe string based math function declaration of “high-level”
ROOT objects (like @ROOT{TF1}).

