using MediaCaptureWPF;
using System;
using System.Windows;
using Windows.Media.Capture;

namespace CaptureTestApp.NuGet
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

            var preview = new CapturePreview(capture);
            Preview.Source = preview;
            await preview.StartAsync();
        }
    }
}
