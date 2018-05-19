# `Maths` package

This package is a contains classes to help with mathematical operations. Especially to help with calculations of measurements with uncertainty

# `Measurement` class
Defined in [`Measurement.hpp`](./interface/Measurement.hpp), it is a class which is a representation of a measured parameter with (potentially asymmetric) error. Basic arithmetic, as well as access to central value, absolute and relative errors is defined in this class. The treatment of asymmetric error here is wrong! The correct functions for treating asymmetric error is currently under construction.

## `Measurement` formatting
Defining function that help represent the `Measurement` class in string formats. Function are defined in the in [`MeasurementFormat.hpp`](./interface//MeasurementFormat.hpp) class. Note that most is designed for raw latex style strings.
