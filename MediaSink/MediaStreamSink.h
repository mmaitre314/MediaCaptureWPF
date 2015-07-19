#pragma once

class MediaStreamSink WrlSealed :
    public Microsoft::WRL::RuntimeClass <
    MW::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
    MW::CloakedIid<IMFStreamSink>,
    MW::CloakedIid<IMFMediaEventGenerator>,
    MW::CloakedIid < IMFMediaTypeHandler >
    >
{
public:

    MediaStreamSink(
        __in const MW::ComPtr<IMFMediaSink>& sink,
        __in DWORD id,
        __in const MW::ComPtr<IMFMediaType>& mt,
        __in const std::function<void(IMFSample*)>& sampleHandler
        );

    //
    // IMFStreamSink
    //

    IFACEMETHOD(GetMediaSink)(__deref_out IMFMediaSink **sink) override;
    IFACEMETHOD(GetIdentifier)(__out DWORD *identifier) override;
    IFACEMETHOD(GetMediaTypeHandler)(__deref_out IMFMediaTypeHandler **handler) override;
    IFACEMETHOD(ProcessSample)(__in_opt IMFSample *sample) override;
    IFACEMETHOD(PlaceMarker)(__in MFSTREAMSINK_MARKER_TYPE markerType, __in const PROPVARIANT * markerValue, __in const PROPVARIANT * contextValue) override;
    IFACEMETHOD(Flush)() override;

    //
    // IMFMediaEventGenerator
    //

    IFACEMETHOD(GetEvent)(__in DWORD flags, __deref_out IMFMediaEvent **event) override;
    IFACEMETHOD(BeginGetEvent)(__in IMFAsyncCallback *callback, __in_opt IUnknown *state) override;
    IFACEMETHOD(EndGetEvent)(__in IMFAsyncResult *result, __deref_out IMFMediaEvent **event) override;
    IFACEMETHOD(QueueEvent)(__in MediaEventType met, __in REFGUID extendedType, __in HRESULT status, __in_opt const PROPVARIANT *value) override;

    //
    // IMFMediaTypeHandler
    //

    IFACEMETHOD(IsMediaTypeSupported)(__in IMFMediaType *mediaType, __deref_out_opt  IMFMediaType **closestMediaType) override;
    IFACEMETHOD(GetMediaTypeCount)(__out DWORD *typeCount) override;
    IFACEMETHOD(GetMediaTypeByIndex)(__in DWORD index, __deref_out  IMFMediaType **mediaType) override;
    IFACEMETHOD(SetCurrentMediaType)(__in IMFMediaType *mediaType) override;
    IFACEMETHOD(GetCurrentMediaType)(__deref_out_opt IMFMediaType **mediaType) override;
    IFACEMETHOD(GetMajorType)(__out GUID *majorType) override;

    //
    // Misc
    //

    void InternalSetCurrentMediaType(__in const MW::ComPtr<IMFMediaType>& mediaType);
    void RequestSample();
    void Shutdown();

private:

    bool _IsMediaTypeSupported(__in const MW::ComPtr<IMFMediaType>& mt) const;
    void _UpdateMediaType(__in const MW::ComPtr<IMFMediaType>& mt);

    void _VerifyNotShutdown()
    {
        if (_shutdown)
        {
            CHK(MF_E_SHUTDOWN);
        }
    }

    MW::ComPtr<IMFMediaSink> _sink;
    MW::ComPtr<IMFMediaEventQueue> _eventQueue;
    MW::ComPtr<IMFMediaType> _curMT;
    std::function<void(IMFSample*)> _sampleHandler;

    GUID _majorType;
    GUID _subType;
    unsigned int _width;
    unsigned int _height;
    DWORD _id;
    bool _shutdown;

    MWW::SRWLock _lock;
};
