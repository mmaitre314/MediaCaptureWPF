// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#include <functional>

// WRL is blocked in C++/CLI projects, so using the next best thing
#include <atlcomcli.h>
#include <comdef.h>

#include <msclr\gcroot.h>
#include <msclr\lock.h>

#include <mfapi.h>
#include <mfidl.h>
#include <Mferror.h>

#include <d3d9.h>
#include <d3d11_1.h>

#include "..\MediaSink\VideoSinkFactory.h"
#include "..\MediaSink\DebuggerLogger.h"

//
// Tracing
//

// C++/CLI does not support varargs, so tracing functions are more limited
#define Trace(message) { \
    s_logger.Log(LogLevel::Information, message "\n"); \
}
#define TraceError(message) { \
    s_logger.Log(LogLevel::Error, message "\n"); \
}

//
// Error handling
//

// Exception-based error handling
#define CHK(statement)  {HRESULT _hr = (statement); if (FAILED(_hr)) { throw gcnew System::Runtime::InteropServices::COMException(nullptr, _hr); }}
#define CHKNULL(p)  {if ((p) == nullptr) { CHK(E_POINTER); }}
#define CHKOOM(p)  {if ((p) == nullptr) { CHK(E_OUTOFMEMORY); }}
