#include <stdint.h>
#include <stdbool.h>
#include <iodefine.h>
#include <stdlib.h>

#define BLACK 0x000000
#define WHITE 0xffffff
#define RED 0xFF0000

void system_clock_config(void);
bool mw_hal_touch_get_point(uint16_t* x, uint16_t* y);
void mw_hal_lcd_init(void);
void lcd_filled_rectangle(int16_t start_x,
		int16_t start_y,
		int16_t width,
		int16_t height,
		uint32_t colour);
void lcd_string(int16_t x, int16_t y, const char *s, uint32_t colour);

int main(void)
{
	bool touched;
	uint16_t x, y;
	char text[10];

	system_clock_config();
	mw_hal_touch_init();
	mw_hal_lcd_init();

	lcd_filled_rectangle(0, 0, 480, 272, RED);

    while (true)
    {
		touched = mw_hal_touch_get_point(&x, &y);
		if (touched)
		{
			if (x < 450 && y < 240)
			{
				lcd_filled_rectangle(x, y, 30, 32, BLACK);
				itoa(x, text, 10);
				lcd_string(x, y, text, WHITE);
				itoa(y, text, 10);
				lcd_string(x, y + 16, text, WHITE);
			}
		}
    }

    return 0;
}


void system_clock_config(void)
{
	/* enable all protect register bits */
	SYSTEM.PRCR.WORD = 0xA50B;

	SYSTEM.MOFCR.BIT.MOFXIN = 0;
	SYSTEM.MOFCR.BIT.MOSEL = 0;
	if (1 == SYSTEM.HOCOCR.BIT.HCSTP)
	{
		SYSTEM.HOCOPCR.BYTE = 0x01;
	}
	else
	{
		while(0 == SYSTEM.OSCOVFSR.BIT.HCOVF)
		{
			asm("NOP");
		}
	}

	SYSTEM.MOFCR.BIT.MODRV2 = 2;
	SYSTEM.MOSCWTCR.BYTE = 0x53;
	SYSTEM.MOSCCR.BYTE = 0x00;
	if (0x00 ==  SYSTEM.MOSCCR.BYTE)
	{
		asm("NOP");
	}

	while(0 == SYSTEM.OSCOVFSR.BIT.MOOVF)
	{
		asm("NOP");
	}

	/* disable the sub-clock as no crystal input on hw */
	RTC.RCR3.BIT.RTCEN = 0;

	SYSTEM.PLLCR.BIT.PLIDIV = 0;

	/* select mian clock oscillator */
	SYSTEM.PLLCR.BIT.PLLSRCSEL = 0;

	SYSTEM.PLLCR.BIT.STC = ((uint8_t)((float)20 * 2.0)) - 1;

	/* set pll in operation */
	SYSTEM.PLLCR2.BYTE = 0x00;

	/* wait for clock stabilization */
	while (0 == SYSTEM.OSCOVFSR.BIT.PLOVF)
	{
		asm("NOP");
	}
	SYSTEM.ROMWT.BYTE = 0x02;
	if (0x02 == SYSTEM.ROMWT.BYTE)
	{
		asm("NOP");
	}

	SYSTEM.SCKCR.LONG = 0x31c22333;
	SYSTEM.SCKCR2.WORD = 0x0011;
	SYSTEM.SCKCR3.WORD = 0x0400;

	/* low speed oscillator is stopped */
	SYSTEM.LOCOCR.BYTE = 0x01;

	/* disable all protect register bits */
	SYSTEM.PRCR.WORD = 0xA500;
}
