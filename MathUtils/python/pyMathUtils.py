import ROOT
import cppyy
import os

# Turning off Error for dictionary not found.
ROOT.gErrorIgnoreLevel = ROOT.kError
if 'CMSSW_BASE' in os.environ:
  cppyy.load_reflection_info("pluginpyMathUtils.so")
else:
  cppyy.load_reflection_info('lib/libpyMathUtils.so')
ROOT.gErrorIgnoreLevel = ROOT.kWarning

# Renaming all the namespace
from cppyy.gbl import Measurement
from cppyy.gbl import NormalCDF
from cppyy.gbl import GetConfidenceLevel
