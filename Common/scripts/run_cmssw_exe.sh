#!/bin/bash
cd $CMSSW_BASE
source /cvmfs/cms.cern.ch/cmsset_default.sh
eval `scramv1 runtime -sh`

cd $PWD
$@
