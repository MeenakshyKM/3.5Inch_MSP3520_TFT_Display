#include "ili9488_driver.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <linux/spi/spidev.h>
//#include <stdlib.h>
//#include <string.h>
//#include <unistd.h>


// Font data for common characters (simplified)
const uint8_t font_8x8[95][8] = {  // ASCII 32-126
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // 32: space
    {0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18},   // 33: !
    {0x6C, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00},   // 34: "
    {0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00},   // 35: #
    {0x0C, 0x3F, 0x68, 0x3E, 0x0B, 0x7E, 0x18, 0x00},   // 36: $
    {0x60, 0x66, 0x0C, 0x18, 0x30, 0x66, 0x06, 0x00},   // 37: %
    {0x38, 0x6C, 0x6C, 0x38, 0x6D, 0x66, 0x3B, 0x00},   // 38: &
    {0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00},   // 39: '
    {0x0C, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0C, 0x00},   // 40: (
    {0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x18, 0x30, 0x00},   // 41: )
    {0x00, 0x18, 0x7E, 0x3C, 0x7E, 0x18, 0x00, 0x00},   // 42: *
    {0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00},   // 43: +
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30},   // 44: ,
    {0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00},   // 45: -
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00},   // 46: .
    {0x00, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x00, 0x00},   // 47: /
    {0x3C, 0x66, 0x6E, 0x7E, 0x76, 0x66, 0x3C, 0x00},   // 48: 0
    {0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00},   // 49: 1
    {0x3C, 0x66, 0x06, 0x0C, 0x18, 0x30, 0x7E, 0x00},   // 50: 2
    {0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C, 0x00},   // 51: 3
    {0x0C, 0x1C, 0x3C, 0x6C, 0x7E, 0x0C, 0x0C, 0x00},   // 52: 4
    {0x7E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00},   // 53: 5
    {0x3C, 0x66, 0x60, 0x7C, 0x66, 0x66, 0x3C, 0x00},   // 54: 6
    {0x7E, 0x06, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x00},   // 55: 7
    {0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00},   // 56: 8
    {0x3C, 0x66, 0x66, 0x3E, 0x06, 0x66, 0x3C, 0x00},   // 57: 9
    {0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00},   // 58: :
    {0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x30},   // 59: ;
    {0x0C, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0C, 0x00},   // 60: <
    {0x00, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x00},   // 61: =
    {0x30, 0x18, 0x0C, 0x06, 0x0C, 0x18, 0x30, 0x00},   // 62: >
    {0x3C, 0x66, 0x06, 0x0C, 0x18, 0x00, 0x18, 0x00},   // 63: ?
    {0x3C, 0x66, 0x6E, 0x6A, 0x6E, 0x60, 0x3C, 0x00},   // 64: @
    {0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00},   // 65: A
    {0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00},   // 66: B
    {0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00},   // 67: C
    {0x78, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00},   // 68: D
    {0x7E, 0x60, 0x60, 0x78, 0x60, 0x60, 0x7E, 0x00},   // 69: E
    {0x7E, 0x60, 0x60, 0x78, 0x60, 0x60, 0x60, 0x00},   // 70: F
    {0x3C, 0x66, 0x60, 0x6E, 0x66, 0x66, 0x3C, 0x00},   // 71: G
    {0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00},   // 72: H
    {0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00},   // 73: I
    {0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x6C, 0x38, 0x00},   // 74: J
    {0x66, 0x6C, 0x78, 0x70, 0x78, 0x6C, 0x66, 0x00},   // 75: K
    {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00},   // 76: L
    {0x63, 0x77, 0x7F, 0x6B, 0x6B, 0x63, 0x63, 0x00},   // 77: M
    {0x66, 0x76, 0x7E, 0x7E, 0x6E, 0x66, 0x66, 0x00},   // 78: N
    {0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00},   // 79: O
    {0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x00},   // 80: P
    {0x3C, 0x66, 0x66, 0x66, 0x6A, 0x6C, 0x36, 0x00},   // 81: Q
    {0x7C, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0x66, 0x00},   // 82: R
    {0x3C, 0x66, 0x60, 0x3C, 0x06, 0x66, 0x3C, 0x00},   // 83: S
    {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00},   // 84: T
    {0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00},   // 85: U
    {0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00},   // 86: V
    {0x63, 0x63, 0x6B, 0x6B, 0x7F, 0x77, 0x63, 0x00},   // 87: W
    {0x66, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x66, 0x00},   // 88: X
    {0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x00},   // 89: Y
    {0x7E, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x7E, 0x00},   // 90: Z
    {0x3C, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3C, 0x00},   // 91: [
    {0x00, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x00, 0x00},   // 92: backslash
    {0x3C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x3C, 0x00},   // 93: ]
    {0x18, 0x3C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00},   // 94: ^
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF},   // 95: _
    {0x30, 0x18, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00},   // 96: `
    {0x00, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x3E, 0x00},   // 97: a
    {0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x7C, 0x00},   // 98: b
    {0x00, 0x00, 0x3C, 0x66, 0x60, 0x66, 0x3C, 0x00},   // 99: c
    {0x06, 0x06, 0x3E, 0x66, 0x66, 0x66, 0x3E, 0x00},   // 100: d
    {0x00, 0x00, 0x3C, 0x66, 0x7E, 0x60, 0x3C, 0x00},   // 101: e
    {0x1C, 0x30, 0x7C, 0x30, 0x30, 0x30, 0x30, 0x00},   // 102: f
    {0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x3C},   // 103: g
    {0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x00},   // 104: h
    {0x18, 0x00, 0x18, 0x18, 0x18, 0x18, 0x0C, 0x00},   // 105: i
    {0x0C, 0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x78},   // 106: j
    {0x60, 0x60, 0x66, 0x6C, 0x78, 0x6C, 0x66, 0x00},   // 107: k
    {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x0C, 0x00},   // 108: l
    {0x00, 0x00, 0x76, 0x7F, 0x6B, 0x6B, 0x63, 0x00},   // 109: m
    {0x00, 0x00, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x00},   // 110: n
    {0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x00},   // 111: o
    {0x00, 0x00, 0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60},   // 112: p
    {0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x06},   // 113: q
    {0x00, 0x00, 0x7C, 0x66, 0x60, 0x60, 0x60, 0x00},   // 114: r
    {0x00, 0x00, 0x3E, 0x60, 0x3C, 0x06, 0x7C, 0x00},   // 115: s
    {0x30, 0x30, 0x7C, 0x30, 0x30, 0x30, 0x1C, 0x00},   // 116: t
    {0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x00},   // 117: u
    {0x00, 0x00, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00},   // 118: v
    {0x00, 0x00, 0x63, 0x6B, 0x6B, 0x7F, 0x36, 0x00},   // 119: w
    {0x00, 0x00, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x00},   // 120: x
    {0x00, 0x00, 0x66, 0x66, 0x66, 0x3E, 0x06, 0x3C},   // 121: y
    {0x00, 0x00, 0x7E, 0x0C, 0x18, 0x30, 0x7E, 0x00},   // 122: z
    {0x0C, 0x18, 0x18, 0x70, 0x18, 0x18, 0x0C, 0x00},   // 123: {
    {0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00},   // 124: |
    {0x30, 0x18, 0x18, 0x0E, 0x18, 0x18, 0x30, 0x00},   // 125: }
    {0x31, 0x6B, 0x46, 0x00, 0x00, 0x00, 0x00, 0x00},   // 126: ~
};

//=================================================
//Static Helper Functions
//=================================================

static void spi_transfer(ili9488_dev_t *dev, uint8_t *tx, uint8_t *rx, int len) {
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = len,
        .speed_hz = dev->spi_speed,
        .bits_per_word = 8,
    };

    if (ioctl(dev->spi_fd, SPI_IOC_MESSAGE(1), &tr) < 0) {
        perror("SPI transfer failed \n");
    }
}

static void send_command(ili9488_dev_t *dev, uint8_t cmd) {
    gpiod_line_set_value(dev->dc_line, 0);       //DC should be low while sending commands
    spi_transfer(dev, &cmd, NULL, 1);
}

static void send_data(ili9488_dev_t *dev, uint8_t data) {
    gpiod_line_set_value(dev->dc_line, 1);      //DC should be high while sending data
    spi_transfer(dev, &data, NULL, 1);
}

static void send_data_multiple(ili9488_dev_t *dev, uint8_t *data, int len) {
    gpiod_line_set_value(dev->dc_line, 1);
    spi_transfer(dev, data, NULL, len);
}

static void set_address_window(ili9488_dev_t *dev, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    send_command(dev, ILI9488_CASET);   //Column Address set
    send_data(dev, x1 >> 8);
    send_data(dev, x1 & 0xFF);
    send_data(dev, x2 >> 8);
    send_data(dev, x2 & 0xFF);
    
    send_command(dev, ILI9488_PASET); // Page address set
    send_data(dev, y1 >> 8);
    send_data(dev, y1 & 0xFF);
    send_data(dev, y2 >> 8);
    send_data(dev, y2 & 0xFF);
    
    send_command(dev, ILI9488_RAMWR); // Memory write
}


//=================================================
//Driver API Functions
//=================================================

int ili9488_init(ili9488_dev_t *dev, const char *spi_device, uint32_t spi_speed, const char *gpio_chip_name, int dc_pin, int reset_pin, int bl_pin) {

    //Initialize SPI

    dev->spi_fd = open(spi_device, O_RDWR);
    if (dev->spi_fd < 0) {
        perror("Failed to open SPI device \n");
        return -1;
    }

    //Set SPI mode as 0
    uint8_t mode = SPI_MODE_0;
    if (ioctl(dev->spi_fd, SPI_IOC_WR_MODE, &mode) < 0) {
        perror("Failed to set SPI mode \n");
        close(dev->spi_fd);
        return -1;
    }

    //Set bits per word
    uint8_t bits = 8;
    if (ioctl(dev->spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) {
        perror("Failed to set bits per word \n");
        close(dev->spi_fd);
        return -1;
    }

    //Set max speed
    dev->spi_speed = spi_speed;
    if (ioctl(dev->spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &dev->spi_speed) < 0) {
        perror("Failed to set SPI speed \n");
        close(dev->spi_fd);
        return -1;
    }

    //Initialize GPIO
    dev->gpio_chip = gpiod_chip_open_by_name(gpio_chip_name);
    if (!dev->gpio_chip) {
        perror("Failed to open GPIO chip \n");
        close(dev->spi_fd);
        return -1;
    }

    dev->dc_line = gpiod_chip_get_line(dev->gpio_chip, dc_pin);
    dev->reset_line = gpiod_chip_get_line(dev->gpio_chip, reset_pin);
    dev->bl_line = gpiod_chip_get_line(dev->gpio_chip, bl_pin);

    if (!dev->dc_line || !dev->reset_line || !dev->bl_line) {
        perror("Failed to get GPIO lines \n");
        gpiod_chip_close(dev->gpio_chip);
        close(dev->spi_fd);
        return -1;
    }

    //Request lines as output
    if(gpiod_line_request_output(dev->dc_line, "ili9488-dc", 0) < 0 || 
        gpiod_line_request_output(dev->reset_line, "ili9488-reset", 1) < 0 ||
        gpiod_line_request_output(dev->bl_line, "ili9488-backlight", 0) < 0 ) {

            perror("Failed to request GPIO lines \n");
            gpiod_chip_close(dev->gpio_chip);
            close(dev->spi_fd);
            return -1;
    }

    //Reset display
    gpiod_line_set_value(dev->reset_line, 0);
    usleep(100000);
    gpiod_line_set_value(dev->reset_line, 1);
    usleep(100000);

    //Initialize ILI9488
    send_command(dev, ILI9488_SWRESET);
    usleep(120000);

    send_command(dev, ILI9488_SLPOUT);
    usleep(120000);

    send_command(dev, ILI9488_COLMOD);
    send_data(dev, 0x66);  //18 bits per pixel

    //Default rotation
    dev->rotation = ROTATION_LANDSCAPE_REVERSE;
    send_command(dev, ILI9488_MADCTL);
    send_data(dev, dev->rotation);

    send_command(dev, ILI9488_DISPON);
    usleep(120000);

    printf("ILI9488 Initialized successfully \n");
    return 0;
}

void ili9488_cleanup(ili9488_dev_t *dev) {
    if(dev->bl_line) {
        gpiod_line_set_value(dev->bl_line, 0);
        gpiod_line_release(dev->bl_line);
    }

    if(dev->reset_line) 
        gpiod_line_release(dev->reset_line);

    if(dev->dc_line)
        gpiod_line_release(dev->dc_line);
    
    if(dev->gpio_chip)
        gpiod_chip_close(dev->gpio_chip);
    
    if(dev->spi_fd >= 0)
        close(dev->spi_fd);
}

void ili9488_set_rotation(ili9488_dev_t *dev, display_rotation_t rotation) {
    dev->rotation = rotation;
    send_command(dev, ILI9488_MADCTL);
    send_data(dev, rotation);
}

void ili9488_set_backlight(ili9488_dev_t *dev, int on) {
    gpiod_line_set_value(dev->bl_line, on ? 1 : 0);
}

void ili9488_fill_screen(ili9488_dev_t *dev, rgb_color_t color) {
    set_address_window(dev, 0, 0, ILI9488_WIDTH - 1, ILI9488_HEIGHT - 1);

    int total_pixels = ILI9488_WIDTH * ILI9488_HEIGHT;
    int chunk_size = 1024;
    uint8_t *chunk_buffer = malloc(chunk_size * 3);

    if(!chunk_buffer) {
        perror("Failed to allocate chunk buffer \n");
        return;
    }

    //Fill chunk with desired color
    for (int i=0; i < chunk_size * 3; i += 3) {
        chunk_buffer[i] = color.r;
        chunk_buffer[i + 1] = color.g;
        chunk_buffer[i + 2] = color.b;
    }

    //Send data in chunks
    int remaining_pixels = total_pixels;
    while (remaining_pixels > 0) {
        int pixels_to_send = (remaining_pixels > chunk_size) ? chunk_size : remaining_pixels;
        int bytes_to_send = pixels_to_send * 3;

        send_data_multiple(dev, chunk_buffer, bytes_to_send);
        remaining_pixels -= pixels_to_send;
    }
    free(chunk_buffer);
}

void ili9488_clear_screen(ili9488_dev_t *dev) {
    rgb_color_t black = BLACK;
    ili9488_fill_screen(dev, black);
}

void ili9488_draw_pixel(ili9488_dev_t *dev, uint16_t x, uint16_t y, rgb_color_t color) {
    if (x >= ILI9488_WIDTH || y >= ILI9488_HEIGHT)
        return;
    
    set_address_window(dev,x, y, x, y);

    uint8_t pixel_data[3] = {color.r, color.g, color.b};
    send_data_multiple(dev, pixel_data, 3);
}

void ili9488_fill_rect(ili9488_dev_t *dev, uint16_t x, uint16_t y, uint16_t w, uint16_t h, rgb_color_t color) {
    if (x >= ILI9488_WIDTH || y >= ILI9488_HEIGHT)
        return;
    
    uint16_t x1 = (x + w - 1) < ILI9488_WIDTH ? (x + w -1) : (ILI9488_WIDTH - 1);
    uint16_t y1 = (y + h - 1) < ILI9488_HEIGHT ? (y + h - 1) : (ILI9488_HEIGHT - 1);

    set_address_window(dev, x, y, x1, y1);

    int total_pixels = (x1 - x + 1) * (y1 - y + 1);
    int chunk_size = 512;
    uint8_t *chunk_buffer = malloc(chunk_size * 3);

    if(!chunk_buffer) {
        perror("Failed to allocate chunk buffer \n");
        return;
    }

    for (int i = 0; i < chunk_size * 3; i += 3) {
        chunk_buffer[i] = color.r;
        chunk_buffer[i + 1] = color.g;
        chunk_buffer[i + 2] = color.b;
    }
    
    int remaining_pixels = total_pixels;
    while (remaining_pixels > 0) {
        int pixels_to_send = (remaining_pixels > chunk_size) ? chunk_size : remaining_pixels;
        int bytes_to_send = pixels_to_send * 3;
        
        send_data_multiple(dev, chunk_buffer, bytes_to_send);
        remaining_pixels -= pixels_to_send;
    }
    
    free(chunk_buffer);
}

void ili9488_draw_image(ili9488_dev_t *dev, const uint8_t *image_data, uint16_t width, uint16_t height, uint16_t x, uint16_t y) {
    set_address_window(dev, x, y, x + width - 1, y + height - 1);

    int total_bytes  = width * height * 3;
    int chunk_size = 2048;
    int bytes_processed = 0;

    while (bytes_processed < total_bytes) {
        int bytes_to_send = (total_bytes - bytes_processed > chunk_size) ? chunk_size : total_bytes - bytes_processed;
        send_data_multiple(dev, (uint8_t*)(image_data + bytes_processed), bytes_to_send);
        bytes_processed +=  bytes_to_send;
        usleep(500);
    }
}

void ili9488_draw_char(ili9488_dev_t *dev, char c, uint16_t x, uint16_t y, uint8_t scale, rgb_color_t color) {
    if (c < 32 || c > 126)
        c = '?';
    const uint8_t *char_data = font_8x8[c - 32];

    for (uint8_t row = 0; row < 8; row++) {
        uint8_t line = char_data[row];
        for (uint8_t col = 0; col < 8; col++) {
            if (line & (1 << (7 - col))) {
                uint16_t start_x = x + col * scale;
                uint16_t start_y = y + row * scale;
                uint16_t end_x = start_x + scale - 1;
                uint16_t end_y = start_y + scale - 1;

                set_address_window(dev, start_x, start_y, end_x, end_y);

                int block_pixels = scale * scale;
                for (int i = 0; i < block_pixels; i++) {
                    uint8_t pixel_data[3] = {color.r, color.g, color.b};
                    send_data_multiple(dev, pixel_data, 3);
                }
            }
        }
    }
}

void ili9488_draw_string(ili9488_dev_t *dev, const char *str, uint16_t x, uint16_t y, uint8_t scale, rgb_color_t color) {
    uint16_t current_x = x;
    uint16_t current_y = y;
    uint16_t char_width = (8 + 1) * scale;
    uint16_t char_height = (8 + 2) * scale;
    
    while (*str) {
        ili9488_draw_char(dev, *str, current_x, current_y, scale, color);
        current_x += char_width;
        
        if (current_x + 8 * scale >= ILI9488_WIDTH) {
            current_x = x;
            current_y += char_height;
            
            if (current_y + 8 * scale >= ILI9488_HEIGHT) {
                break;
            }
        }
        
        str++;
    }
}