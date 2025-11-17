#ifndef ILI9488_DRIVER_H
#define ILI9488_DRIVER_H

#include <stdint.h>
#include <gpiod.h>

//Display dimension
#define ILI9488_WIDTH  480
#define ILI9488_HEIGHT 320

//ILI9488 Commands
#define ILI9488_SWRESET 0x01
#define ILI9488_SLPOUT  0x11
#define ILI9488_DISPON  0x29
#define ILI9488_CASET   0x2A
#define ILI9488_PASET   0x2B
#define ILI9488_RAMWR   0x2C
#define ILI9488_MADCTL  0x36
#define ILI9488_COLMOD  0x3A

//Display Rotation modes
typedef enum {
    ROTATION_PORTRAIT = 0x48,
    ROTATION_LANDSCAPE = 0x28,
    ROTATION_PORTRAIT_REVERSE = 0x88,
    ROTATION_LANDSCAPE_REVERSE = 0xE8
}display_rotation_t;

//Color Structure
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
}rgb_color_t;

//Predefined colors
#define WHITE {0xFF, 0xFF, 0xFF}
#define BLACK {0x00, 0x00, 0x00}
#define RED   {0xFF, 0x00, 0x00}
#define GREEN {0x00, 0xFF, 0x00}
#define BLUE  {0x00, 0x00, 0xFF}

//SPI device
typedef struct {
    int spi_fd;
    uint32_t spi_speed;
    struct gpiod_chip *gpio_chip;
    struct gpiod_line *dc_line;
    struct gpiod_line *reset_line;
    struct gpiod_line *bl_line;
    display_rotation_t rotation;
}ili9488_dev_t;

//Driver API
int ili9488_init(ili9488_dev_t *dev, const char *spi_device, uint32_t spi_speed, const char *gpio_chip_name, int dc_pin, int reset_pin, int bl_pin);
void ili9488_cleanup(ili9488_dev_t *dev);

//Display control
void ili9488_set_rotation(ili9488_dev_t *dev, display_rotation_t rotation);
void ili9488_set_backlight(ili9488_dev_t *dev, int on);
void ili9488_fill_screen(ili9488_dev_t *dev, rgb_color_t color);
void ili9488_clear_screen(ili9488_dev_t *dev);

//Drawing Functions
void ili9488_draw_pixel(ili9488_dev_t *dev, uint16_t x, uint16_t y, rgb_color_t color);
void ili9488_fill_rect(ili9488_dev_t *dev, uint16_t x, uint16_t y, uint16_t w, uint16_t h, rgb_color_t color);
void ili9488_draw_image(ili9488_dev_t *dev, const uint8_t *image_data, uint16_t width, uint16_t height, uint16_t x, uint16_t y);

//Text Functions
void ili9488_draw_char(ili9488_dev_t *dev, char c, uint16_t x, uint16_t y, uint8_t scale, rgb_color_t color);
void ili9488_draw_string(ili9488_dev_t *dev, const char *str, uint16_t x, uint16_t y, uint8_t scale, rgb_color_t color);

#endif
