#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

#define LCD_DISPLAY_WIDTH_PIXELS	480
#define LCD_DISPLAY_HEIGHT_PIXELS	272
#define BLACK 						0x000000
#define WHITE 						0xffffff
#define RED 						0xff0000
#define ROW_HEIGHT					16

void lcd_init(void);
void lcd_filled_rectangle(int16_t start_x,
		int16_t start_y,
		int16_t width,
		int16_t height,
		uint32_t colour);
void lcd_string(int16_t x, int16_t y, const char *s, uint32_t colour);

#endif /* LCD_DRIVER_H_ */
