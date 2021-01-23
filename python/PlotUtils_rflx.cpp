// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME dIhomedIenscdIHomeworkdICMSSWdIUserUtilsdIpythondIPlotUtils_rflx
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
#include "/home/ensc/Homework/CMSSW/UserUtils/python/PlotUtils.hpp"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *usrcLcLpltcLcLPad1D_Dictionary();
   static void usrcLcLpltcLcLPad1D_TClassManip(TClass*);
   static void delete_usrcLcLpltcLcLPad1D(void *p);
   static void deleteArray_usrcLcLpltcLcLPad1D(void *p);
   static void destruct_usrcLcLpltcLcLPad1D(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::usr::plt::Pad1D*)
   {
      ::usr::plt::Pad1D *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::usr::plt::Pad1D));
      static ::ROOT::TGenericClassInfo 
         instance("usr::plt::Pad1D", "UserUtils/PlotUtils/Pad1D.hpp", 50,
                  typeid(::usr::plt::Pad1D), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &usrcLcLpltcLcLPad1D_Dictionary, isa_proxy, 4,
                  sizeof(::usr::plt::Pad1D) );
      instance.SetDelete(&delete_usrcLcLpltcLcLPad1D);
      instance.SetDeleteArray(&deleteArray_usrcLcLpltcLcLPad1D);
      instance.SetDestructor(&destruct_usrcLcLpltcLcLPad1D);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::usr::plt::Pad1D*)
   {
      return GenerateInitInstanceLocal((::usr::plt::Pad1D*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::usr::plt::Pad1D*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *usrcLcLpltcLcLPad1D_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::usr::plt::Pad1D*)0x0)->GetClass();
      usrcLcLpltcLcLPad1D_TClassManip(theClass);
   return theClass;
   }

   static void usrcLcLpltcLcLPad1D_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *usrcLcLpltcLcLSimple1DCanvas_Dictionary();
   static void usrcLcLpltcLcLSimple1DCanvas_TClassManip(TClass*);
   static void *new_usrcLcLpltcLcLSimple1DCanvas(void *p = 0);
   static void *newArray_usrcLcLpltcLcLSimple1DCanvas(Long_t size, void *p);
   static void delete_usrcLcLpltcLcLSimple1DCanvas(void *p);
   static void deleteArray_usrcLcLpltcLcLSimple1DCanvas(void *p);
   static void destruct_usrcLcLpltcLcLSimple1DCanvas(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::usr::plt::Simple1DCanvas*)
   {
      ::usr::plt::Simple1DCanvas *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::usr::plt::Simple1DCanvas));
      static ::ROOT::TGenericClassInfo 
         instance("usr::plt::Simple1DCanvas", "UserUtils/PlotUtils/Simple1DCanvas.hpp", 21,
                  typeid(::usr::plt::Simple1DCanvas), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &usrcLcLpltcLcLSimple1DCanvas_Dictionary, isa_proxy, 4,
                  sizeof(::usr::plt::Simple1DCanvas) );
      instance.SetNew(&new_usrcLcLpltcLcLSimple1DCanvas);
      instance.SetNewArray(&newArray_usrcLcLpltcLcLSimple1DCanvas);
      instance.SetDelete(&delete_usrcLcLpltcLcLSimple1DCanvas);
      instance.SetDeleteArray(&deleteArray_usrcLcLpltcLcLSimple1DCanvas);
      instance.SetDestructor(&destruct_usrcLcLpltcLcLSimple1DCanvas);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::usr::plt::Simple1DCanvas*)
   {
      return GenerateInitInstanceLocal((::usr::plt::Simple1DCanvas*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::usr::plt::Simple1DCanvas*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *usrcLcLpltcLcLSimple1DCanvas_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::usr::plt::Simple1DCanvas*)0x0)->GetClass();
      usrcLcLpltcLcLSimple1DCanvas_TClassManip(theClass);
   return theClass;
   }

   static void usrcLcLpltcLcLSimple1DCanvas_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *usrcLcLpltcLcLRatio1DCanvas_Dictionary();
   static void usrcLcLpltcLcLRatio1DCanvas_TClassManip(TClass*);
   static void *new_usrcLcLpltcLcLRatio1DCanvas(void *p = 0);
   static void *newArray_usrcLcLpltcLcLRatio1DCanvas(Long_t size, void *p);
   static void delete_usrcLcLpltcLcLRatio1DCanvas(void *p);
   static void deleteArray_usrcLcLpltcLcLRatio1DCanvas(void *p);
   static void destruct_usrcLcLpltcLcLRatio1DCanvas(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::usr::plt::Ratio1DCanvas*)
   {
      ::usr::plt::Ratio1DCanvas *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::usr::plt::Ratio1DCanvas));
      static ::ROOT::TGenericClassInfo 
         instance("usr::plt::Ratio1DCanvas", "UserUtils/PlotUtils/Ratio1DCanvas.hpp", 84,
                  typeid(::usr::plt::Ratio1DCanvas), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &usrcLcLpltcLcLRatio1DCanvas_Dictionary, isa_proxy, 4,
                  sizeof(::usr::plt::Ratio1DCanvas) );
      instance.SetNew(&new_usrcLcLpltcLcLRatio1DCanvas);
      instance.SetNewArray(&newArray_usrcLcLpltcLcLRatio1DCanvas);
      instance.SetDelete(&delete_usrcLcLpltcLcLRatio1DCanvas);
      instance.SetDeleteArray(&deleteArray_usrcLcLpltcLcLRatio1DCanvas);
      instance.SetDestructor(&destruct_usrcLcLpltcLcLRatio1DCanvas);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::usr::plt::Ratio1DCanvas*)
   {
      return GenerateInitInstanceLocal((::usr::plt::Ratio1DCanvas*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::usr::plt::Ratio1DCanvas*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *usrcLcLpltcLcLRatio1DCanvas_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::usr::plt::Ratio1DCanvas*)0x0)->GetClass();
      usrcLcLpltcLcLRatio1DCanvas_TClassManip(theClass);
   return theClass;
   }

   static void usrcLcLpltcLcLRatio1DCanvas_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *usrcLcLpltcLcLPad2DFlat_Dictionary();
   static void usrcLcLpltcLcLPad2DFlat_TClassManip(TClass*);
   static void delete_usrcLcLpltcLcLPad2DFlat(void *p);
   static void deleteArray_usrcLcLpltcLcLPad2DFlat(void *p);
   static void destruct_usrcLcLpltcLcLPad2DFlat(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::usr::plt::Pad2DFlat*)
   {
      ::usr::plt::Pad2DFlat *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::usr::plt::Pad2DFlat));
      static ::ROOT::TGenericClassInfo 
         instance("usr::plt::Pad2DFlat", "UserUtils/PlotUtils/Pad2DFlat.hpp", 36,
                  typeid(::usr::plt::Pad2DFlat), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &usrcLcLpltcLcLPad2DFlat_Dictionary, isa_proxy, 4,
                  sizeof(::usr::plt::Pad2DFlat) );
      instance.SetDelete(&delete_usrcLcLpltcLcLPad2DFlat);
      instance.SetDeleteArray(&deleteArray_usrcLcLpltcLcLPad2DFlat);
      instance.SetDestructor(&destruct_usrcLcLpltcLcLPad2DFlat);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::usr::plt::Pad2DFlat*)
   {
      return GenerateInitInstanceLocal((::usr::plt::Pad2DFlat*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::usr::plt::Pad2DFlat*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *usrcLcLpltcLcLPad2DFlat_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::usr::plt::Pad2DFlat*)0x0)->GetClass();
      usrcLcLpltcLcLPad2DFlat_TClassManip(theClass);
   return theClass;
   }

   static void usrcLcLpltcLcLPad2DFlat_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *usrcLcLpltcLcLFlat2DCanvas_Dictionary();
   static void usrcLcLpltcLcLFlat2DCanvas_TClassManip(TClass*);
   static void *new_usrcLcLpltcLcLFlat2DCanvas(void *p = 0);
   static void *newArray_usrcLcLpltcLcLFlat2DCanvas(Long_t size, void *p);
   static void delete_usrcLcLpltcLcLFlat2DCanvas(void *p);
   static void deleteArray_usrcLcLpltcLcLFlat2DCanvas(void *p);
   static void destruct_usrcLcLpltcLcLFlat2DCanvas(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::usr::plt::Flat2DCanvas*)
   {
      ::usr::plt::Flat2DCanvas *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::usr::plt::Flat2DCanvas));
      static ::ROOT::TGenericClassInfo 
         instance("usr::plt::Flat2DCanvas", "UserUtils/PlotUtils/Flat2DCanvas.hpp", 19,
                  typeid(::usr::plt::Flat2DCanvas), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &usrcLcLpltcLcLFlat2DCanvas_Dictionary, isa_proxy, 4,
                  sizeof(::usr::plt::Flat2DCanvas) );
      instance.SetNew(&new_usrcLcLpltcLcLFlat2DCanvas);
      instance.SetNewArray(&newArray_usrcLcLpltcLcLFlat2DCanvas);
      instance.SetDelete(&delete_usrcLcLpltcLcLFlat2DCanvas);
      instance.SetDeleteArray(&deleteArray_usrcLcLpltcLcLFlat2DCanvas);
      instance.SetDestructor(&destruct_usrcLcLpltcLcLFlat2DCanvas);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::usr::plt::Flat2DCanvas*)
   {
      return GenerateInitInstanceLocal((::usr::plt::Flat2DCanvas*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::usr::plt::Flat2DCanvas*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *usrcLcLpltcLcLFlat2DCanvas_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::usr::plt::Flat2DCanvas*)0x0)->GetClass();
      usrcLcLpltcLcLFlat2DCanvas_TClassManip(theClass);
   return theClass;
   }

   static void usrcLcLpltcLcLFlat2DCanvas_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *usrcLcLpltcLcLCommonXCanvas_Dictionary();
   static void usrcLcLpltcLcLCommonXCanvas_TClassManip(TClass*);
   static void delete_usrcLcLpltcLcLCommonXCanvas(void *p);
   static void deleteArray_usrcLcLpltcLcLCommonXCanvas(void *p);
   static void destruct_usrcLcLpltcLcLCommonXCanvas(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::usr::plt::CommonXCanvas*)
   {
      ::usr::plt::CommonXCanvas *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::usr::plt::CommonXCanvas));
      static ::ROOT::TGenericClassInfo 
         instance("usr::plt::CommonXCanvas", "UserUtils/PlotUtils/CommonXCanvas.hpp", 23,
                  typeid(::usr::plt::CommonXCanvas), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &usrcLcLpltcLcLCommonXCanvas_Dictionary, isa_proxy, 4,
                  sizeof(::usr::plt::CommonXCanvas) );
      instance.SetDelete(&delete_usrcLcLpltcLcLCommonXCanvas);
      instance.SetDeleteArray(&deleteArray_usrcLcLpltcLcLCommonXCanvas);
      instance.SetDestructor(&destruct_usrcLcLpltcLcLCommonXCanvas);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::usr::plt::CommonXCanvas*)
   {
      return GenerateInitInstanceLocal((::usr::plt::CommonXCanvas*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::usr::plt::CommonXCanvas*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *usrcLcLpltcLcLCommonXCanvas_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::usr::plt::CommonXCanvas*)0x0)->GetClass();
      usrcLcLpltcLcLCommonXCanvas_TClassManip(theClass);
   return theClass;
   }

   static void usrcLcLpltcLcLCommonXCanvas_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrapper around operator delete
   static void delete_usrcLcLpltcLcLPad1D(void *p) {
      delete ((::usr::plt::Pad1D*)p);
   }
   static void deleteArray_usrcLcLpltcLcLPad1D(void *p) {
      delete [] ((::usr::plt::Pad1D*)p);
   }
   static void destruct_usrcLcLpltcLcLPad1D(void *p) {
      typedef ::usr::plt::Pad1D current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::usr::plt::Pad1D

namespace ROOT {
   // Wrappers around operator new
   static void *new_usrcLcLpltcLcLSimple1DCanvas(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) ::usr::plt::Simple1DCanvas : new ::usr::plt::Simple1DCanvas;
   }
   static void *newArray_usrcLcLpltcLcLSimple1DCanvas(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) ::usr::plt::Simple1DCanvas[nElements] : new ::usr::plt::Simple1DCanvas[nElements];
   }
   // Wrapper around operator delete
   static void delete_usrcLcLpltcLcLSimple1DCanvas(void *p) {
      delete ((::usr::plt::Simple1DCanvas*)p);
   }
   static void deleteArray_usrcLcLpltcLcLSimple1DCanvas(void *p) {
      delete [] ((::usr::plt::Simple1DCanvas*)p);
   }
   static void destruct_usrcLcLpltcLcLSimple1DCanvas(void *p) {
      typedef ::usr::plt::Simple1DCanvas current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::usr::plt::Simple1DCanvas

namespace ROOT {
   // Wrappers around operator new
   static void *new_usrcLcLpltcLcLRatio1DCanvas(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) ::usr::plt::Ratio1DCanvas : new ::usr::plt::Ratio1DCanvas;
   }
   static void *newArray_usrcLcLpltcLcLRatio1DCanvas(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) ::usr::plt::Ratio1DCanvas[nElements] : new ::usr::plt::Ratio1DCanvas[nElements];
   }
   // Wrapper around operator delete
   static void delete_usrcLcLpltcLcLRatio1DCanvas(void *p) {
      delete ((::usr::plt::Ratio1DCanvas*)p);
   }
   static void deleteArray_usrcLcLpltcLcLRatio1DCanvas(void *p) {
      delete [] ((::usr::plt::Ratio1DCanvas*)p);
   }
   static void destruct_usrcLcLpltcLcLRatio1DCanvas(void *p) {
      typedef ::usr::plt::Ratio1DCanvas current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::usr::plt::Ratio1DCanvas

namespace ROOT {
   // Wrapper around operator delete
   static void delete_usrcLcLpltcLcLPad2DFlat(void *p) {
      delete ((::usr::plt::Pad2DFlat*)p);
   }
   static void deleteArray_usrcLcLpltcLcLPad2DFlat(void *p) {
      delete [] ((::usr::plt::Pad2DFlat*)p);
   }
   static void destruct_usrcLcLpltcLcLPad2DFlat(void *p) {
      typedef ::usr::plt::Pad2DFlat current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::usr::plt::Pad2DFlat

namespace ROOT {
   // Wrappers around operator new
   static void *new_usrcLcLpltcLcLFlat2DCanvas(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) ::usr::plt::Flat2DCanvas : new ::usr::plt::Flat2DCanvas;
   }
   static void *newArray_usrcLcLpltcLcLFlat2DCanvas(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) ::usr::plt::Flat2DCanvas[nElements] : new ::usr::plt::Flat2DCanvas[nElements];
   }
   // Wrapper around operator delete
   static void delete_usrcLcLpltcLcLFlat2DCanvas(void *p) {
      delete ((::usr::plt::Flat2DCanvas*)p);
   }
   static void deleteArray_usrcLcLpltcLcLFlat2DCanvas(void *p) {
      delete [] ((::usr::plt::Flat2DCanvas*)p);
   }
   static void destruct_usrcLcLpltcLcLFlat2DCanvas(void *p) {
      typedef ::usr::plt::Flat2DCanvas current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::usr::plt::Flat2DCanvas

namespace ROOT {
   // Wrapper around operator delete
   static void delete_usrcLcLpltcLcLCommonXCanvas(void *p) {
      delete ((::usr::plt::CommonXCanvas*)p);
   }
   static void deleteArray_usrcLcLpltcLcLCommonXCanvas(void *p) {
      delete [] ((::usr::plt::CommonXCanvas*)p);
   }
   static void destruct_usrcLcLpltcLcLCommonXCanvas(void *p) {
      typedef ::usr::plt::CommonXCanvas current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::usr::plt::CommonXCanvas

namespace {
  void TriggerDictionaryInitialization_PlotUtils_rflx_Impl() {
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
#line 1 "PlotUtils_rflx dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
namespace usr{namespace plt{class __attribute__((annotate("$clingAutoload$UserUtils/PlotUtils/Pad1D.hpp")))  __attribute__((annotate("$clingAutoload$UserUtils/PlotUtils/Simple1DCanvas.hpp")))  Pad1D;}}
namespace usr{namespace plt{class __attribute__((annotate("$clingAutoload$UserUtils/PlotUtils/Simple1DCanvas.hpp")))  Simple1DCanvas;}}
namespace usr{namespace plt{class __attribute__((annotate("$clingAutoload$UserUtils/PlotUtils/Ratio1DCanvas.hpp")))  Ratio1DCanvas;}}
namespace usr{namespace plt{class __attribute__((annotate("$clingAutoload$UserUtils/PlotUtils/Pad2DFlat.hpp")))  __attribute__((annotate("$clingAutoload$UserUtils/PlotUtils/Flat2DCanvas.hpp")))  Pad2DFlat;}}
namespace usr{namespace plt{class __attribute__((annotate("$clingAutoload$UserUtils/PlotUtils/Flat2DCanvas.hpp")))  Flat2DCanvas;}}
namespace usr{namespace plt{class __attribute__((annotate("$clingAutoload$UserUtils/PlotUtils/CommonXCanvas.hpp")))  CommonXCanvas;}}
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "PlotUtils_rflx dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#ifndef USERUTILS_PYUTILS_PLOTUTILS_HPP
#define USERUTILS_PYUTILS_PLOTUTILS_HPP

// Including all UserLevel headers in the PlotUtils subpackage

#include "UserUtils/PlotUtils/Simple1DCanvas.hpp"
#include "UserUtils/PlotUtils/Ratio1DCanvas.hpp"
#include "UserUtils/PlotUtils/Flat2DCanvas.hpp"
#include "UserUtils/PlotUtils/CommonXCanvas.hpp"

#include "UserUtils/PlotUtils/Pad1D.hpp"
#include "UserUtils/PlotUtils/Constants.hpp"
#include "UserUtils/PlotUtils/PlotCommon.hpp"

// Users will probably never use this...
// #include "UserUtils/PlotUtils/Canvas.hpp"

#endif
#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"usr::plt::CommonXCanvas", payloadCode, "@",
"usr::plt::EntryText", payloadCode, "@",
"usr::plt::ExtrapolateInRatio", payloadCode, "@",
"usr::plt::FillColor", payloadCode, "@",
"usr::plt::FillStyle", payloadCode, "@",
"usr::plt::Flat2DCanvas", payloadCode, "@",
"usr::plt::LineColor", payloadCode, "@",
"usr::plt::LineStyle", payloadCode, "@",
"usr::plt::LineWidth", payloadCode, "@",
"usr::plt::MarkerColor", payloadCode, "@",
"usr::plt::MarkerSize", payloadCode, "@",
"usr::plt::MarkerStyle", payloadCode, "@",
"usr::plt::Pad1D", payloadCode, "@",
"usr::plt::Pad2DFlat", payloadCode, "@",
"usr::plt::Plot2DF", payloadCode, "@",
"usr::plt::PlotType", payloadCode, "@",
"usr::plt::PlotUnder", payloadCode, "@",
"usr::plt::Ratio1DCanvas", payloadCode, "@",
"usr::plt::ScaleY", payloadCode, "@",
"usr::plt::Simple1DCanvas", payloadCode, "@",
"usr::plt::TextAlign", payloadCode, "@",
"usr::plt::TextAngle", payloadCode, "@",
"usr::plt::TextColor", payloadCode, "@",
"usr::plt::TextSize", payloadCode, "@",
"usr::plt::VisualizeError", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("PlotUtils_rflx",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_PlotUtils_rflx_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_PlotUtils_rflx_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_PlotUtils_rflx() {
  TriggerDictionaryInitialization_PlotUtils_rflx_Impl();
}
