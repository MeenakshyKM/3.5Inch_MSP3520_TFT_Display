# 3.5Inch_MSP3520_TFT_Display
This 3.5" MSP3520 TFT display uses ILI9488 controller for display and ADS7846 controller for touch. I tested this device on verdin AM62 integrated on verdin development board. Currently this repository has the source code for display.

This include the functions to :-
  
  Initialize the display
  
  cleanup the display
  
  set the rotation, whether landscape or portrait mode
  
  set the backlight
  
  fill the screen with desired color
  
  clear the screen with black color
  
  draw a single pixel
  
  fill a color in a rectangular area
  
  draw an image
  
  Write a character
  
  Write a string

Also, I'm running this code as a docker container. 

Commands to run :-

docker build --platform linux/arm64 -t ili9488 .

docker save ili9488 -o ili9488.tar

scp ili9488.tar torizon@192.168.0.131:/home/torizon/

docker load -i ili9488.tar

docker run -it --rm --privileged --device /dev/spidev0.0 --device /dev/gpiochip2 -v /dev:/dev -v /sys:/sys ili9488


docker load -i ili9488.tar
docker run -it --rm --privileged --device /dev/spidev0.0 --device /dev/gpiochip2 -v /dev:/dev -v /sys:/sys ili9488
