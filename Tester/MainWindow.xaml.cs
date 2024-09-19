using CppLibImaging;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace ForegroundDetectorTester
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        //ForegroundDetector3DH _foregroundDetector3DH;
        private string _filePath;
        private BitmapImage _bitmap;
        private int _nthColorNumber = 0;

        public MainWindow()
        {
            InitializeComponent();
            OpenFileButton.Click += OpenFileButton_Click;
            DetectForegroundButton.Click += DetectForegroundButton_Click;
            RestartButton.Click += RestartButton_Click;
        }

        private void RestartButton_Click(object sender, RoutedEventArgs e)
        {
            _nthColorNumber = 0;
        }

        private void DetectForegroundButton_Click(object sender, RoutedEventArgs e)
        {
            if (_filePath == null)
            {
                MessageBox.Show("Please Select a file and then detect its foreground");
                return;
            }

            DetectForegroundButton.Content = "Processing...";

            Task.Run(() => 
            {
                Thread.Sleep(200);
                Dispatcher.Invoke(RunDection);
            });
        }

        private void RunDection()
        {
            WriteableBitmap writableBitmap = new WriteableBitmap(_bitmap);

            int width = writableBitmap.PixelWidth;
            int height = writableBitmap.PixelHeight;

            int[] pixels = new int[width * height];
            writableBitmap.CopyPixels(pixels, width * 4, 0);

            ForegroundDetector3DH foregroundDetector3DH = new ForegroundDetector3DH();

            int dim = 12;

            foregroundDetector3DH.Create(dim, dim, dim);
            foregroundDetector3DH.SetColorSharpness(Convert.ToDouble(SharpnessTextBox.Text));
            foregroundDetector3DH.SetDistancePow(Convert.ToDouble(DistPowTextBox.Text));
            foregroundDetector3DH.SetPNorm(Convert.ToDouble(PNormTextBox.Text));
            foregroundDetector3DH.SetLightEffect(Convert.ToDouble(LightEffectTextBox.Text));

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    int pixelColor = pixels[y * width + x];
                    byte blue = (byte)(pixelColor & 0xFF);
                    byte green = (byte)((pixelColor >> 8) & 0xFF);
                    byte red = (byte)((pixelColor >> 16) & 0xFF);
                    byte alpha = (byte)((pixelColor >> 24) & 0xFF);

                    foregroundDetector3DH.AddColor(alpha, red, green, blue);
                }
            }

            var c = foregroundDetector3DH.DetectNth(_nthColorNumber++);

            ForegroundText.Foreground = DetectedForegroundRect.Fill = new SolidColorBrush(Color.FromRgb(c.R, c.G, c.B));


            var s = foregroundDetector3DH.DetectMultiple(dim * dim * dim);

            SpectrumGrid.Children.Clear();
            SpectrumGrid.ColumnDefinitions.Clear();
            for (int i = 0; i < s.Length; i++)
            {
                SpectrumGrid.ColumnDefinitions.Add(new ColumnDefinition() { Width = new GridLength(5, GridUnitType.Pixel) });
                Rectangle r = new Rectangle() { Fill = new SolidColorBrush(Color.FromRgb(s[i].R, s[i].G, s[i].B)) };
                Grid.SetColumn(r, i);
                SpectrumGrid.Children.Add(r);
            }

            DetectForegroundButton.Content = "Detect Next Foreground";
        }

        private void OpenFileButton_Click(object sender, RoutedEventArgs e)
        {
            System.Windows.Forms.OpenFileDialog openFileDialog = new System.Windows.Forms.OpenFileDialog
            {
                Filter = "Image Files|*.jpg;*.jpeg;*.png;*.bmp;*.gif|All Files|*.*",
                Title = "Select an Image"
            };

            if (openFileDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                // Set the selected image source to the Image control
                _filePath = openFileDialog.FileName;
                _bitmap = new BitmapImage(new Uri(_filePath));
                Image.Source = _bitmap;
                _nthColorNumber = 0;
            }
        }
    }
}
