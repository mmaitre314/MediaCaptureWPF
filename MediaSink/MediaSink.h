#pragma once

const unsigned int c_audioStreamSinkId = 0;
const unsigned int c_videoStreamSinkId = 1;

class MediaSink WrlSealed
    : public MW::RuntimeClass <
    MW::RuntimeClassFlags<
    MW::RuntimeClassType::WinRtClassicComMix>,
    AWM::IMediaExtension,
    MW::CloakedIid<IMFMediaSink>,
    MW::CloakedIid<IMFClockStateSink>,
    MW::FtmBase
    >
{
    InspectableClass(L"MediaSink", BaseTrust)

public:

    MediaSink(
        _In_opt_ const MW::ComPtr<IMFMediaType>& audioMT,
        _In_opt_ const MW::ComPtr<IMFMediaType>& videoMT,
        _In_opt_ const std::function<void(IMFSample*)>& audioSampleHandler,
        _In_opt_ const std::function<void(IMFSample*)>& videoSampleHandler
        )
        : _shutdown(false)
    {
        if (audioMT != nullptr)
        {
            _audioStreamSink = MW::Make<MediaStreamSink>(
                this,
                c_audioStreamSinkId,
                audioMT,
                audioSampleHandler
                );
        }

        if (videoMT != nullptr)
        {
            _videoStreamSink = MW::Make<MediaStreamSink>(
                this,
                c_videoStreamSinkId,
                videoMT,
                videoSampleHandler
                );
        }
    }

    void SetCurrentAudioMediaType(_In_ IMFMediaType* mt)
    {
        auto lock = _lock.LockExclusive();

        _VerifyNotShutdown();

        _audioStreamSink->InternalSetCurrentMediaType(mt);
    }

    void SetCurrentVideoMediaType(_In_ IMFMediaType* mt)
    {
        auto lock = _lock.LockExclusive();

        _VerifyNotShutdown();

        _videoStreamSink->InternalSetCurrentMediaType(mt);
    }

    //
    // IMediaExtension
    //

    IFACEMETHOD(SetProperties)(_In_ AWFC::IPropertySet * /*configuration*/) override
    {
        return ExceptionBoundary([this]()
        {
            auto lock = _lock.LockExclusive();

            _VerifyNotShutdown();
        });
    }

    //
    // IMFMediaSink
    //

    IFACEMETHOD(GetCharacteristics)(_Out_ DWORD *characteristics)  override
    {
        return ExceptionBoundary([this, characteristics]()
        {
            _VerifyNotShutdown();

            CHKNULL(characteristics);
            *characteristics = MEDIASINK_RATELESS | MEDIASINK_FIXED_STREAMS;
        });
    }

    IFACEMETHOD(AddStreamSink)(
        DWORD /*streamSinkIdentifier*/,
        _In_ IMFMediaType * /*mediaType*/,
        _COM_Outptr_ IMFStreamSink **streamSink
        ) override
    {
        return ExceptionBoundary([this, streamSink]()
        {
            _VerifyNotShutdown();

            CHKNULL(streamSink);
            *streamSink = nullptr;

            CHK(MF_E_STREAMSINKS_FIXED);
        });
    }

    IFACEMETHOD(RemoveStreamSink)(DWORD /*streamSinkIdentifier*/) override
    {
        return ExceptionBoundary([this]()
        {
            _VerifyNotShutdown();

            CHK(MF_E_STREAMSINKS_FIXED);
        });
    }

    IFACEMETHOD(GetStreamSinkCount)(_Out_ DWORD *streamSinkCount) override
    {
        return ExceptionBoundary([this, streamSinkCount]()
        {
            CHKNULL(streamSinkCount);

            _VerifyNotShutdown();

            *streamSinkCount = (_audioStreamSink != nullptr) + (_videoStreamSink != nullptr);
        });
    }

    IFACEMETHOD(GetStreamSinkByIndex)(DWORD index, _COM_Outptr_ IMFStreamSink **streamSink) override
    {
        return ExceptionBoundary([this, index, streamSink]()
        {
            auto lock = _lock.LockExclusive();

            CHKNULL(streamSink);
            *streamSink = nullptr;

            _VerifyNotShutdown();

            switch (index)
            {
            case 0:
                if (_audioStreamSink != nullptr)
                {
                    CHK(_audioStreamSink.CopyTo(streamSink));
                }
                else
                {
                    CHK(_videoStreamSink.CopyTo(streamSink));
                }
                break;

            case 1:
                if ((_audioStreamSink != nullptr) && (_videoStreamSink != nullptr))
                {
                    CHK(_videoStreamSink.CopyTo(streamSink));
                }
                else
                {
                    CHK(E_INVALIDARG);
                }
                break;

            default:
                CHK(E_INVALIDARG);
            }
        });
    }

    IFACEMETHOD(GetStreamSinkById)(DWORD identifier, _COM_Outptr_ IMFStreamSink **streamSink) override
    {
        return ExceptionBoundary([this, identifier, streamSink]()
        {
            auto lock = _lock.LockExclusive();

            CHKNULL(streamSink);
            *streamSink = nullptr;

            _VerifyNotShutdown();

            if ((identifier == 0) && (_audioStreamSink != nullptr))
            {
                CHK(_audioStreamSink.CopyTo(streamSink));
            }
            else if ((identifier == 1) && (_videoStreamSink != nullptr))
            {
                CHK(_videoStreamSink.CopyTo(streamSink));
            }
            else
            {
                CHK(E_INVALIDARG);
            }
        });
    }

    IFACEMETHOD(SetPresentationClock)(_In_ IMFPresentationClock *clock) override
    {
        return ExceptionBoundary([this, clock]()
        {
            auto lock = _lock.LockExclusive();

            _VerifyNotShutdown();

            if (_clock != nullptr)
            {
                CHK(_clock->RemoveClockStateSink(this));
                _clock = nullptr;
            }

            if (clock != nullptr)
            {
                CHK(clock->AddClockStateSink(this));
                _clock = clock;
            }
        });
    }

    IFACEMETHOD(GetPresentationClock)(_COM_Outptr_ IMFPresentationClock **clock) override
    {
        return ExceptionBoundary([this, clock]()
        {
            auto lock = _lock.LockExclusive();

            CHKNULL(clock);
            *clock = nullptr;

            _VerifyNotShutdown();

            if (_clock != nullptr)
            {
                CHK(_clock.CopyTo(clock))
            }
        });
    }

    IFACEMETHOD(Shutdown)() override
    {
        return ExceptionBoundary([this]()
        {
            auto lock = _lock.LockExclusive();

            if (_shutdown)
            {
                return;
            }
            _shutdown = true;

            if (_audioStreamSink != nullptr)
            {
                _audioStreamSink->Shutdown();
                _audioStreamSink = nullptr;
            }

            if (_videoStreamSink != nullptr)
            {
                _videoStreamSink->Shutdown();
                _videoStreamSink = nullptr;
            }

            if (_clock != nullptr)
            {
                (void)_clock->RemoveClockStateSink(this);
                _clock = nullptr;
            }
        });
    }

    //
    // IMFClockStateSink methods
    //

    IFACEMETHOD(OnClockStart)(MFTIME /*hnsSystemTime*/, LONGLONG /*llClockStartOffset*/) override
    {
        return ExceptionBoundary([this]()
        {
            auto lock = _lock.LockExclusive();

            _VerifyNotShutdown();

            if (_audioStreamSink != nullptr)
            {
                _audioStreamSink->RequestSample();
            }

            if (_videoStreamSink != nullptr)
            {
                _videoStreamSink->RequestSample();
            }
        });
    }

    IFACEMETHOD(OnClockStop)(MFTIME /*hnsSystemTime*/) override
    {
        return ExceptionBoundary([this]()
        {
            auto lock = _lock.LockExclusive();

            _VerifyNotShutdown();
        });
    }

    IFACEMETHOD(OnClockPause)(MFTIME /*hnsSystemTime*/) override
    {
        return ExceptionBoundary([this]()
        {
            auto lock = _lock.LockExclusive();

            _VerifyNotShutdown();
        });
    }

    IFACEMETHOD(OnClockRestart)(MFTIME /*hnsSystemTime*/) override
    {
        return ExceptionBoundary([this]()
        {
            auto lock = _lock.LockExclusive();

            _VerifyNotShutdown();
        });
    }

    IFACEMETHOD(OnClockSetRate)(MFTIME /*hnsSystemTime*/, float /*flRate*/) override
    {
        return ExceptionBoundary([this]()
        {
            auto lock = _lock.LockExclusive();

            _VerifyNotShutdown();
        });
    }

private:

    bool _shutdown;

    void _VerifyNotShutdown()
    {
        if (_shutdown)
        {
            CHK(MF_E_SHUTDOWN);
        }
    }

    MW::ComPtr<MediaStreamSink> _audioStreamSink;
    MW::ComPtr<MediaStreamSink> _videoStreamSink;
    MW::ComPtr<IMFPresentationClock> _clock;

    MWW::SRWLock _lock;
};