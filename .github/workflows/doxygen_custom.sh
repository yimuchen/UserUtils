#!/bin/bash 
# This script assumes that we are working from the root directory of the repository

# Adding custom styling to the doxygen stuff 
mkdir -p docs/html/customjs
cp docs/customjs/* docs/html/customjs

# Adding images to the doxygen stuf
mkdir -p docs/html/image
cp docs/image/* docs/html/image
