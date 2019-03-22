#include <stdint.h>
#include <stdbool.h>
#include <iodefine.h>

#define BLACK 	0x000000
#define WHITE 	0xffffff
#define RED 	0xff0000

void system_clock_config(void);
void touch_init(void);
bool touch_get_point(uint16_t* x, uint16_t* y);
void lcd_init(void);
void lcd_filled_rectangle(int16_t start_x,
		int16_t start_y,
		int16_t width,
		int16_t height,
		uint32_t colour);
void lcd_string(int16_t x, int16_t y, const char *s, uint32_t colour);
char *itoa(int value, char *str, int base);

int main(void)
{
	bool touched;
	uint16_t x, y;
	char text[10];

	system_clock_config();
	touch_init();
	lcd_init();

	lcd_filled_rectangle(0, 0, 480, 272, RED);

    while (true)
    {
		touched = touch_get_point(&x, &y);
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

/**
 * Configure the system and peripheral clocks
 */
void system_clock_config(void)
{
	/* enable all protect register bits */
	SYSTEM.PRCR.WORD = 0xa50bU;

	/* forcibly oscillate the main clock */
	SYSTEM.MOFCR.BIT.MOFXIN = 0U;

	/* select resonator as main clock source */
	SYSTEM.MOFCR.BIT.MOSEL = 0U;

	/* stop the HOCO, not using it */
	SYSTEM.HOCOCR.BIT.HCSTP = 1U;

	/* disable the sub-clock as no crystal input on hw */
	RTC.RCR3.BIT.RTCEN = 0U;

	/* not using USB clock */
	SYSTEM.SCKCR2.WORD = 0x0001U;

	/* set drive level for 12MHz input */
	SYSTEM.MOFCR.BIT.MODRV2 = 2U;

	/* set wait time for starting main clock oscillator */
	SYSTEM.MOSCWTCR.BYTE = 0x53U;

	/* start main clock oscillator */
	SYSTEM.MOSCCR.BIT.MOSTP = 0U;

	/* set rom wait state appropriate to main clock speed */
	SYSTEM.ROMWT.BIT.ROMWT = 2U;

	/* set PLL division to 1 */
	SYSTEM.PLLCR.BIT.PLIDIV = 0U;

	/* set PLL multiplication to 20 */
	SYSTEM.PLLCR.BIT.STC = 39U;

	/* start the PLL */
	SYSTEM.PLLCR2.BIT.PLLEN = 0U;

	/* wait for clock stabilization */
	while (0 == SYSTEM.OSCOVFSR.BIT.PLOVF)
	{
		__asm("NOP");
	}

	/* ICLK to 120MHz */
	SYSTEM.SCKCR.BIT.ICK = 1U;

	/* FCLK to 60 MHz */
	SYSTEM.SCKCR.BIT.FCK = 2U;

	/* PCLKA to 120MHz */
	SYSTEM.SCKCR.BIT.PCKA = 1U;

	/* PCLKB to 60MHz */
	SYSTEM.SCKCR.BIT.PCKB = 2U;

	/* PCLKC to 60MHz */
	SYSTEM.SCKCR.BIT.PCKC = 2U;

	/* PCLKD to 60MHz */
	SYSTEM.SCKCR.BIT.PCKD = 2U;

	/* SDCLK not used */
	SYSTEM.SCKCR.BIT.PSTOP0 = 1U;

	/* BCLK not used */
	SYSTEM.SCKCR.BIT.PSTOP1 = 1U;

	/* switch main clock source to PLL */
	SYSTEM.SCKCR3.BIT.CKSEL = 4U;

	/* stop LOCO as finished with it now */
	SYSTEM.LOCOCR.BYTE = 1U;

	/* disable all protect register bits */
	SYSTEM.PRCR.WORD = 0xa500U;
}
