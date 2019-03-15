#!/bin/bash

# Getting the images
../testbin/PlotUtils/usrutil_simple1dcanvas
../testbin/PlotUtils/usrutil_ratio1dcanvas
# rsync $testdir/simple1dcanvas_roofit_test.png  image/
# rsync $testdir/ratio1dcanvas_test.png          image/
# rsync $testdir/ratio1dcanvas_roofit_test.png   image/

# Generating CSS file
sass style.scss:style.css --style compact

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