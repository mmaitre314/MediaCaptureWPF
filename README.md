[![Build status](https://ci.appveyor.com/api/projects/status/e8oe4ihb3f4it8k5?svg=true)](https://ci.appveyor.com/project/mmaitre314/mediacapturewpf)

```XML
<Grid>
    <Image Name="Preview"/>
</Grid>
```

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
- Blogs: 
  - [Direct3D 10/11 & Direct2D in WPF](http://jmorrill.hjtcentral.com/Home/tabid/428/EntryId/437/Direct3D-10-11-Direct2D-in-WPF.aspx)
  - [C++/CLI Cheat Sheet](http://manski.net/2011/04/cpp-cli-cheat-sheet/)
  - [Media WinRT APIs in Desktop apps](http://mmaitre314.github.io/2015/02/01/using-the-winrt-media-apis-to-encode-audio-in-desktop-cs-apps.html)
