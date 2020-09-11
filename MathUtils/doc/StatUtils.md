@defgroup StatUtils StatUtils
@brief    Statistical calculation routines using ROOT MathMore library
@ingroup  MathUtils
@details

One of the main routines of this these functions is the calculations of the
uncertainties of estimations/measurements of physics parameters in the
experiment. This is performed by some @NLL(NLL) function which takes the physical
parameters(\f$x_i\f$) and the observed data (\f$d_i\f$) as the input, and
determine how variation in \f$x_i\f$ changes the output of the NLL to decide on
an estimation on the value of the physical parameter, which we are going to
notate as:\f$\hat{x_i}^{+\sigma_+}_{-\sigma_{-}}\f$. Even though ROOT uses GSL
under the hood, we are going to use the ROOT::Math function for calculation,
which allows user to provide functions to the calculations using classes and
functions instead of just using function pointers (which allow the extention to
python more easily).

# Mathematics

While there are multiple interpretation as to how to obtain \f$\hat{x}\f$ and co
from a given negative log-likelihood ([frequentists
inference](https://en.wikipedia.org/wiki/Frequentist_inference) and [Bayesian
inference](https://en.wikipedia.org/wiki/Bayesian_inference)), the library opts
to go for the Minos uncertainty, where the NLL function is minimized at
\f$\mathrm(NLL)(\hat{x})\f$, and is shifted by a certain amount by the parameter
uncertainty:

  \f[
    \mathrm{NLL}(\hat{x}\pm\sigma_{\pm})-\mathrm{NLL}(x) = \frac{1}{2}C^2,
 \f]

where \f$C\f$ is the “sigma interval” that the uncertainty is trying to
represent. In the case that there are multiple parameters are taken into
consideration in a common NLL function \f$\mathrm{NLL}(x_i)\f$, the effective NLL
to be used for a single parameter \f$\mathrm{NLL}_\mathrm{eff}(x_0)\f$ to be used
is would be the “profile NLL” function, where all other parameters are allowed to
change value such that to minimize the total NLL function \f$\mathrm{NLL}\f$ for
a given value of \f$x_0\f$. This scheme also extends to extended measurements of
\f$u=f(x_i)\f$, where we might be interested in the sum, product, or more
complicated parameter which could be calculated based on the parameters directly
obtained in an experiment. In this case, for a given value of \f$u\f$, we would
be find the effective NLL function for our extended parameter \f$u\f$, notated as
\f$ \mathrm{NLL}_\mathrm{eff}(u) \f$, by maximizing the original NLL function
under the constraint of \f$u = f(x_i)\f$. In this library, we would be carrying
out such calculation using the Lagrange multiplier method.

# Container

A container for a measurement results containing a central value, upper and lower
uncertainties is implemented with the Measurement class. Aside for acting as a
simple number container, it also allows for the performance of numerical
operations of measurement results with a quick-and-dirty version of combined
MINOS uncertainty evaluation with an estimated likelihood function (see the @ref
LinearVarianceNLL() method).

# Output

This library also provides classes that precedes the `usr::base::format` classes
to allow for latex printing of the measurement container classes in the form of:

\f[
  \hat{x}^{+\sigma_+}_{-\sigma_-} \;;\quad
  \hat{x}^{+\sigma_+}_{-\sigma_-}\times 10^{E \neq 0 } \;;\quad
\f]

In the event that the string representation of \f$\sigma_+\f$ and \f$\sigma_-\f$
are identical with the requested precision (**NOT** necessarily when the doubles
that of identical value!), the symmetric notation of

\f[
  \hat{x}\pm\sigma \;;\quad
  \left( \hat{x}\pm\sigma\right)\times 10^{E \neq 0}
\f]
would be used.

The precision used for the class construction represents the number of digits to
display for behind the decimal point for the central value. The precision used
for generating the strings representing the uncertainties would be of equal
value. For more details, see the implemented classes `usr::fmt::decimal` and
`usr::fmt::scientific`

@class usr::fmt::decimal
@details

Decimal representation of a measurement with uncertainties. If the precision is
set to negative, the precision is automatically [determined](@ref
usr::fmt::decimal::SetPrecision())

@class usr::fmt::scientific

Scientific notation of a measurement with uncertainty, with the central values
and uncertainties scaled by a common exponent such that the mantissa of the
central value is inline with the common definition of the scientific notations.
If precision is set to negative for a measurement, the precision would be
automatically determined such that the larger uncertainty displays at least two
significant digits.
