using System;
using System.Net.Http;
using System.Threading.Tasks;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Markup;
using Windows.UI.Xaml.Media;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace RGBLightControllerUW
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public string DeviceURL = "http://10.0.0.175/";
        private int FunctionMode = 0;
        private Color SelectedColour;


        public MainPage()
        {
            this.InitializeComponent();
            
        }

        private void ButtonSetOff_Click(object sender, RoutedEventArgs e)
        {
            SendToDevice("poweroff");
        }

        

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            Button _myButton = (Button)sender;
            var color = (Color)XamlBindingHelper.ConvertValue(typeof(Color), _myButton.CommandParameter.ToString());
            

            SelectedColour = color;
            FunctionMode = 0;
            SendToDevice("setcolour?colour=" + color.R.ToString("X2") + color.G.ToString("X2") + color.B.ToString("X2") + "&level=" + ((int)SliderBrightness.Value).ToString());
        }

        private void Effect_Click(object sender, RoutedEventArgs e)
        {
            FunctionMode = 1;
            Button _myButton = (Button)sender;
            var c = _myButton.CommandParameter.ToString();
            switch (c)
            {
                case "Rainbow":
                    SendToDevice("rainbow");
                    break;
                case "Glow":
                    SendToDevice("glow");
                    break;

                case "twinkle":
                    SendToDevice("twinkle");
                    break;
                case "breath":
                    SendToDevice("breath");
                    break;
                default:
                    break;
            }
        }

        public void SendToDevice(String url)
        {
            _ = SendURL(url);
        }

            public async Task<String> SendURL(String url)
        {

            using var client = new HttpClient();
            var request = await client.GetAsync(DeviceURL + url);

            var download = await request.Content.ReadAsStringAsync();
            return download.ToString();
        }


        private void LEDColorPicker_ColorChanged(ColorPicker sender, ColorChangedEventArgs args)
        {
            if (LEDColorPicker.Color != null)
            {
                Color c = LEDColorPicker.Color;
                if (c != null && TextCustomColour != null)
                {
                    SelectedColour = c;
                    FunctionMode = 0;
                    ButtonSetCustom.Background = new SolidColorBrush(c);
                    //SendToDevice("setcolour?colour=" + c.R.ToString("X2") + c.G.ToString("X2") + c.B.ToString("X2") + "&level=" + ((int)SliderBrightness.Value).ToString());
                }
            }
        }

        private void ButtonSetCustom_Click(object sender, RoutedEventArgs e)
        {
            if (LEDColorPicker.Color != null)
            {
                Color c = LEDColorPicker.Color;
                if (c != null && TextCustomColour != null)
                {
                    SelectedColour = c;
                    FunctionMode = 0;
                    ButtonSetCustom.Background = new SolidColorBrush(c);
                    SendToDevice("setcolour?colour=" + c.R.ToString("X2") + c.G.ToString("X2") + c.B.ToString("X2") + "&level=" + ((int)SliderBrightness.Value).ToString());
                }
            }
        }

        private void SliderBrightness_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (FunctionMode == 0)
            {
                SendToDevice("setcolour?colour=" + SelectedColour.R.ToString("X2") + SelectedColour.G.ToString("X2") + SelectedColour.B.ToString("X2") + "&level=" + ((int)SliderBrightness.Value).ToString());
            }
            else
            {
               SendToDevice("setbrightness?level=" + ((int)SliderBrightness.Value).ToString());
            }
        }
    }
}
