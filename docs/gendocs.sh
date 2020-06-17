#!/bin/bash

# Getting the images
if [[ "$1" = "replot" ]] ; then
  ../testbin/PlotUtils/usrutil_simple1dcanvas
  ../testbin/PlotUtils/usrutil_ratio1dcanvas
  ../testbin/PlotUtils/usrutil_flat2dcanvas

  ../testbin/PlotUtils/usrutil_make_fake_standardformat_data
  ../bin/usr_PlotStandard standard ../PlotUtils/test/standardplot_format.json
  mv Mass.pdf image/std_plot.pdf
  convert  -density 144 -trim image/std_plot.pdf -quality 100 -flatten -sharpen 0x1.0 image/std_plot.png
  convert  -density 500 -trim image/std_plot.pdf -quality 100 -flatten -sharpen 0x1.0 image/std_plot_highres.png
fi

# Generating CSS file
sass style.scss:style.css --style compressed

# Running doxygen!
doxygen doxygen.cfg

## removing redundant components in documentation
rm html/tabs.css
rm html/bc_s.png
rm html/bdwn.png
rm html/doc.png
rm html/folderclosed.png
rm html/folderopen.png
rm html/nav_f.png
rm html/nav_g.png
rm html/nav_h.png
rm html/sync_off.png
rm html/sync_on.png
rm html/tab_a.png
rm html/tab_b.png
rm html/tab_h.png
rm html/tab_s.png

# Copying custom files
mkdir -p html/customjs
cp customjs/* html/customjs
mkdir -p html/image
cp image/* html/image
