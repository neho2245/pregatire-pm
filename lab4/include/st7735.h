#ifndef ST7735_H
#define ST7735_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define ST7735_CS_PORT   PORTB
#define ST7735_CS_DDR    DDRB
#define ST7735_CS_PIN    PB0

#define ST7735_RST_PORT  PORTB
#define ST7735_RST_DDR   DDRB
#define ST7735_RST_PIN   PB1

#define ST7735_DC_PORT   PORTB
#define ST7735_DC_DDR    DDRB
#define ST7735_DC_PIN    PB4

#define ST7735_LED_PORT  PORTA
#define ST7735_LED_DDR   DDRA
#define ST7735_LED_PIN   PA7

#define CS_LOW()    (ST7735_CS_PORT  &= ~(1 << ST7735_CS_PIN))
#define CS_HIGH()   (ST7735_CS_PORT  |=  (1 << ST7735_CS_PIN))
#define DC_CMD()    (ST7735_DC_PORT  &= ~(1 << ST7735_DC_PIN))
#define DC_DATA()   (ST7735_DC_PORT  |=  (1 << ST7735_DC_PIN))
#define RST_LOW()   (ST7735_RST_PORT &= ~(1 << ST7735_RST_PIN))
#define RST_HIGH()  (ST7735_RST_PORT |=  (1 << ST7735_RST_PIN))
#define LED_ON()    (ST7735_LED_PORT |=  (1 << ST7735_LED_PIN))
#define LED_OFF()   (ST7735_LED_PORT &= ~(1 << ST7735_LED_PIN))

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_SLPOUT  0x11
#define ST7735_NORON   0x13
#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36
#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5
#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

#define ST7735_WIDTH   128
#define ST7735_HEIGHT  160

#define COLOR_BLACK   0x0000
#define COLOR_WHITE   0xFFFF
#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_BLUE    0x001F
#define COLOR_YELLOW  0xFFE0
#define COLOR_CYAN    0x07FF
#define COLOR_MAGENTA 0xF81F

void st7735_init(void);
void st7735_fill_screen(uint16_t color);
void st7735_draw_pixel(uint8_t x, uint8_t y, uint16_t color);
void st7735_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);
void st7735_set_addr_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void st7735_push_color(uint16_t color);

#endif