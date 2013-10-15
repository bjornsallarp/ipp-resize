
/* ////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2005-2012 Intel Corporation. All Rights Reserved.
//
//
//
*/

#ifndef __DIAGNDESCR_H__
#define __DIAGNDESCR_H__

//
//      Declaration of the most common non-specific part of interfaces
//  to provide binding between diagnostic events generation
//  and receiving (translation to context).
//
//  In this interface model the component is responsible only for providing
//  of unique name for constants and data structures (and this is common programming task,
//  nothing additional to care). So it's possible to do not care about duplicating error
//  numerical codes in different components. The interface suggested here allows binding local
//  error codes and structures with any [unique or non-unique] global error contexts. The examples of such a context
//  are some global error codes or some text messages or UUID or any structure of data.
//
//  The main advantage of this interface model,
//  is independence of component's local numerical codes and data structures.
//  So it's possible to easy introduce new components.
//
//  The abstract diagnostic model, which is used here, consists of
//  diagnostic event generation and receiving.
//  The generation stage creates special descriptor of event,
//  that convert specific event data structure
//  to the free-customizable receiver context.
//  (For example, missing file name and file operation flag
//  to some specific language text).
//  During the receiving stage the application-specific context
//  is passed to the specific diagnostic output (stream).
//
//  This part of interfaces encapsulates generation of diagnostic
//  events from their subsequent processing, so you may implement
//  generation of diagnostic events in one object module
//  and complex (for example multi-language) translation context
//  in another object module and here is only that you need
//  for their binding.
//
//  Specific details of these interfaces should be completely declared
//  and implemented in code parts, which are responsible for such
//  particular specific.
//
//  This header includes only the most common interfaces to link
//  application-customized diagnosis with diagnostic event generators:
//
//  )) Diagnostic event contexts declarations (compile-time, run-time,
//     and virtual base common interface to provide possibility of single common receiver).
//     They encapsulate receiver-side treatment of diagnostic data
//     (like specific-language support in application and so on).
//
//  )) Diagnostic event descriptors declarations (compile-time, run-time,
//     and virtual base common interface to provide possibility of single common receiver).
//     They encapsulate translation from generator-side treatment of diagnostic data
//     (like structure of data, that cause the event)
//     to receiver-side treatment (like text message).
//
//
//



////////////////////////////////////////////////////////////////////////
//
//  Here is the most general interfaces to provide connection
//  with common receiver part (like text output stream).
//  (And it can not be used to generate events).
//

// Customized general interface that is supported by any context.
// As an example you may include in it the virtual function returning text message,
// throw it and use common "catch(const BDiagnDescr &exc) {...}" to process event.
// Here is just declaration, implementation details should be provided by application.
class DiagnCtxt;

// Here is general interface for run-time receiver of diagnostic event.
class BDiagnDescr {
public:
    virtual DiagnCtxt Context() const = 0;
    virtual const char* GetMessage() const = 0;
    virtual ~BDiagnDescr() {}
protected:
    BDiagnDescr() {}
};


////////////////////////////////////////////////////////////////////////
//
//  Below are the detailed interfaces
//  to generate any specific diagnostic events
//  as well as to provide connection with specific receiver part
//  (just as an example, file access exception filter).
//


// DiagnDescrBrief provides interface for creation of specific diagnostic events,
// which can be binded with diagnostic context unambiguously in compile-time,
// which do not require any information to pass in run-time.
// You need to redefine it for each translation of diagnostic event
// to the constant context, like constant text string, or some global error code.
// To do it simply redefine (specialize) Context function
// for some locally enumerated error code (CODE from ENUM).
// Custom context will be binded with it during compile-time.
// (ATTENTION! to be on the safe side on memory allocation,
// remember, you can not specify additional parameters in specialization!
// You must not specialize members externally, except only "Context()"!)
template<typename ENUM, ENUM CODE>
class DiagnDescrCT : public BDiagnDescr {
public:
    DiagnDescrCT() {}

    DiagnCtxt Context() const; // (it is not static, ONLY because it should be virtual)
    const char* GetMessage() const;
};


// DiagnDescrRT provides interface for generation of specific diagnostic events,
// which can be binded with diagnostic context completely only in run-time,
// and they are require some actual information to pass in run-time
// (in addition to compile-time binded CODE from ENUM).
// To do it simply redefine (specialize) Context function
// for some locally enumerated error code, and specify parameter structure(class)
// that should be passed from generator to receiver in run-time.
// (ATTENTION! to be on the safe side on memory allocation,
// remember, you can not specify additional parameters in specialization!
// You must not specialize members externally, except only "Context()"!)
template<typename ENUM, ENUM CODE, typename PARAM>
class DiagnDescrRT : public BDiagnDescr  {
public:
    DiagnDescrRT(const PARAM &param) : m_param(param) {}

    DiagnCtxt Context() const;

protected:
    PARAM m_param;
};

// If all local error codes and data for run-time binding
// are encapsulated in the single class, and local error codes
// enumerator has name "Cause", it may be used as shortage.
#define DIAGN_DESCR_RT(excClass, code) \
DiagnDescrRT<excClass::Cause, excClass::code, excClass>

#endif // __DIAGNDESCR_H__
