#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <algorithm>
#include <sstream>
#include <queue>
#include <chrono>
#include <functional>

#include <strsafe.h>

#include <wrl.h>
#include <comdef.h>

#include <mfapi.h>
#include <mfidl.h>
#include <Mferror.h>
#include <windows.media.h>

#include "DebuggerLogger.h"

namespace AWFC = ::ABI::Windows::Foundation::Collections;
namespace AWM = ::ABI::Windows::Media;
namespace MW = ::Microsoft::WRL;
namespace MWW = ::Microsoft::WRL::Wrappers;

//
// Tracing
//

#define Trace(format, ...) { \
    if(s_logger.IsEnabled(LogLevel::Information)) { s_logger.Log(__FUNCTION__, LogLevel::Information, format, __VA_ARGS__); } \
}
#define TraceError(format, ...) { \
    if(s_logger.IsEnabled(LogLevel::Error)) { s_logger.Log(__FUNCTION__, LogLevel::Error, format, __VA_ARGS__); } \
}

//
// Error handling
//

// Exception-based error handling
#define CHK(statement)  {HRESULT _hr = (statement); if (FAILED(_hr)) { throw _com_error(_hr); }}
#define CHKNULL(p)  {if ((p) == nullptr) { CHK(E_POINTER); }}
#define CHKOOM(p)  {if ((p) == nullptr) { CHK(E_OUTOFMEMORY); }}

// Exception-free error handling
#define CHK_RETURN(statement) {hr = (statement); if (FAILED(hr)) { return hr; };}

//
// Exception boundary (converts exceptions into HRESULTs)
//

namespace Details
{
    template<size_t L /*= sizeof(__FUNCTION__)*/>
    class TracedExceptionBoundary
    {
    public:
        TracedExceptionBoundary(_In_ const char *function /*= __FUNCTION__*/)
            : _function(function)
        {
        }

        TracedExceptionBoundary(const TracedExceptionBoundary&) = delete;
        TracedExceptionBoundary& operator=(const TracedExceptionBoundary&) = delete;

        HRESULT operator()(std::function<void()>&& lambda)
        {
            s_logger.Log(_function, L, LogLevel::Verbose, "boundary enter");

            HRESULT hr = S_OK;
            try
            {
                lambda();
            }
#ifdef _INC_COMDEF // include comdef.h to enable
            catch (const _com_error& e)
            {
                hr = e.Error();
            }
#endif
#ifdef __cplusplus_winrt // enable C++/CX to use (/ZW)
            catch (Platform::Exception^ e)
            {
                hr = e->HResult;
            }
#endif
            catch (const std::bad_alloc&)
            {
                hr = E_OUTOFMEMORY;
            }
            catch (const std::out_of_range&)
            {
                hr = E_BOUNDS;
            }
            catch (const std::exception& e)
            {
                s_logger.Log(_function, L, LogLevel::Error, "caught unknown STL exception: %s", e.what());
                hr = E_FAIL;
            }
            catch (...)
            {
                s_logger.Log(_function, L, LogLevel::Error, "caught unknown exception");
                hr = E_FAIL;
            }

            if (FAILED(hr))
            {
                s_logger.Log(_function, L, LogLevel::Error, "boundary exit - failed hr=%08X", hr);
            }
            else
            {
                s_logger.Log(_function, L, LogLevel::Verbose, "boundary exit");
            }

            return hr;
        }

    private:
        const char* _function;
    };
}

#define ExceptionBoundary ::Details::TracedExceptionBoundary<sizeof(__FUNCTION__)>(__FUNCTION__)

//
// Exception-safe PROPVARIANT
//

class PropVariant : public PROPVARIANT
{
public:
    PropVariant()
    {
        PropVariantInit(this);
    }
    ~PropVariant()
    {
        (void)PropVariantClear(this);
    }

    PropVariant(const PropVariant&) = delete;
    PropVariant& operator&(const PropVariant&) = delete;
};
