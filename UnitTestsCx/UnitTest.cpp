#include "stdafx.h"

using namespace std;
using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;
using namespace System::Windows;
using namespace System::Windows::Interop;
using namespace System::Windows::Threading;
using namespace MediaCaptureWPF::Native;
using namespace Microsoft::VisualStudio::TestTools::UnitTesting;

namespace UnitTestsCx
{
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
        void TestMediaSink()
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
    };
}
