#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include <stdbool.h>

#define BLACK 						0x000000
#define WHITE 						0xffffff
#define RED 						0xff0000
#define BLUE 						0x0000ff
#define LCD_DISPLAY_WIDTH_PIXELS	480
#define LCD_DISPLAY_HEIGHT_PIXELS	272

void lcd_init_1_2(void);
void lcd_select_buffer(uint8_t buffer);
void lcd_pixel_1(int16_t x, int16_t y, bool colour);
void lcd_pixel_2(int16_t x, int16_t y, uint32_t colour);
void lcd_filled_rectangle_1(int16_t start_x,
		int16_t start_y,
		int16_t width,
		int16_t height,
		bool colour);
void lcd_filled_rectangle_2(int16_t start_x,
		int16_t start_y,
		int16_t width,
		int16_t height,
		uint32_t colour);
void lcd_character_1(int16_t x, int16_t y, char c);
void lcd_string_1(int16_t x, int16_t y, const char *s);
void lcd_scroll_display_up_1(int16_t lines);

#endif
