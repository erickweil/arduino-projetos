#ifndef HT_ST7735_H
#define HT_ST7735_H

#include <Arduino.h>

// Color definitions
#define	ST7735_BLACK   0x0000
#define	ST7735_BLUE    0x001F
#define	ST7735_RED     0xF800
#define	ST7735_GREEN   0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE   0xFFFF
#define ST7735_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

typedef enum {
	GAMMA_10 = 0x01,
	GAMMA_25 = 0x02,
	GAMMA_22 = 0x04,
	GAMMA_18 = 0x08
} GammaDef;

typedef struct {
    const uint8_t width;
    uint8_t height;
} FontDef;

FontDef Font_7x10 = {7,10};
FontDef Font_11x18 = {11,18};
FontDef Font_16x26 = {16,26};

class HT_st7735
{
public:
	HT_st7735(	int8_t cs_pin=-1,
				int8_t    rest_pin=-1,     
				int8_t    dc_pin=-1,    
				int8_t    sclk_pin=-1,   
				int8_t    mosi_pin=-1,  
				int8_t    led_k_pin=-1,
				int8_t    vtft_ctrl_pin=-1 ) {}
	~HT_st7735() {}
	void st7735_init(void) { }
	void st7735_draw_pixel(uint16_t x, uint16_t y, uint16_t color) { }
	void st7735_write_char(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor) {
        Serial.print("\n[");Serial.print(x);Serial.print(",");Serial.print(y);Serial.print("]");
        Serial.print(ch);
    }
	void st7735_write_str(uint16_t x, uint16_t y, String str_data, FontDef font=Font_11x18, uint16_t color=ST7735_BLUE, uint16_t bgcolor=ST7735_BLACK) {
        Serial.print("\n[");Serial.print(x);Serial.print(",");Serial.print(y);Serial.print("]");
        Serial.print(str_data);
    }
	void st7735_write_str(uint16_t x, uint16_t y, const char *str, FontDef font=Font_11x18, uint16_t color=ST7735_BLUE, uint16_t bgcolor=ST7735_BLACK) {
        Serial.print("\n[");Serial.print(x);Serial.print(",");Serial.print(y);Serial.print("]");
        Serial.print(str);
    }
	void st7735_fill_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {}
	void st7735_fill_screen(uint16_t color) {}
	void st7735_draw_image(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {}
	void st7735_invert_colors(bool invert) {}
	void st7735_set_gamma(GammaDef gamma) {}
};

#endif