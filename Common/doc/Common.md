@defgroup Common Common
@brief A collection of functions for simpler experience using STL and boost functions.

@details
The [STL](http://en.cppreference.com/w/) and [boost](https://www.boost.org/)
template libraries are widely-avaiable, incredibly versitile library that could be used as a base for many programming tasks. The two libraries being designed
to be general puporse however does make using the libraries extermely verbose
when writing the code, and could be much simplified if one already knows that
your code would be using some specific use cases.

This sub-packages aims to do just that, specializing the STL and boost libraries
to cases most commonly used during my HEP analysis trip, as well as providing
additional classes that are of more general purpose with only STL interactions.