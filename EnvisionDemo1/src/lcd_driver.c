#include <stdlib.h>
#include <iodefine.h>
#include <stdint.h>
#include <string.h>

#define MW_GL_TITLE_FONT_HEIGHT		16
#define LCD_DISPLAY_WIDTH_PIXELS	480
#define LCD_DISPLAY_HEIGHT_PIXELS	272
extern const uint16_t font_positions[];
extern const uint8_t font_bitmap[];

void mw_hal_lcd_init(void)
{
	/* allow writing to PFSWE bit */
    MPC.PWPR.BIT.B0WI = 0;

    /* enable writing to PFS registers. */
    MPC.PWPR.BIT.PFSWE = 1;

    /* set LCD_CLK pin to peripheral i/o */
    MPC.PB5PFS.BYTE = 0x25U;
    PORTB.PMR.BIT.B5 = 1;

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

    /* enable writing to PFSWE */
    MPC.PWPR.BIT.B0WI = 0;

    /* disable writing to PFS registers. */
    MPC.PWPR.BIT.PFSWE = 0;

    /* disable writing of PFSWE bit. */
    MPC.PWPR.BIT.B0WI = 1;

    /* set up reset line and bring display out of reset */
	PORT6.PMR.BIT.B3 = 0;	/* mode to gpio */
	PORT6.PDR.BIT.B3 = 1; 	/* port direction output */
	PORT6.PODR.BIT.B3 = 1;  /* bring display out of reset */

	/* set backlight pin as gpio output and switch backlight on */
	PORT6.PMR.BIT.B6 = 0;	/* mode to gpio */
	PORT6.PDR.BIT.B6 = 1;	/* port direction output */
	PORT6.ODR0.BIT.B6 = 0;	/* cmos */
	PORT6.PODR.BIT.B6 = 1;	/* backlight on */

	/* enable writing to clock control bit */
	SYSTEM.PRCR.WORD = (uint16_t)(SYSTEM.PRCR.WORD | 0xa500 | 2);

	/* supply the peripheral clock to the glcd module */
	MSTP(GLCDC) = 0;

	/* disable writing to clock control bit */
	SYSTEM.PRCR.WORD = (uint16_t)((SYSTEM.PRCR.WORD | 0xa500) & ~2);

	/* release glcd module from a software reset status. */
	GLCDC.BGEN.BIT.SWRST = 1;

	/* set the dot clock to pll clock */
	GLCDC.PANELCLK.BIT.CLKSEL = 1;

	/* set clock division ratio */
	GLCDC.PANELCLK.BIT.DCDR = 0x18;

	/* selects pixel clock output to no division */
	GLCDC.PANELCLK.BIT.PIXSEL = 0;

	/* enable panel clock LCD_CLK output */
	GLCDC.PANELCLK.BIT.CLKEN = 1;

	/* waiting for supply of panel clock(LCD_CLK) and pixel clock(PXCLK)
	 * the BGMON.SWRST bit is sampled with PXCLK so if the CLKEN bit is set,
	 * the BGEN.SWRST bit is reflected on the BGMON.SWRST bit. */
	while (0 == GLCDC.BGMON.BIT.SWRST)
	{
		asm("NOP");
	}

	/* set the panel signal timing */

	/* the outputs of LCD_DATA are synchronized with rising edges of LCD_CLK */
	GLCDC.CLKPHASE.BIT.LCDEDG = 0;

	/* the output of LCD_TCON0 is synchronized with rising edges of LCD_CLK */
	GLCDC.CLKPHASE.BIT.TCON0EDG = 0;

	/* the output of LCD_TCON1 is synchronized with rising edges of LCD_CLK */
	GLCDC.CLKPHASE.BIT.TCON1EDG = 0;

	/* the output of LCD_TCON2 is synchronized with rising edges of LCD_CLK */
	GLCDC.CLKPHASE.BIT.TCON2EDG = 0;

	/* the output of LCD_TCON3 is synchronized with rising edges of LCD_CLK */
	GLCDC.CLKPHASE.BIT.TCON3EDG = 0;

	/* horiz synch timing offset is 1 pixel */
	GLCDC.TCONTIM.BIT.OFFSET = 0;

	/* vert synch timing offset is 1 pixel */
	GLCDC.TCONTIM.BIT.HALF = 0;

	/* output signal select STHA */
	GLCDC.TCONSTHA2.BIT.SEL = 2;

	/* invert the STHy signal */
	GLCDC.TCONSTHA2.BIT.INV = 1;/* Hsync(STHA) -> Invert or Not Invert */

	/* set Hsync beginning position to no delay */
	GLCDC.TCONSTHA1.BIT.HS = 0;

	/* select input Hsync as reference */
	GLCDC.TCONSTHA2.BIT.HSSEL = 0;

	/* STHy signal pulse width 1 pixel */
	GLCDC.TCONSTHA1.BIT.HW = 1;

	GLCDC.TCONSTVA2.BIT.SEL = 0; /* Vsync(STVA) -> TCON0 */
	GLCDC.TCONSTVA2.BIT.INV = (uint32_t)1; /* Vsync(STVA) -> Invert or Vsync(STVA) -> Not Invert */
	GLCDC.TCONSTVA1.BIT.VS = 0;      /* No delay. */
	GLCDC.TCONSTVA1.BIT.VW = 1;
	GLCDC.TCONSTHB2.BIT.SEL = 7;
	GLCDC.TCONDE.BIT.INV =0;
	GLCDC.TCONSTHB1.BIT.HS = 0x29;
	GLCDC.TCONSTHB1.BIT.HW = 0x1e0;
	GLCDC.TCONSTHB2.BIT.HSSEL = 0; /* Select input Hsync as reference */
	GLCDC.TCONSTVB1.BIT.VS = 9;
	GLCDC.TCONSTVB1.BIT.VW = 0x110;

	/* configure the background screen */
	GLCDC.BGPERI.BIT.FH = 0x20d;

	GLCDC.BGPERI.BIT.FV = 0x120;
	GLCDC.BGSYNC.BIT.HP = 3;
	GLCDC.BGSYNC.BIT.VP = 7;
	GLCDC.BGHSIZE.BIT.HP = 0x2c;
	GLCDC.BGVSIZE.BIT.VP = 0x10;
	GLCDC.BGHSIZE.BIT.HW = 0x1e0;
	GLCDC.BGVSIZE.BIT.VW = 0x110;
	GLCDC.BGCOLOR.BIT.R = 0;
	GLCDC.BGCOLOR.BIT.G = 0;
	GLCDC.BGCOLOR.BIT.B = 0;

	/* frame buffer 565 format */
	GLCDC.GR2FLM6.BIT.FORMAT = 0;

	/* set start address of gr2 buffer */
	GLCDC.GR2FLM2 = 0x800000;

	/* set gr2 background colour */
	GLCDC.GR2BASE.BIT.R = 0;
	GLCDC.GR2BASE.BIT.G = 0;
	GLCDC.GR2BASE.BIT.B = 0;


	GLCDC.GR2FLM5.BIT.DATANUM = 0xe;

	/* set number of lines in display */
	GLCDC.GR2FLM5.BIT.LNNUM = 0x10f;

	GLCDC.GR2FLM3.BIT.LNOFF = 0x3c0;

	GLCDC.GR2AB2.BIT.GRCVW = 0x110;
	GLCDC.GR2AB2.BIT.GRCVS = 9;
	GLCDC.GR2AB3.BIT.GRCHW = 0x1e0;
	GLCDC.GR2AB3.BIT.GRCHS = 0x29;
	GLCDC.GR2AB1.BIT.GRCDISPON = 0;
	GLCDC.GR2AB1.BIT.ARCON = 0;
	GLCDC.GR2AB1.BIT.DISPSEL = 2;

	/* disable chroma control */
	GLCDC.GR2AB7.BIT.CKON = 0;

	/* Configure the output control block */
	GLCDC.OUTSET.BIT.ENDIANON = 0;

	/* Byte order RGB */
	GLCDC.OUTSET.BIT.SWAPON = 0;

	/* pixel format 565 */
	GLCDC.OUTSET.BIT.FORMAT = 2;
	GLCDC.PANELDTHA.BIT.FORM = 2;

	/* no pixel clock division */
	GLCDC.OUTSET.BIT.FRQSEL = 0;

	/* rounding mode is truncate */
	GLCDC.PANELDTHA.BIT.SEL = 0;

	/* brightness setting correction all midpoint */
	GLCDC.BRIGHT1.BIT.BRTG = 512;
	GLCDC.BRIGHT2.BIT.BRTB = 512;
	GLCDC.BRIGHT2.BIT.BRTR = 512;

	/* contrast setting correction all midpoint */
	GLCDC.CONTRAST.BIT.CONTG = 128;
	GLCDC.CONTRAST.BIT.CONTB = 128;
	GLCDC.CONTRAST.BIT.CONTR = 128;

	/* disable gamma correction */
	GLCDC.GAMSW.BIT.GAMON = 0;

	/* Enable the GLCD detections and interrupts */

	/* disable VPOS line notification */
	GLCDC.DTCTEN.BIT.VPOSDTC = 0;

	/* disable gr1 underflow notification */
	GLCDC.DTCTEN.BIT.GR1UFDTC = 0;

	/* disable gr2 underflow notofication */
	GLCDC.DTCTEN.BIT.GR2UFDTC = 0;

	/* disable gr1 underflow interrupt */
	GLCDC.INTEN.BIT.GR1UFINTEN = 0;
	EN(GLCDC, GR1UF) = 0;
	while (0 != IS(GLCDC,GR1UF))
	{
		asm("NOP");
	}

	/* disable gr2 underflow interrupt */
	GLCDC.INTEN.BIT.GR2UFINTEN = 0;
	EN(GLCDC,GR2UF) = 0;
	while (0 != IS(GLCDC,GR2UF))
	{
		asm("NOP");
	}

	IEN(ICU, GROUPAL1) = 0;
	IPR(ICU, GROUPAL1) = 0;

	/* allow reading of the frame buffer */
	GLCDC.GR2FLMRD.BIT.RENB = 1;
    GLCDC.BGEN.LONG = 0x00010101;
}

void lcd_pixel(int16_t x, int16_t y, uint32_t colour)
{
	uint16_t* address = (uint16_t*)0x00800000;
	uint32_t converted_colour = ((colour & 0xf80000U) >> 8U) |
			((colour & 0xfc00U) >> 5U) |
			((colour & 0xf8U) >> 3U);

	address += x;
	address += y * LCD_DISPLAY_WIDTH_PIXELS;
	*address = (uint16_t)converted_colour;
}

void lcd_filled_rectangle(int16_t start_x,
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
	uint32_t converted_colour = ((colour & 0xf80000U) >> 8U) |
			((colour & 0xfc00U) >> 5U) |
			((colour & 0xf8U) >> 3U);

	end_x = start_x + width - 1;
	end_y = start_y + height - 1;

	for (x = start_x; x <= end_x; x++)
	{
		for (y = start_y; y <= end_y; y++)
		{
			address = (uint16_t*)0x00800000;
			address += x;
			address += y * 480;
			*address = (uint16_t)converted_colour;
		}
	}
}

void lcd_string(int16_t x, int16_t y, const char *s, uint32_t colour)
{
	size_t length = strlen(s);
	uint8_t mask;
	int16_t start_pos_in_bitmap;
	int16_t end_pos_in_bitmap;
	int16_t bitmap_x;
	int16_t bitmap_y;
	uint8_t i;
	uint8_t byte_from_bitmap;
	char c;
	int16_t next_char_start_position_along_string = 0;
	int16_t position_across_character;
	int16_t string_width_pixels;
	uint16_t temp_uint16;

	for (i = 0U; i < length; i++)
	{
		for (bitmap_y = 0; bitmap_y < MW_GL_TITLE_FONT_HEIGHT; bitmap_y++)
		{
			c = s[i];
			if (c < ' ' || c > '~')
			{
				c = '*';
			}
			c -= 32;

			start_pos_in_bitmap = (int16_t)font_positions[(uint8_t)c];
			end_pos_in_bitmap = (int16_t)font_positions[(uint8_t)c + 1U];

			mask = 0x80U;
			temp_uint16 = font_positions[(uint8_t)c] & 0x07U;
			/* the next line is MISRA compliant despite a warning because the shift operand cannot be > 7 */
			mask >>= (uint8_t)temp_uint16;

			position_across_character = 0;
			for (bitmap_x = start_pos_in_bitmap; bitmap_x < end_pos_in_bitmap; bitmap_x++)
			{
				byte_from_bitmap = font_bitmap[((uint16_t)bitmap_x >> 3U) + (uint16_t)bitmap_y * 87U];

				if ((byte_from_bitmap & mask) == 0U)
				{
					lcd_pixel(x + next_char_start_position_along_string + position_across_character,
							y + bitmap_y, colour);
				}

				mask >>= 1U;
				if (mask == 0U)
				{
					mask = 0x80U;
				}
				position_across_character++;
			}
		}
		next_char_start_position_along_string += position_across_character;
	}
}
