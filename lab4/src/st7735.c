#include "st7735.h"
#include "inttypes.h"

static void spi_init(void) {

    DDRB |= (1 << PB5) | (1 << PB7) | (1 << PB4) | (1 << PB0) | (1 << PB1);

    ST7735_LED_DDR |= (1 << ST7735_LED_PIN);

    SPCR0 = (1 << SPE0) | (1 << MSTR0);
}

static uint8_t spi_transfer(uint8_t data) {
    SPDR0 = data;
    while (!(SPSR0 & (1 << SPIF0)))
        ;
    return SPDR0;
}

static void st7735_write_cmd(uint8_t cmd) {
    DC_CMD();
    CS_LOW();
    spi_transfer(cmd);
    CS_HIGH();
}

static void st7735_write_data(uint8_t data) {
    DC_DATA();
    CS_LOW();
    spi_transfer(data);
    CS_HIGH();
}

static void st7735_hw_reset(void) {
    RST_HIGH();
    _delay_ms(10);
    RST_LOW();
    _delay_ms(50);
    RST_HIGH();
    _delay_ms(150);
}

void st7735_init(void) {
    spi_init();
    CS_HIGH();
    st7735_hw_reset();

    st7735_write_cmd(ST7735_SWRESET);
    _delay_ms(150);

    st7735_write_cmd(ST7735_SLPOUT);
    _delay_ms(255);

    st7735_write_cmd(ST7735_FRMCTR1);
    st7735_write_data(0x01);
    st7735_write_data(0x2C);
    st7735_write_data(0x2D);

    st7735_write_cmd(ST7735_FRMCTR2);
    st7735_write_data(0x01);
    st7735_write_data(0x2C);
    st7735_write_data(0x2D);

    st7735_write_cmd(ST7735_FRMCTR3);
    st7735_write_data(0x01); st7735_write_data(0x2C); st7735_write_data(0x2D);
    st7735_write_data(0x01); st7735_write_data(0x2C); st7735_write_data(0x2D);

    st7735_write_cmd(ST7735_INVCTR);
    st7735_write_data(0x07);

    st7735_write_cmd(ST7735_PWCTR1);
    st7735_write_data(0xA2);
    st7735_write_data(0x02);
    st7735_write_data(0x84);

    st7735_write_cmd(ST7735_PWCTR2);
    st7735_write_data(0xC5);

    st7735_write_cmd(ST7735_PWCTR3);
    st7735_write_data(0x0A);
    st7735_write_data(0x00);

    st7735_write_cmd(ST7735_PWCTR4);
    st7735_write_data(0x8A);
    st7735_write_data(0x2A);

    st7735_write_cmd(ST7735_PWCTR5);
    st7735_write_data(0x8A);
    st7735_write_data(0xEE);

    st7735_write_cmd(ST7735_VMCTR1);
    st7735_write_data(0x0E);

    st7735_write_cmd(ST7735_INVOFF);

    st7735_write_cmd(ST7735_MADCTL);
    st7735_write_data(0x00);

    st7735_write_cmd(ST7735_COLMOD);
    st7735_write_data(0x05);

    st7735_write_cmd(ST7735_GMCTRP1);
    const uint8_t gp[] = {0x02,0x1C,0x07,0x12,0x37,0x32,0x29,0x2D,
                          0x29,0x25,0x2B,0x39,0x00,0x01,0x03,0x10};
    for (uint8_t i = 0; i < 16; i++) st7735_write_data(gp[i]);

    st7735_write_cmd(ST7735_GMCTRN1);
    const uint8_t gn[] = {0x03,0x1D,0x07,0x06,0x2E,0x2C,0x29,0x2D,
                          0x2E,0x2E,0x37,0x3F,0x00,0x00,0x02,0x10};
    for (uint8_t i = 0; i < 16; i++) st7735_write_data(gn[i]);

    st7735_write_cmd(ST7735_NORON);
    _delay_ms(10);

    st7735_write_cmd(ST7735_DISPON);
    _delay_ms(100);

    LED_ON();
}

void st7735_set_addr_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    st7735_write_cmd(ST7735_CASET);
    st7735_write_data(0x00); st7735_write_data(x0);
    st7735_write_data(0x00); st7735_write_data(x1);

    st7735_write_cmd(ST7735_RASET);
    st7735_write_data(0x00); st7735_write_data(y0);
    st7735_write_data(0x00); st7735_write_data(y1);

    st7735_write_cmd(ST7735_RAMWR);
}

void st7735_push_color(uint16_t color) {
    DC_DATA();
    CS_LOW();
    spi_transfer(color >> 8);
    spi_transfer(color & 0xFF);
    CS_HIGH();
}

void st7735_draw_pixel(uint8_t x, uint8_t y, uint16_t color) {
    if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT) return;
    st7735_set_addr_window(x, y, x, y);
    st7735_push_color(color);
}

void st7735_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color) {
    if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT) return;
    if (x + w > ST7735_WIDTH)  w = ST7735_WIDTH  - x;
    if (y + h > ST7735_HEIGHT) h = ST7735_HEIGHT - y;

    st7735_set_addr_window(x, y, x + w - 1, y + h - 1);

    uint16_t total = (uint16_t)w * h;
    DC_DATA();
    CS_LOW();
    for (uint16_t i = 0; i < total; i++) {
        spi_transfer(color >> 8);
        spi_transfer(color & 0xFF);
    }
    CS_HIGH();
}

void st7735_fill_screen(uint16_t color) {
    st7735_fill_rect(0, 0, ST7735_WIDTH, ST7735_HEIGHT, color);
}