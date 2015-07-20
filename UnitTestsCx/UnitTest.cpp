#include "stdafx.h"

using namespace std;
using namespace System;
using namespace System::Text;
using namespace System::Threading;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;
using namespace System::Windows;
using namespace System::Windows::Interop;
using namespace System::Windows::Threading;
using namespace MediaCaptureWPF::Native;
using namespace Microsoft::VisualStudio::TestTools::UnitTesting;

#undef Yield

namespace UnitTestsCx
{

    ref class MockD3DImage : public D3DImage
    {
    public:
        MockD3DImage()
            : m_callCountAddDirtyRect(0)
        {
        }

        void Lock()
        {
            m_callCountAddDirtyRect++;
            D3DImage::Lock();
        }

        void AddDirtyRect(Int32Rect dirtyRect)
        {
            m_callCountAddDirtyRect++;
            D3DImage::AddDirtyRect(dirtyRect);
        }

        unsigned int m_callCountAddDirtyRect;

    };

    [TestClass]
    public ref class UnitTest
    {
    private:
        TestContext^ testContextInstance;

    public: 
        property TestContext^ TestContext
        {
            Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ get()
            {
                return testContextInstance;
            }
            void set(Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ value)
            {
                testContextInstance = value;
            }
        };

        [TestMethod]
        void TestCreate()
        {
            auto image = gcnew D3DImage();
            auto preview = gcnew CapturePreviewNative(image, 320, 240);
        };

        [TestMethod]
        void TestDispose()
        {
            auto image = gcnew D3DImage();
            auto preview = gcnew CapturePreviewNative(image, 320, 240);
            delete preview;
        };

        [TestMethod]
        void TestMediaSinkInterfaces()
        {
            Console::WriteLine(L"Creating D3DImage and CapturePreviewNative");
            auto image = gcnew D3DImage();
            auto preview = gcnew CapturePreviewNative(image, 320, 240);

            Console::WriteLine(L"Getting MediaSink IUnknown");
            IntPtr sinkPtr = Marshal::GetIUnknownForObject(preview->MediaSink);

            Console::WriteLine(L"Querying IMFMediaSink and IMFClockStateSink");
            CComPtr<IUnknown> sinkUnk((IUnknown*)(void*)sinkPtr);
            CComPtr<IMFMediaSink> sink;
            CComPtr<IMFClockStateSink> sinkClock;
            CHK(sinkUnk.QueryInterface(&sink));
            CHK(sinkUnk.QueryInterface(&sinkClock));

            Marshal::Release(sinkPtr);
            delete preview;
        };

        [TestMethod]
        void TestMediaSinkStreaming()
        {
            const unsigned int width = 320;
            const unsigned int height = 240;
            Console::WriteLine(L"Creating mock D3DImage and CapturePreviewNative");
            auto image = gcnew D3DImage();

            // Check that the image is ready to receive a texture and empty
            Assert::IsTrue(image->IsFrontBufferAvailable);
            Assert::AreEqual(0., image->Width);
            Assert::AreEqual(0., image->Height);

            auto preview = gcnew CapturePreviewNative(image, width, height);

            Console::WriteLine(L"Getting MediaSink IUnknown");
            IntPtr sinkPtr = Marshal::GetIUnknownForObject(preview->MediaSink);

            Console::WriteLine(L"Querying IMFMediaSink and IMFClockStateSink");
            CComPtr<IUnknown> sinkUnk((IUnknown*)(void*)sinkPtr);
            CComPtr<IMFMediaSink> sink;
            CComPtr<IMFClockStateSink> sinkClock;
            CHK(sinkUnk.QueryInterface(&sink));
            CHK(sinkUnk.QueryInterface(&sinkClock));

            Console::WriteLine(L"Getting sink stream");
            CComPtr<IMFStreamSink> stream;
            CHK(sink->GetStreamSinkByIndex(0, &stream));

            Console::WriteLine(L"Starting streaming and waiting for sample request");
            CComPtr<IMFMediaEvent> evt;
            MediaEventType evtType;
            CHK(sinkClock->OnClockStart(0, 0));
            CHK(stream->GetEvent(0, &evt));
            CHK(evt->GetType(&evtType));
            Assert::AreEqual((int)MEStreamSinkRequestSample, (int)evtType);

            Console::WriteLine(L"Process sample");
            auto device = CreateD3D11Device();
            CHK(stream->ProcessSample(CreateSample(device, width, height)));

            Console::WriteLine(L"Waiting from texture to be copied to the image");
            image->Dispatcher->Invoke(DispatcherPriority::Background, gcnew Action(this, &UnitTest::Noop));
            Assert::AreEqual((double)width, image->Width);
            Assert::AreEqual((double)height, image->Height);

            Console::WriteLine(L"Waiting for second sample request");
            evt = nullptr;
            CHK(stream->GetEvent(0, &evt));
            CHK(evt->GetType(&evtType));
            Assert::AreEqual((int)MEStreamSinkRequestSample, (int)evtType);

            Console::WriteLine(L"Shutting down sink");
            CHK(sink->Shutdown());

            Marshal::Release(sinkPtr);
            delete preview;
        };

        CComPtr<ID3D11Device> CreateD3D11Device()
        {
            D3D_FEATURE_LEVEL level;
            static const D3D_FEATURE_LEVEL levels[] =
            {
                D3D_FEATURE_LEVEL_11_1,
                D3D_FEATURE_LEVEL_11_0,
                D3D_FEATURE_LEVEL_10_1,
                D3D_FEATURE_LEVEL_10_0,
                D3D_FEATURE_LEVEL_9_3,
                D3D_FEATURE_LEVEL_9_2,
                D3D_FEATURE_LEVEL_9_1
            };

            CComPtr<ID3D11Device> device;
            CHK(D3D11CreateDevice(
                nullptr,
                D3D_DRIVER_TYPE_HARDWARE,
                nullptr,
                D3D11_CREATE_DEVICE_VIDEO_SUPPORT | D3D11_CREATE_DEVICE_BGRA_SUPPORT, // DEVICE_VIDEO needed for MF, BGRA for SurfaceImageSource
                levels,
                ARRAYSIZE(levels),
                D3D11_SDK_VERSION,
                &device,
                &level,
                nullptr
                ));

            CComPtr<ID3D10Multithread> multithread;
            CHK(device.QueryInterface(&multithread));
            multithread->SetMultithreadProtected(true);

            return device;
        }

        CComPtr<IMFSample> CreateSample(const CComPtr<ID3D11Device> &device, unsigned int width, unsigned int height)
        {
            D3D11_TEXTURE2D_DESC desc = {};
            desc.Width = width;
            desc.Height = height;
            desc.Format = DXGI_FORMAT_B8G8R8X8_UNORM;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.CPUAccessFlags = 0;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            desc.MiscFlags = 0;
            desc.ArraySize = 1;
            desc.MipLevels = 1;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;

            CComPtr<ID3D11Texture2D> texture;
            CHK(device->CreateTexture2D(&desc, nullptr, &texture));

            CComPtr<IMFMediaBuffer> buffer;
            CHK(MFCreateDXGISurfaceBuffer(__uuidof(ID3D11Texture2D), texture, 0, /*fBottomUpWhenLinear*/false, &buffer));

            CComPtr<IMFSample> sample;
            CHK(MFCreateSample(&sample));
            CHK(sample->AddBuffer(buffer));
            CHK(sample->SetSampleDuration(0));
            CHK(sample->SetSampleTime(0));

            return sample;
        }

        void Noop()
        {
        }
    };
}
