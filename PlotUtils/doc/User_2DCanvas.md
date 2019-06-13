@class usr::plt::Flat2DCanvas
@ingroup PlotUtilsUser

The Flat2DCanvas class is a canvas containing exactly Pad2DFlat instance. Its
member functions are generated such that it looks like a users plotting calls
are directly performed on the canvas, greatly simplifying the plotting process.

The pad assumes that the plotted objects would be fill up the main axis frame,
hence the choice of the CMS labels and luminosity labels being placed both
outside the main axis frame. The legend and other text, should therefore be
placed on the left of the main canvas.

The following are a few examples of plotting using the Flat2DCanvas (with
the style/color settings and data filling code removed)
