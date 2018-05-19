#!/bin/env  python
#*******************************************************************************
 #
 #  Filename    : dumpLumiCount.py
 #  Description : Dumping number of Events, Lumi and Runs in a list of
 #                input files
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************
import ROOT
import argparse
import sys
from DataFormats.FWLite import Events, Lumis, Runs

def main( argv ):
    parser = argparse.ArgumentParser()
    parser.add_argument( 'inputfiles', metavar='N', type=str, nargs='+',
                    help='List of input files to use')
    args = parser.parse_args()

    numofevts = 0
    numoflumis = 0
    numofruns  = 0

    # Using wc style output format
    print "{:8s} {:6s} {:4s} {}".format("Events","Lumis", "Runs", "File Name")
    fmtstr = "{:8d} {:6d} {:4d} {}"

    for inputfile in args.inputfiles:
        ev = Events( inputfile )
        lm = Lumis( inputfile )
        rn = Runs( inputfile )
        numofevts = numofevts + ev.size()
        numoflumis = numoflumis + lm._lumi.size()
        numofruns  = numofruns + rn._run.size()
        print fmtstr.format(
            ev.size(),
            lm._lumi.size(),
            rn._run.size(),
            inputfile
             )

    print fmtstr.format( numofevts,  numoflumis, numofruns, "total({} files)".format(len(args.inputfiles)) )


if __name__ == "__main__":
    main( 'hello' )
