[![Build status](https://ci.appveyor.com/api/projects/status/e8oe4ihb3f4it8k5?svg=true)](https://ci.appveyor.com/project/mmaitre314/mediacapturewpf)
[![NuGet package](http://mmaitre314.github.io/images/nuget.png)](https://www.nuget.org/packages/MMaitre.MediaCaptureWPF/)
[![Symbols](http://mmaitre314.github.io/images/Symbols.png)](http://mmaitre314.github.io/2015/05/24/personal-pdb-symbol-server.html)


`CameraPreview` allows previewing the camera video stream of the WinRT [MediaCapture](https://msdn.microsoft.com/en-us/library/windows/apps/windows.media.capture.mediacapture.aspx) API in Desktop WPF apps.

![Preview](http://mmaitre314.github.io/images/WPFCameraPreview.jpg)

`CameraPreview` derives from [D3DImage](https://msdn.microsoft.com/en-us/library/system.windows.interop.d3dimage(v=vs.110).aspx) and can be used anywhere a GPU-accelerated [ImageSource](https://msdn.microsoft.com/en-us/library/system.windows.media.imagesource(v=vs.110).aspx) can be. For instance, inside an `<Image>` XAML element:

```XML
<Grid>
    <Image Name="Preview"/>
</Grid>
```

The C# code behind creates `MediaCapture`, initializes it, and passes it to `CameraPreview`. This object is then set as source of the `<Image>` element and preview is started.

```C#
var capture = new MediaCapture();
await capture.InitializeAsync();

var preview = new CapturePreview(capture);
Preview.Source = preview;
await preview.StartAsync();
```

# References

- CodeProject: [Introduction to D3DImage](http://www.codeproject.com/Articles/28526/Introduction-to-D-DImage)
- MSDN: [D3DImage](https://msdn.microsoft.com/en-us/library/system.windows.interop.d3dimage(v=vs.110).aspx)
- GitHub: [Camera Starter Kit](https://github.com/Microsoft/Windows-universal-samples/tree/master/CameraStarterKit)
- Blogs: 
  - [Direct3D 10/11 & Direct2D in WPF](http://jmorrill.hjtcentral.com/Home/tabid/428/EntryId/437/Direct3D-10-11-Direct2D-in-WPF.aspx)
  - [C++/CLI Cheat Sheet](http://manski.net/2011/04/cpp-cli-cheat-sheet/)
  - [Media WinRT APIs in Desktop apps](http://mmaitre314.github.io/2015/02/01/using-the-winrt-media-apis-to-encode-audio-in-desktop-cs-apps.html)
