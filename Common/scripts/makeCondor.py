#!/bin/env python3

#/bin/env python3
import argparse
import sys
import os

parser = argparse.ArgumentParser("Generating condor jobs from a list of input")
parser.add_argument('--executable', '-e', type=str, help='Executable to run')
parser.add_argument(
    '--jobname',
    '-j',
    type=str,
    help=
    'Unique name for job in case the name is not unique, existing files might overwritten.'
)
parser.add_argument(
    '--jobcount',
    '-n',
    type=int,
    default=1,
    help=
    'Number of jobs to submit, In case the certain input/outputs need to vary by the job process, you can use the arguments options with $(Process), just be careful of bash string expansion.'
)
parser.add_argument('--logdir',
                    '-l',
                    type=str,
                    default=os.getcwd(),
                    help='Where to store the logging information')
parser.add_argument('--jdldir',
                    type=str,
                    default=os.getcwd(),
                    help='Where to store the output JDL files.')
parser.add_argument('arguments',
                    type=str,
                    nargs='+',
                    help='Additional arguments to pass to the script')
args = parser.parse_args()

CONDOR_JDL_TEMPLATE = """
universe              = vanilla
Executable            = {CMS_BASE}/src/UserUtils/Common/scripts/run_cmssw_exe.sh
should_transfer_files = NO
Requirements          = TARGET.FileSystemDomain=="privnet" && (OpSysMajorVer==6)
request_memory        = 1 GB
Output                = {LOGDIR}/{JOBNAME}_$(Process).stdout
Error                 = {LOGDIR}/{JOBNAME}_$(Process).stderr
Log                   = {LOGDIR}/{JOBNAME}_$(Process).condor
Arguments             = {ARGS}
Queue {JOBCOUNT}
"""

condor_args = "{exe} {args}".format(exe=args.executable,
                                    args=" ".join(args.arguments))

jdldir = args.jdldir
jobname = args.jobname

with open(jdldir + '/' + jobname + '.jdl', 'w') as f:
  f.write(
      CONDOR_JDL_TEMPLATE.format(CMS_BASE=os.environ['CMSSW_BASE'],
                                 LOGDIR=args.logdir,
                                 JOBNAME=jobname,
                                 ARGS=condor_args,
                                 JOBCOUNT=args.jobcount))
