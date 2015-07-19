#pragma once

using namespace System;

class CapturePreviewNativeState;

namespace MediaCaptureWPF { namespace Native
{
    public ref class CapturePreviewNative
    {
    public:
        CapturePreviewNative(_In_ System::Windows::Interop::D3DImage^ image, unsigned int width, unsigned int height);

        // IDisposable::Dispose()
        ~CapturePreviewNative();

        // Finalizer (GC's non-deterministic destruction)
        !CapturePreviewNative();

        property Object^ MediaSink {
            Object^ get() { return m_sink; }
        }

    internal:
        void OnSample(_In_ IMFSample* sample);

    private:
        void RefreshPreview();

        delegate void RefreshPreviewDelegate();

        const unsigned int m_width;
        const unsigned int m_height;
        bool m_imageInitialized;
        System::Windows::Interop::D3DImage^ m_image;
        Object^ m_sink;
        CapturePreviewNativeState* m_state;
    };
} }
