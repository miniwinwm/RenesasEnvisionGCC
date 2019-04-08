#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "iodefine.h"
#include "lcd_driver.h"

extern const uint8_t Font9_Table[];

const uint32_t gr1_base_address = 0x00800000U + 480U * 272U * 2U;
const uint32_t gr2_base_address = 0x00800000U;

void lcd_init_1_2(void)
{
	/* allow writing to PFSWE bit */
    MPC.PWPR.BIT.B0WI = 0U;

    /* enable writing to PFS registers. */
    MPC.PWPR.BIT.PFSWE = 1U;

    /* set LCD_CLK pin to peripheral i/o */
    MPC.PB5PFS.BYTE = 0x25U;
    PORTB.PMR.BIT.B5 = 1U;

    /* set LCD_TCON0 pin to peripheral i/o */
    MPC.PB4PFS.BYTE = 0x25U;
    PORTB.PMR.BIT.B4 = 1U;

    /* set LCD_TCON2 pin to peripheral i/o */
    MPC.PB2PFS.BYTE = 0x25U;
    PORTB.PMR.BIT.B2 = 1U;

    /* set LCD_TCON3 pin to peripheral i/o */
    MPC.PB1PFS.BYTE = 0x25U;
    PORTB.PMR.BIT.B1 = 1U;

    /* set LCD_DATA0 pin to peripheral i/o */
    MPC.PB0PFS.BYTE = 0x25U;
    PORTB.PMR.BIT.B0 = 1U;

    /* set LCD_DATA1 pin to peripheral i/o */
    MPC.PA7PFS.BYTE = 0x25U;
    PORTA.PMR.BIT.B7 = 1U;

    /* set LCD_DATA2 pin to peripheral i/o */
    MPC.PA6PFS.BYTE = 0x25U;
    PORTA.PMR.BIT.B6 = 1U;

    /* set LCD_DATA3 pin to peripheral i/o */
    MPC.PA5PFS.BYTE = 0x25U;
    PORTA.PMR.BIT.B5 = 1U;

    /* set LCD_DATA4 pin to peripheral i/o */
    MPC.PA4PFS.BYTE = 0x25U;
    PORTA.PMR.BIT.B4 = 1U;

    /* set LCD_DATA5 pin to peripheral i/o */
    MPC.PA3PFS.BYTE = 0x25U;
    PORTA.PMR.BIT.B3 = 1U;

    /* set LCD_DATA6 pin to peripheral i/o */
    MPC.PA2PFS.BYTE = 0x25U;
    PORTA.PMR.BIT.B2 = 1U;

    /* set LCD_DATA7 pin to peripheral i/o */
    MPC.PA1PFS.BYTE = 0x25U;
    PORTA.PMR.BIT.B1 = 1U;

    /* set LCD_DATA8 pin to peripheral i/o */
    MPC.PA0PFS.BYTE = 0x25U;
    PORTA.PMR.BIT.B0 = 1U;

    /* set LCD_DATA9 pin to peripheral i/o */
    MPC.PE7PFS.BYTE = 0x25U;
    PORTE.PMR.BIT.B7 = 1U;

    /* set LCD_DATA10 pin to peripheral i/o */
    MPC.PE6PFS.BYTE = 0x25U;
    PORTE.PMR.BIT.B6 = 1U;

    /* set LCD_DATA11 pin to peripheral i/o */
    MPC.PE5PFS.BYTE = 0x25U;
    PORTE.PMR.BIT.B5 = 1U;

    /* set LCD_DATA12 pin to peripheral i/o */
    MPC.PE4PFS.BYTE = 0x25U;
    PORTE.PMR.BIT.B4 = 1U;

    /* set LCD_DATA13 pin to peripheral i/o */
    MPC.PE3PFS.BYTE = 0x25U;
    PORTE.PMR.BIT.B3 = 1U;

    /* set LCD_DATA14 pin to peripheral i/o */
    MPC.PE2PFS.BYTE = 0x25U;
    PORTE.PMR.BIT.B2 = 1U;

    /* set LCD_DATA15 pin to peripheral i/o */
    MPC.PE1PFS.BYTE = 0x25U;
    PORTE.PMR.BIT.B1 = 1U;

    /* disable writing to PFS registers. */
    MPC.PWPR.BIT.PFSWE = 0U;

    /* disable writing of PFSWE bit. */
    MPC.PWPR.BIT.B0WI = 1U;

    /* set up reset line and bring display out of reset */
	PORT6.PMR.BIT.B3 = 0U;	/* mode to gpio */
	PORT6.PDR.BIT.B3 = 1U; 	/* port direction output */
	PORT6.ODR0.BIT.B6 = 0U;	/* cmos */
	PORT6.PODR.BIT.B3 = 1U; /* bring display out of reset */

	/* set backlight pin as gpio output and switch backlight on */
	PORT6.PMR.BIT.B6 = 0U;	/* mode to gpio */
	PORT6.PDR.BIT.B6 = 1U;	/* port direction output */
	PORT6.ODR1.BIT.B4 = 0U;	/* cmos */
	PORT6.PODR.BIT.B6 = 1U;	/* backlight on */

	/* enable writing to clock control bit */
	SYSTEM.PRCR.WORD = 0xa502U;

	/* supply the peripheral clock to the glcd module */
	MSTP(GLCDC) = 0U;

	/* disable writing to clock control bit */
	SYSTEM.PRCR.WORD = 0xa500;

	/* release glcd module from a software reset status */
	GLCDC.BGEN.BIT.SWRST = 1U;

	/* set the dot clock to pll clock */
	GLCDC.PANELCLK.BIT.CLKSEL = 1U;

	/* set clock division ratio */
	GLCDC.PANELCLK.BIT.DCDR = 0x18U;

	/* selects pixel clock output to no division */
	GLCDC.PANELCLK.BIT.PIXSEL = 0U;

	/* enable panel clock LCD_CLK output */
	GLCDC.PANELCLK.BIT.CLKEN = 1U;

	/* waiting for supply of panel clock(LCD_CLK) and pixel clock(PXCLK)
	 * the BGMON.SWRST bit is sampled with PXCLK so if the CLKEN bit is set,
	 * the BGEN.SWRST bit is reflected on the BGMON.SWRST bit. */
	while (0U == GLCDC.BGMON.BIT.SWRST)
	{
		__asm("NOP");
	}

	/* set the panel signal timing */

	/* the outputs of LCD_DATA are synchronized with rising edges of LCD_CLK */
	GLCDC.CLKPHASE.BIT.LCDEDG = 0U;

	/* the output of LCD_TCON0 is synchronized with rising edges of LCD_CLK */
	GLCDC.CLKPHASE.BIT.TCON0EDG = 0U;

	/* the output of LCD_TCON2 is synchronized with rising edges of LCD_CLK */
	GLCDC.CLKPHASE.BIT.TCON2EDG = 0U;

	/* the output of LCD_TCON3 is synchronized with rising edges of LCD_CLK */
	GLCDC.CLKPHASE.BIT.TCON3EDG = 0U;

	/* horiz synch timing offset is 1 pixel */
	GLCDC.TCONTIM.BIT.OFFSET = 0U;

	/* vert synch timing offset is 1 pixel */
	GLCDC.TCONTIM.BIT.HALF = 0U;

	/* output signal select STHA */
	GLCDC.TCONSTHA2.BIT.SEL = 2U;

	/* invert the STHy signal */
	GLCDC.TCONSTHA2.BIT.INV = 1U;

	/* set hsync beginning position to no delay */
	GLCDC.TCONSTHA1.BIT.HS = 0U;

	/* select input hsync as reference */
	GLCDC.TCONSTHA2.BIT.HSSEL = 0U;

	/* STHy signal pulse width 1 pixel */
	GLCDC.TCONSTHA1.BIT.HW = 1U;

	/* vsync to STVA */
	GLCDC.TCONSTVA2.BIT.SEL = 0U;

	/* invert vsync */
	GLCDC.TCONSTVA2.BIT.INV = 1U;

	/* no vsync delay */
	GLCDC.TCONSTVA1.BIT.VS = 0U;

	/* pulse width 1 */
	GLCDC.TCONSTVA1.BIT.VW = 1U;

	/* output signal data enable */
	GLCDC.TCONSTHB2.BIT.SEL = 7U;

	/* don't invert data enable */
	GLCDC.TCONDE.BIT.INV = 0U;

	/* hsync delay */
	GLCDC.TCONSTHB1.BIT.HS = 0x29U;

	/* horiz width 480 */
	GLCDC.TCONSTHB1.BIT.HW = 480U;

	/* hsync is reference */
	GLCDC.TCONSTHB2.BIT.HSSEL = 0U;

	/* vsync 9 lines delay */
	GLCDC.TCONSTVB1.BIT.VS = 9U;

	/* vert height 272 */
	GLCDC.TCONSTVB1.BIT.VW = 272U;

	/* hsync period */
	GLCDC.BGPERI.BIT.FH = 0x20dU;

	/* vsync period */
	GLCDC.BGPERI.BIT.FV = 0x120U;

	/* hsync assert position */
	GLCDC.BGSYNC.BIT.HP = 3U;

	/* vsync assert position */
	GLCDC.BGSYNC.BIT.VP = 7U;

	/* horiz active start pixel */
	GLCDC.BGHSIZE.BIT.HP = 0x2cU;

	/* vert active start position */
	GLCDC.BGVSIZE.BIT.VP = 0x10U;

	/* horiz active pixel width */
	GLCDC.BGHSIZE.BIT.HW = 480U;

	/* vert active pixel height */
	GLCDC.BGVSIZE.BIT.VW = 272U;

	/* set background colour to black */
	GLCDC.BGCOLOR.BIT.R = 0U;
	GLCDC.BGCOLOR.BIT.G = 0U;
	GLCDC.BGCOLOR.BIT.B = 0U;

	/* frame buffer 1 1 bpp format */
	GLCDC.GR1FLM6.BIT.FORMAT = 7U;

	/* frame buffer 2 565 format */
	GLCDC.GR2FLM6.BIT.FORMAT = 0U;

	/* set start address of gr1 buffer */
	GLCDC.GR1FLM2 = gr1_base_address;

	/* set start address of gr2 buffer */
	GLCDC.GR2FLM2 = gr2_base_address;

	/* set background colour, both buffers */
	GLCDC.GR1BASE.BIT.R = 0U;
	GLCDC.GR1BASE.BIT.G = 0U;
	GLCDC.GR1BASE.BIT.B = 0U;
	GLCDC.GR2BASE.BIT.R = 0U;
	GLCDC.GR2BASE.BIT.G = 0U;
	GLCDC.GR2BASE.BIT.B = 0U;

	/* set number of 64 byte transfers - 1, gr1 */
	GLCDC.GR1FLM5.BIT.DATANUM = 0U;

	/* set number of 64 byte transfers - 1, gr2 */
	GLCDC.GR2FLM5.BIT.DATANUM = 0xeU;

	/* set number of lines in display, both buffers */
	GLCDC.GR1FLM5.BIT.LNNUM = 0x10fU;
	GLCDC.GR2FLM5.BIT.LNNUM = 0x10fU;

	/* line offset bytes gr1 */
	GLCDC.GR1FLM3.BIT.LNOFF = 64U;

	/* line offset bytes gr2 */
	GLCDC.GR2FLM3.BIT.LNOFF = 480U * 2U;

	/* height of graphical area, both buffers */
	GLCDC.GR1AB2.BIT.GRCVW = 272U;
	GLCDC.GR2AB2.BIT.GRCVW = 272U;

	/* graphical area vertical start position, both buffers */
	GLCDC.GR1AB2.BIT.GRCVS = 9U;
	GLCDC.GR2AB2.BIT.GRCVS = 9U;

	/* width of graphical area, both buffers */
	GLCDC.GR1AB3.BIT.GRCHW = 480U;
	GLCDC.GR2AB3.BIT.GRCHW = 480U;

	/* graphical area horizontal start position, both buffers */
	GLCDC.GR1AB3.BIT.GRCHS = 0x29U;
	GLCDC.GR2AB3.BIT.GRCHS = 0x29U;

	/* frame not displayed, both buffers */
	GLCDC.GR1AB1.BIT.GRCDISPON = 0U;
	GLCDC.GR2AB1.BIT.GRCDISPON = 0U;

	/* colour lookup table, gr1 only */
	GLCDC.GR1CLUT0[0].LONG = 0U;		/* black */
	GLCDC.GR1CLUT0[1].LONG = 0xffffU;	/* white */

	/* set show current graphics, both buffers, but buffer 2 is on top so this shows */
	GLCDC.GR1AB1.BIT.DISPSEL = 2U;
	GLCDC.GR2AB1.BIT.DISPSEL = 2U;

	/* disable chroma control, both buffers */
	GLCDC.GR1AB7.BIT.CKON = 0U;
	GLCDC.GR2AB7.BIT.CKON = 0U;

	/* configure the output control block */
	GLCDC.OUTSET.BIT.ENDIANON = 0U;

	/* byte order RGB */
	GLCDC.OUTSET.BIT.SWAPON = 0U;

	/* pixel format 565 */
	GLCDC.OUTSET.BIT.FORMAT = 2U;
	GLCDC.PANELDTHA.BIT.FORM = 2U;

	/* no pixel clock division */
	GLCDC.OUTSET.BIT.FRQSEL = 0U;

	/* rounding mode is truncate */
	GLCDC.PANELDTHA.BIT.SEL = 0U;

	/* brightness setting correction all midpoint */
	GLCDC.BRIGHT1.BIT.BRTG = 512U;
	GLCDC.BRIGHT2.BIT.BRTB = 512U;
	GLCDC.BRIGHT2.BIT.BRTR = 512U;

	/* contrast setting correction all midpoint */
	GLCDC.CONTRAST.BIT.CONTG = 128U;
	GLCDC.CONTRAST.BIT.CONTB = 128U;
	GLCDC.CONTRAST.BIT.CONTR = 128U;

	/* disable gamma correction */
	GLCDC.GAMSW.BIT.GAMON = 0U;

	/* set the GLCD detections and interrupts */

	/* disable VPOS line notification */
	GLCDC.DTCTEN.BIT.VPOSDTC = 0U;

	/* disable gr1 underflow notification */
	GLCDC.DTCTEN.BIT.GR1UFDTC = 0U;

	/* disable gr2 underflow notification */
	GLCDC.DTCTEN.BIT.GR2UFDTC = 0U;

	/* disable gr1 underflow interrupt */
	GLCDC.INTEN.BIT.GR1UFINTEN = 0U;
	EN(GLCDC, GR1UF) = 0U;
	while (0U != IS(GLCDC, GR1UF))
	{
		__asm("NOP");
	}

	/* disable gr2 underflow interrupt */
	GLCDC.INTEN.BIT.GR2UFINTEN = 0U;
	EN(GLCDC, GR2UF) = 0U;
	while (0U != IS(GLCDC,GR2UF))
	{
		__asm("NOP");
	}

	IEN(ICU, GROUPAL1) = 0U;
	IPR(ICU, GROUPAL1) = 0U;

	/* allow reading of frame buffer 1 */
	GLCDC.GR1FLMRD.BIT.RENB = 1U;

	/* allow reading of frame buffer 2 */
	GLCDC.GR2FLMRD.BIT.RENB = 1U;

	/* enable background generating block */
	GLCDC.BGEN.BIT.EN = 1U;

	/* enable register reflection */
	GLCDC.BGEN.BIT.VEN = 1U;

	/* release from software reset */
	GLCDC.BGEN.BIT.SWRST = 1U;

}

void lcd_select_buffer(uint8_t buffer)
{
	GLCDC.BGEN.BIT.EN = 0U;
	while (GLCDC.BGEN.BIT.EN == 1U)
	{
	}

	if (buffer == 1)
	{
		GLCDC.GR2AB1.BIT.DISPSEL = 1U;
	}
	else if (buffer == 2)
	{
		GLCDC.GR2AB1.BIT.DISPSEL = 2U;
	}

	/* enable background generating block */
	GLCDC.BGEN.BIT.EN = 1U;

	/* enable register reflection */
	GLCDC.BGEN.BIT.VEN = 1U;
}

void lcd_pixel_1(int16_t x, int16_t y, bool colour)
{
	uint8_t* address = (uint8_t*)gr1_base_address;
	uint8_t mask = 0x01U << (x % 8U);

	if (x < 0 || x >= LCD_DISPLAY_WIDTH_PIXELS || y < 0 || y >= LCD_DISPLAY_HEIGHT_PIXELS)
	{
		return;
	}

	address += x / 8U;
	address += y * 64U;

	if (colour)
	{
		*address |= mask;
	}
	else
	{
		*address &= ~mask;
	}
}

void lcd_pixel_2(int16_t x, int16_t y, uint32_t colour)
{
	if (x < 0 || x >= LCD_DISPLAY_WIDTH_PIXELS || y < 0 || y >= LCD_DISPLAY_HEIGHT_PIXELS)
	{
		return;
	}

	uint16_t* address = (uint16_t*)gr2_base_address;
	uint32_t converted_colour = ((colour & 0xf80000U) >> 8) |
			((colour & 0xfc00U) >> 5) |
			((colour & 0xf8U) >> 3);

	address += x;
	address += y * LCD_DISPLAY_WIDTH_PIXELS;
	*address = (uint16_t)converted_colour;
}

void lcd_filled_rectangle_1(int16_t start_x,
		int16_t start_y,
		int16_t width,
		int16_t height,
		bool colour)
{
	int16_t x;
	int16_t y;

	for (x = start_x; x < start_x + width; x++)
	{
		for (y = start_y; y < start_y + height; y++)
		{
			lcd_pixel_1(x, y, colour);
		}
	}
}

void lcd_filled_rectangle_2(int16_t start_x,
		int16_t start_y,
		int16_t width,
		int16_t height,
		uint32_t colour)
{
	int16_t end_x;
	int16_t end_y;
	int16_t x;
	int16_t y;
	uint16_t* address;
	uint32_t converted_colour = ((colour & 0xf80000U) >> 8) |
			((colour & 0xfc00U) >> 5) |
			((colour & 0xf8U) >> 3);

	end_x = start_x + width - 1;
	end_y = start_y + height - 1;

	for (x = start_x; x <= end_x; x++)
	{
		for (y = start_y; y <= end_y; y++)
		{
			address = (uint16_t*)gr2_base_address;
			address += x;
			address += y * (uint16_t)LCD_DISPLAY_WIDTH_PIXELS;
			*address = (uint16_t)converted_colour;
		}
	}
}

void lcd_scroll_display_up_1(int16_t lines)
{
	int16_t y;
	uint8_t *gr1_start_address = (uint8_t *)gr1_base_address;

	if (lines > LCD_DISPLAY_HEIGHT_PIXELS || lines < 1)
	{
		return;
	}

	for (y = 0; y < LCD_DISPLAY_HEIGHT_PIXELS - lines; y++)
	{
		memcpy(gr1_start_address + y * 64, gr1_start_address + (y + lines) * 64, 64);
	}

	lcd_filled_rectangle_1(0, LCD_DISPLAY_HEIGHT_PIXELS - lines, LCD_DISPLAY_WIDTH_PIXELS, lines, false);
}

void lcd_character_1(int16_t x, int16_t y, char c)
{
	int16_t char_x;
	int16_t char_y;
	uint8_t mask;
	int16_t byte_pos;

	lcd_filled_rectangle_1(x,
			y,
			6,
			10,
			false);

	byte_pos = ((int16_t)c - (int16_t)' ') * 9;

	for (char_y = 0; char_y < 9; char_y++)
	{
		mask = 0x80;
		for (char_x = 0; char_x < 5; char_x++)
		{
			if ((Font9_Table[byte_pos] & mask) == mask)
			{
				lcd_pixel_1(x + char_x, y + char_y, true);
			}

			mask >>= 1U;
		}

		byte_pos++;
	}
}

void lcd_string_1(int16_t x, int16_t y, const char *s)
{
	size_t c;
	size_t length;

	length = strlen(s);
	for (c = (size_t)0; c < length; c++)
	{
		lcd_character_1(x + (int16_t)c * 6, y, s[c]);
	}
}
