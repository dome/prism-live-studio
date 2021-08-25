// Copyright (c) 2019 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the CEF translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//
// $hash=7eda7522edc12bf229307a92fbf463194fea8a3f$
//

#ifndef CEF_LIBCEF_DLL_CPPTOC_REQUEST_CONTEXT_HANDLER_CPPTOC_H_
#define CEF_LIBCEF_DLL_CPPTOC_REQUEST_CONTEXT_HANDLER_CPPTOC_H_
#pragma once

#if !defined(WRAPPING_CEF_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/cef_request_context_handler_capi.h"
#include "include/cef_request_context_handler.h"
#include "libcef_dll/cpptoc/cpptoc_ref_counted.h"

// Wrap a C++ class with a C structure.
// This class may be instantiated and accessed wrapper-side only.
class CefRequestContextHandlerCppToC
    : public CefCppToCRefCounted<CefRequestContextHandlerCppToC,
                                 CefRequestContextHandler,
                                 cef_request_context_handler_t> {
 public:
  CefRequestContextHandlerCppToC();
  virtual ~CefRequestContextHandlerCppToC();
};

#endif  // CEF_LIBCEF_DLL_CPPTOC_REQUEST_CONTEXT_HANDLER_CPPTOC_H_
