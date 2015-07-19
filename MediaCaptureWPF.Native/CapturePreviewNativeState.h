#pragma once

class CapturePreviewNativeState
{
public:
    CapturePreviewNativeState(unsigned int width, unsigned int height, _In_ const CComPtr<IMFMediaSink>& sink);

    CComPtr<IMFMediaSink> m_sink;
    CComPtr<IDirect3D9Ex> m_d3d9;
    CComPtr<IDirect3DDevice9Ex> m_d3d9Device;
    CComPtr<IDirect3DSurface9> m_d3d9SharedSurface;
    CComPtr<IDirect3DTexture9> m_d3d9SharedTexture;
    CComPtr<IMFSample> m_sample;
    HANDLE m_d3d9SharedTextureHandle;
};

