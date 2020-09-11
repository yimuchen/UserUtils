#!/bin/env  python

##
#  @file
#  @brief   dumping the number of events in a list of input files.
#  @ingroup EDMUtils
#
import ROOT
import argparse
import sys
from DataFormats.FWLite import Events, Lumis, Runs

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument( 'inputfiles', metavar='N', type=str, nargs='+',
                    help='List of input files to use')
    args = parser.parse_args()

    numofevts = 0
    numoflumis = 0
    numofruns  = 0

    # Using wc style output format
    print "{:>8s} {:>8s} {:>8s}   {}".format("Events","Lumis", "Runs", "Files")
    fmtstr = "{:>8d} {:>8d} {:>8d}   {}"

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

    print fmtstr.format( numofevts,
        numoflumis,
        numofruns,
        "total({} files)".format(len(args.inputfiles))
    )

if __name__ == "__main__":
    main()
