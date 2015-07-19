// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#include <sstream>
#include <iostream>

// WRL is blocked in C++/CLI projects, so using the next best thing
#include <atlcomcli.h>
#include <comdef.h>

#include <mfapi.h>
#include <mfidl.h>
#include <Mferror.h>

#include <d3d11_1.h>

//
// Error handling
//

// Exception-based error handling
#define CHK(statement)  {HRESULT _hr = (statement); if (FAILED(_hr)) { throw gcnew System::Runtime::InteropServices::COMException(nullptr, _hr); }}
#define CHKNULL(p)  {if ((p) == nullptr) { CHK(E_POINTER); }}
#define CHKOOM(p)  {if ((p) == nullptr) { CHK(E_OUTOFMEMORY); }}

