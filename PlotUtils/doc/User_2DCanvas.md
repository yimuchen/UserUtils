@class usr::plt::Flat2DCanvas
@ingroup PlotUtilsUser

The Flat2DCanvas class is a canvas containing exactly Pad2DFlat instance. Its
member functions are generated such that it looks like a users plotting calls
are directly performed on the canvas, greatly simplifying the plotting process.

This class is designed as a preset for simple data plotting, with the margin,
dimension and font defaults tuned to produce decent, publication ready plots
with minimal amount of user code.

The following are a few examples of plotting using the Flat2DCanvas (with
the style/color settings and data filling code removed)
