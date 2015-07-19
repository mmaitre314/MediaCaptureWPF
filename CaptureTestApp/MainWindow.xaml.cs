using MediaCaptureWPF;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Windows.Media;
using Windows.Media.Capture;
using Windows.Media.MediaProperties;

namespace CaptureTestApp
{
    public partial class MainWindow : Window
    {
        bool m_initialized;

        public MainWindow()
        {
            InitializeComponent();
        }

        protected override async void OnActivated(EventArgs e)
        {
            if (m_initialized)
            {
                return; // Already initialized
            }
            m_initialized = true;

            var capture = new MediaCapture();
            await capture.InitializeAsync(new MediaCaptureInitializationSettings
                {
                    StreamingCaptureMode = StreamingCaptureMode.Video // No audio
                });

            // TODO: move to C# assembly, rename MediaCaptureWPF -> MediaCaptureWPFNative
            var props = (VideoEncodingProperties)capture.VideoDeviceController.GetMediaStreamProperties(MediaStreamType.VideoPreview);
            var profile = new MediaEncodingProfile
            {
                Audio = null,
                Video = VideoEncodingProperties.CreateUncompressed(MediaEncodingSubtypes.Rgb32, props.Width, props.Height),
                Container = null
            };

            var preview = new CapturePreview(props.Width, props.Height);
            Preview.Source = preview.ImageSource;

            await capture.StartPreviewToCustomSinkAsync(profile, (IMediaExtension)preview.MediaSink);
        }
    }
}
