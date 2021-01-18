# Wifi RGB LED Controller

This project is a Wi-Fi controlled RGB led controller using a ESP8266 Wi-Fi module connected to a RGB LED strip of 300 WS2811 LEDS. 

The esp8266-Firmware folder contains a Visual Studio Code project which can be compiled and uploaded to your ESP8266 module. The Wi-Fi network name and password will need to be changed to match your network and the static IP address needs to be configured to match your network. 
The code uses the FastLED driver system and there are links in the source code to the pages which each effect was copied from. 

Control of the light be either be via a web browser or using the Windows Application which has the source code in the RGBLightControllerUW folder. This can be compiled using Visual Studio to run on Windows 10 as a universal application.

More details about this project can be found on my blog at https://www.briandorey.com/post/esp8266-wifi-rgb-led-controller

FASTLED demos licensed under GNU General Public License v3.0
https://github.com/atuline/FastLED-Demos

Breath code from Marc Miller licensed under MIT License
https://github.com/marmilicious/FastLED_examples/blob/master/breath_effect_v2.ino
