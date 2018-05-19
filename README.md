# UserUtils {#mainpage}

This is a collection of utility classes and functions intended to enhance the analysis experience in CMSSW environment.
The goal is to isolate commonly used analysis variables, statistics subroutines, and computing tools in general into easy to access packages within the CMSSW framework.

---

# Quick overview

## [Common](./Common/)

A bunch of functions and minimal classes designed to enhance usage experience
of the Standard Template Library, the Boost library.
Highlights of this sub-package includes:

-   Added ostream objects
-   better floating point/scientific formatting of doubles.
-   std::filesystem specialisations for CMSSW programming environment (unix
    globbing, CMSSW file paths.)
-   Command line interaction functions.
-   boost::property_tree wrappers
-   boost::program_options wrappers.
-   Concrete type maths functions not listed in STL.

## [EDMUtils](./EDMUtils/)

A bunch of functions for interactions with EDM files and enhancing the used
of EDM function and classes in EDM analysis general.
Highlights of this package include:

-   a new Counter class, aim for taking over the edm::MergeableCounter class to include negative weighted events.
-   Scripts for dumping EDM File information.
-   Plugins for calculating commonly used run-wide cached (PDF weights and
    weight summing)

## [MathUtils](./MathUtils/)

Additional maths library for more complicated statistical operations.
Highlights of this package include

-   General GSL functions for computing minos uncertainties.
-   a Measurement class to represent a measurement with uncertainties, including its basic (crude) arithmetic operations and string formatting.
-   Proper execution of the Kolmogorov--Smirnov test of RooFit objects.

## [PhysUtils](./PhysUtils/)

Additional physics function for the handling of physics object stored in the
EDM files. Note that due the the ever changing recommendations, this subpackage
would most likely need manual editing for the analysis needs.
Highlights of this package includes:

-   Simplified B tag scale factor accessor.
-   EDM object flavoured MC truth topology crawling
-   EDM object flavoured trigger matching (includes globbing support)
-   four momentum caching functions
-   Extended physics vaiable calculations.

## [PlotUtils](./PlotUtils)

Plotting classes and function for a better batch plotting scripts, while still leaving all ROOT functions available.
Hightlights of this package includes:

-   Constants for more intuitive declaration of plotting objects and styles.
-   new Canvas class (inherited from TCanvas),  to contain strictly pads. Objects on now plotted on the pads, instead of individual Draw calls
-   Specialised classes for 1D plotting.

---

# Code style overview

-   The namespace used in this package will always be `usr` if present.
-   Header files will always exists in the `interface` directory for the functions and classes with the `.hpp` extension. The only exception are EDM plugins classes, where a standalone header file will be omitted.
-   Source code file will always have the extension of `.cc`. Implementation of template functions would be stored in `.ipp` files.
-   Class definition, public member functions, and function are in "CamelCase" format. The only exception are if the class for function are intended to be used in tandem to STL objects only. In which case they would follow the stl convention of all_lowercase_with_underscore.
-   Functions would have inline detailed documentation in their own header files.
-   No comment is the best comment! I try and keep the control flow as intuitive
    as I can, and keep all comment at the beginning of the function and avoid inline comments as much as possible.
