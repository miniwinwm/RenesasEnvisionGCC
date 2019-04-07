#include <stdint.h>
#include <stdbool.h>
#include "iodefine.h"
#include "lcd_driver.h"

void system_clock_config(void);

int main(void)
{
	int16_t next_row = 0;
	bool button_pressed;

	/* init system clock */
	system_clock_config();

	/* init display */
	lcd_init();
	lcd_filled_rectangle(0, 0, LCD_DISPLAY_WIDTH_PIXELS, LCD_DISPLAY_HEIGHT_PIXELS, BLACK);

	/* set up push button gpio input */
	PORT0.PMR.BIT.B5 = 0U;	/* mode to gpio */
	PORT0.PDR.BIT.B5 = 0U;	/* input */
	PORT0.PCR.BIT.B5 = 0U;  /* pull-up disable */

	/* look at reset reason */
	if (SYSTEM.RSTSR2.BIT.IWDTRF == 1U)
	{
		lcd_string(0, next_row * ROW_HEIGHT, "Watchdog reset", RED);
	}
	else
	{
		lcd_string(0, next_row * ROW_HEIGHT, "Power-on reset", RED);
	}
	next_row++;

	/* endless loop watchdog cycle */
    while (true)
    {
		lcd_string(0, next_row * ROW_HEIGHT, "Watchdog cycle started. Too early to press button to kick watchdog", WHITE);
		next_row++;

		/* look at watchdog counter to see where we are in the cycle, counter counts down from 4096 to 0
		 *
		 * 		4096 -> 3072	too early, kicking watchdog will cause reset
		 * 		3071 -> 1024	watchdog window, needs to be kicked in this region
		 * 		1023 -> 0		too late, kicking watchdog here won't stop a reset
		 */
		while (IWDT.IWDTSR.BIT.CNTVAL > 3072U)
		{
			/* check if button pressed */
			if (PORT0.PIDR.BIT.B5 == 0U)
			{
				/* button pressed so kick watchdog */
	    		IWDT.IWDTRR = 0U;
	    		IWDT.IWDTRR = 0xffU;

	    		/* watchdog kicked too early, this will cause a reset so following code should never be seen */
	    		lcd_string(0, next_row * ROW_HEIGHT, "This should never be seen", WHITE);
	    		next_row++;
			}
		}

		/* now in the kick allowed window until counter reaches 1024 */
		lcd_string(0, next_row * ROW_HEIGHT, "Press button to kick watchdog", WHITE);
		next_row++;

		/* if button pressed in this region kick watchdog */
		button_pressed = false;
		while (IWDT.IWDTSR.BIT.CNTVAL >= 1024U && button_pressed == false)
		{
			/* check if button pressed */
			if (PORT0.PIDR.BIT.B5 == 0U)
			{
				/* wait until button is released */
				while (PORT0.PIDR.BIT.B5 == 0U)
				{
				}
				button_pressed = true;

				/* button pressed so kick watchdog */
	    		IWDT.IWDTRR = 0U;
	    		IWDT.IWDTRR = 0xffU;
			}
		}

		if (button_pressed)
		{
			continue;
		}

		/* kick window closed, going to reset regardless */
		lcd_string(0, next_row * ROW_HEIGHT, "Too late to kick watchdog, going to reset", WHITE);
		next_row++;

		while (IWDT.IWDTSR.BIT.CNTVAL < 1024U)
		{
			/* can kick the watchdog, but it won't help, going to die anyway... */

			/* check if button pressed */
			if (PORT0.PIDR.BIT.B5 == 0U)
			{
				/* button pressed so kick watchdog */
	    		IWDT.IWDTRR = 0U;
	    		IWDT.IWDTRR = 0xffU;
			}
		}

		/* watchdog not kicked or kicked too late, this will cause a reset so following code should never be seen */
		lcd_string(0, next_row * ROW_HEIGHT, "This should also never be seen", WHITE);
		next_row++;
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
