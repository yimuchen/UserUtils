#!/bin/bash 

#Simple bash script for running condor where all input/output are directly
# accessible for all worker nodes, like at the umd cluster or small scale readout
# at FNAL using the /data_scratch directories. 

# The first argument is the directory for the CMSSW software environment that you
# want to run the environment in. All other arugments used for the command that
# you want to run

CMSSW_DIR=$1 
CMD="${@:2}"

# Moving into the directory, and setting up the CMSSW environemnt 
echo "Moving to CMSSW directory: "${CMSSW_DIR}
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd $CMSSW_DIR
eval $(scramv1 runtime -sh)

# Running the command 
$CMD

