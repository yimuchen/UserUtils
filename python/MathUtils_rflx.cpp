// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME dIhomedIenscdIHomeworkdICMSSWdIUserUtilsdIpythondIMathUtils_rflx
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// The generated code does not explicitly qualifies STL entities
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "/home/ensc/Homework/CMSSW/UserUtils/python/MathUtils.hpp"

// Header files passed via #pragma extra_include

namespace {
  void TriggerDictionaryInitialization_MathUtils_rflx_Impl() {
    static const char* headers[] = {
"0",
0
    };
    static const char* includePaths[] = {
"/home/ensc/Homework/CMSSW/UserUtils/include",
"/usr/include",
"/home/ensc/Homework/CMSSW/UserUtils/python/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "MathUtils_rflx dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "MathUtils_rflx dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#ifndef USERUTILS_PYUTILS_PLOTUTILS_HPP
#define USERUTILS_PYUTILS_PLOTUTILS_HPP

// Including all UserLevel headers in the PlotUtils subpackage

#include "UserUtils/MathUtils/Measurement.hpp"
#include "UserUtils/MathUtils/Miscellaneous.hpp"
#include "UserUtils/MathUtils/RootMathTools.hpp"
#include "UserUtils/MathUtils/StatisticsUtil.hpp"

// Users will probably never use this...
// #include "UserUtils/PlotUtils/Canvas.hpp"

#endif

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("MathUtils_rflx",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_MathUtils_rflx_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_MathUtils_rflx_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_MathUtils_rflx() {
  TriggerDictionaryInitialization_MathUtils_rflx_Impl();
}
