#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

#define LCD_DISPLAY_WIDTH_PIXELS	480
#define LCD_DISPLAY_HEIGHT_PIXELS	272
#define BLACK 						0x0000
#define WHITE 						0xffff
#define RED 						0xf800
#define BLUE 						0x001f

void lcd_init(void);
void lcd_filled_rectangle(int16_t start_x,
		int16_t start_y,
		int16_t width,
		int16_t height,
		uint32_t colour);

#endif
