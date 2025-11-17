#include <stdio.h>
#include <unistd.h>
#include "ili9488_driver.h"
#include "logo.h"

int main() {
    printf("Starting ILI9488 Driver Test...\n");
    
    ili9488_dev_t display;
    rgb_color_t white = WHITE;
    rgb_color_t black = BLACK;
    
    // Initialize display
    if (ili9488_init(&display, "/dev/spidev0.0", 10000000, "gpiochip2", 1, 2, 3) < 0) {
        fprintf(stderr, "Failed to initialize display\n");
        return -1;
    }
    
    // Turn on backlight
    ili9488_set_backlight(&display, 1);
    
    printf("Filling screen with white...\n");
    ili9488_fill_screen(&display, white);
    
    printf("Displaying QinnTech logo...\n");
    // Center the logo
    uint16_t logo_x = (ILI9488_WIDTH - qinn_logo_width) / 2;
    uint16_t logo_y = (ILI9488_HEIGHT - qinn_logo_height) / 2;
    
    ili9488_draw_image(&display, qinn_logo, qinn_logo_width, qinn_logo_height, logo_x, logo_y);
    sleep(5);
    
    printf("Clearing screen and displaying text...\n");
    ili9488_clear_screen(&display);
    
    rgb_color_t text_color = {0xFF, 0xFF, 0xFF}; // White
    ili9488_draw_string(&display, "Datalogger", 80, 130, 4, text_color);
    sleep(3);
    
    printf("Test completed\n");
    ili9488_cleanup(&display);
    
    return 0;
}