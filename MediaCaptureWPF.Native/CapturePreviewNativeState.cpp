#include "stdafx.h"
#include "CapturePreviewNativeState.h"

CapturePreviewNativeState::CapturePreviewNativeState(
    unsigned int width, 
    unsigned int height, 
    _In_ const CComPtr<IMFMediaSink>& sink
    )
    : m_d3d9SharedTextureHandle(nullptr)
    , m_sink(sink)
{
    Trace("@ initializing DirectX9");

    CHK(Direct3DCreate9Ex(D3D_SDK_VERSION, &m_d3d9));

    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed = true;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = GetDesktopWindow();
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    CHK(m_d3d9->CreateDeviceEx(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        d3dpp.hDeviceWindow,
        D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
        &d3dpp,
        nullptr,
        &m_d3d9Device
        ));

    CHK(m_d3d9Device->CreateTexture(
        width, 
        height, 
        1, 
        D3DUSAGE_RENDERTARGET, 
        D3DFMT_X8R8G8B8, 
        D3DPOOL_DEFAULT, 
        &m_d3d9SharedTexture, 
        &m_d3d9SharedTextureHandle
        ));

    CHK(m_d3d9SharedTexture->GetSurfaceLevel(0, &m_d3d9SharedSurface));

    Trace("@ DirectX9 initialized");
}
