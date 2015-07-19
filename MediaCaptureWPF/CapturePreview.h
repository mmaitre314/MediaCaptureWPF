#pragma once

using namespace System;

class CapturePreviewNativeState;

namespace MediaCaptureWPF
{
    public ref class CapturePreview
    {
    public:
        CapturePreview(unsigned int width, unsigned int height);

        // IDisposable::Dispose()
        ~CapturePreview();

        // Finalizer (GC's non-deterministic destruction)
        !CapturePreview();

        property System::Windows::Interop::D3DImage^ ImageSource {
            System::Windows::Interop::D3DImage^ get() { return m_image; }
        }

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
}
