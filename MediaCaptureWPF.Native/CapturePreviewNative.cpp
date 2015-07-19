#include "stdafx.h"
#include "SampleCallback.h"
#include "CapturePreviewNativeState.h"
#include "CapturePreviewNative.h"

using namespace MediaCaptureWPF::Native;
using namespace msclr;
using namespace System::Runtime::InteropServices;
using namespace System::Windows;
using namespace System::Windows::Interop;
using namespace System::Windows::Threading;

CapturePreviewNative::CapturePreviewNative(_In_ D3DImage^ image, unsigned int width, unsigned int height)
    : m_width(width)
    , m_height(height)
    , m_imageInitialized(false)
    , m_image(image)
    , m_state(nullptr)
{
    Trace("CapturePreviewNative::CapturePreview@ creating the video sink");

    if (image == nullptr)
    {
        throw gcnew ArgumentNullException(L"image");
    }

    CComPtr<IMFMediaSink> sink;
    CreateVideoSink(
        width, 
        height,
        SampleCallback<CapturePreviewNative>(this),
        &sink
        );

    Trace("CapturePreviewNative::CapturePreview@ wrapping the native sink in managed object");

    m_sink = Marshal::GetObjectForIUnknown(IntPtr(sink.p));

    m_state = new CapturePreviewNativeState(width, height, sink);
}

// IDisposable::Dispose()
CapturePreviewNative::~CapturePreviewNative()
{
    lock lock(this);
    Trace("CapturePreviewNative::~CapturePreview@ disposing CapturePreview");

    if (m_image != nullptr)
    {
        m_image->Lock();
        m_image->SetBackBuffer(D3DResourceType::IDirect3DSurface9, IntPtr::Zero);
        m_image->Unlock();
        m_image = nullptr;
        m_imageInitialized = false;
    }

    this->!CapturePreviewNative();
}

CapturePreviewNative::!CapturePreviewNative()
{
    Trace("CapturePreviewNative::!CapturePreview@ finalizing CapturePreview");

    if (m_state != nullptr)
    {
        (void)m_state->m_sink->Shutdown();
        delete m_state;
        m_state = nullptr;
    }
}

void CapturePreviewNative::OnSample(_In_ IMFSample* sample)
{
    Dispatcher^ dispatcher;
    {
        lock lock(this);

        if ((m_image == nullptr) || (m_state == nullptr))
        {
            return;
        }
        if (m_state->m_sample != nullptr)
        {
            return; // Drop frame: UI thread not keeping up
        }
        m_state->m_sample = sample;
        dispatcher = m_image->Dispatcher;
    }

    // Go back to the UI thread
    dispatcher->BeginInvoke(gcnew RefreshPreviewDelegate(this, &CapturePreviewNative::RefreshPreview));
}

void CapturePreviewNative::RefreshPreview()
{
    lock lock(this);

    if ((m_image == nullptr) || (m_state == nullptr))
    {
        return;
    }
    if (!m_image->IsFrontBufferAvailable)
    {
        if (m_imageInitialized)
        {
            Trace("CapturePreviewNative::RefreshPreview@ setting back buffer on D3DImage");

            m_image->Lock();
            m_image->SetBackBuffer(D3DResourceType::IDirect3DSurface9, IntPtr::Zero);
            m_image->Unlock();
            m_imageInitialized = false;
        }
        return;
    }

    if (!m_imageInitialized)
    {
        m_image->Lock();
        m_image->SetBackBuffer(D3DResourceType::IDirect3DSurface9, IntPtr(m_state->m_d3d9SharedSurface.p));
        m_image->Unlock();
        m_imageInitialized = true;
    }

    CComPtr<IMFDXGIBuffer> bufferDxgi;
    CComPtr<IMFMediaBuffer> buffer;
    CHK(m_state->m_sample->GetBufferByIndex(0, &buffer));
    CHK(buffer.QueryInterface(&bufferDxgi));

    CComPtr<IDXGIResource> inputResource;
    unsigned int inputSubresource;
    CHK(bufferDxgi->GetResource(IID_PPV_ARGS(&inputResource)));
    CHK(bufferDxgi->GetSubresourceIndex(&inputSubresource));

    CComPtr<ID3D11Texture2D> inputD3dTexture;
    CComPtr<ID3D11Resource> inputD3dResource;
    CHK(inputResource.QueryInterface(&inputD3dTexture));
    CHK(inputResource.QueryInterface(&inputD3dResource));

    CComPtr<ID3D11Device> inputD3dDevice;
    inputD3dTexture->GetDevice(&inputD3dDevice);

    CComPtr<ID3D11Resource> outputD3dResource;
    CHK(inputD3dDevice->OpenSharedResource(m_state->m_d3d9SharedTextureHandle, IID_PPV_ARGS(&outputD3dResource)));

    Trace("CapturePreviewNative::RefreshPreview@ copying DX texture to D3DImage");

    CComPtr<ID3D11DeviceContext> inputD3dContext;
    inputD3dDevice->GetImmediateContext(&inputD3dContext);
    m_image->Lock();
    inputD3dContext->CopySubresourceRegion(outputD3dResource, 0, 0, 0, 0, inputD3dResource, inputSubresource, nullptr);
    m_image->AddDirtyRect(Int32Rect(0, 0, m_width, m_height));
    m_image->Unlock();

    m_state->m_sample = nullptr;

    // TODO: add tests
    // TODO: add NuGet
    // TODO: add AppVeyor
    // TODO: add GitHub doc
    // TODO: StackOverflow, twitter
}
