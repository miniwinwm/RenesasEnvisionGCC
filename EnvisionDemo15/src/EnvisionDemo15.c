#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "iodefine.h"
#include "lcd_driver.h"

void system_clock_config(void);
void delay_us(uint32_t us);

void main(void)
{
	int16_t next_row = 0;
	bool button_pressed;
	char text[30];
	uint8_t dummy;

	/* init system clock */
	system_clock_config();

	/* set up led gpio output */
	PORT7.PMR.BIT.B0 = 0U;	/* mode to gpio */
	PORT7.ODR0.BIT.B0 = 0U;	/* cmos output type */
	PORT7.PDR.BIT.B0 = 1U;	/* output */
	PORT7.PODR.BIT.B0 = 1U;	/* switch off */

	/* set up push button gpio input */
	PORT0.PMR.BIT.B5 = 0U;	/* mode to gpio */
	PORT0.PDR.BIT.B5 = 0U;	/* input */
	PORT0.PCR.BIT.B5 = 0U;  /* pull-up disable */

	/* increment reset cycle counter */
	SYSTEM.DPSBKR[0]++;

	/* look at reset reason */
	if (SYSTEM.RSTSR0.BIT.DPSRSTF == 1U)
	{
		/* clear deep standby reset wake flag */
		SYSTEM.RSTSR0.BIT.DPSRSTF = 0U;

	    /* enable writing to power control registers */
        SYSTEM.PRCR.WORD = 0xa502U;

		/* clear standby flag */
		SYSTEM.SBYCR.BIT.SSBY = 0U;

		/* clear deep standby flag */
		SYSTEM.DPSBYCR.BIT.DPSBY = 0U;

		/* release ports from retained state */
		SYSTEM.DPSBYCR.BIT.IOKEEP = 0U;

	    /* disable writing to power control registers */
        SYSTEM.PRCR.WORD = 0xa500U;

		/* set user led off */
		PORT7.PODR.BIT.B0 = 1U;

		strcpy(text, "Deep standby wake-up");
	}
	else if (SYSTEM.RSTSR0.BIT.PORF == 1U)
	{
		strcpy(text, "Power on reset");

		/* initialize back up register */
		SYSTEM.DPSBKR[0] = 0U;
	}
	else if (SYSTEM.RSTSR1.BIT.CWSF == 1U)
	{
		strcpy(text, "User reset or debugger restart");
	}
	else
	{
		strcpy(text, "Debugger start");

		/* initialize back up register */
		SYSTEM.DPSBKR[0] = 0U;
	}

	/* save warm start state so can determine reset reason next time */
	SYSTEM.RSTSR1.BIT.CWSF = 1U;

	/* init display */
	lcd_init();
	lcd_filled_rectangle(0, 0, LCD_DISPLAY_WIDTH_PIXELS, LCD_DISPLAY_HEIGHT_PIXELS, BLACK);

	/* display reset reason determined above */
	lcd_string(0, next_row * ROW_HEIGHT, text, RED);
	next_row++;

	/* display reset cycle count */
	lcd_string(0, next_row *ROW_HEIGHT, "Reset cycle count:", WHITE);
	itoa(SYSTEM.DPSBKR[0], text, 10);
	lcd_string(150, next_row *ROW_HEIGHT, text, WHITE);
	next_row++;

	/* wait for user button pressed */
	lcd_string(0, next_row * ROW_HEIGHT, "Press button to go into standby mode", WHITE);
	next_row++;
	while (PORT0.PIDR.BIT.B5 == 1U)
	{
	}

	lcd_string(0, next_row * ROW_HEIGHT, "Going to sleep in 2s...", WHITE);
	next_row++;

	delay_us(2E6);

	/* set user led on */
	PORT7.PODR.BIT.B0 = 0U;

	/* set lcd backlight off */
	PORT6.PODR.BIT.B6 = 0U;

	/* disable IRQ8, normal interrupt from IRQ8 not needed */
    IEN(ICU, IRQ8) = 0U;

    /* disable filtering on IRQ8 */
    ICU.IRQFLTE1.BIT.FLTEN8 = 0U;

    /* enable writing to MPC pin function control registers */
    MPC.PWPR.BIT.B0WI = 0U;
    MPC.PWPR.BIT.PFSWE = 1U;

    /* set P40 to IRQ8-DS */
    MPC.P40PFS.BIT.ISEL = 1U;

    /* disable writing to MPC pin function control registers */
    MPC.PWPR.BIT.PFSWE = 0U;
    MPC.PWPR.BIT.B0WI = 1U;

    /* set IRQ8 interrupt priority level highest */
    IPR(ICU, IRQ8) = 15U;

    /* enable writing to power control registers */
	SYSTEM.PRCR.WORD = 0xa503U;

	/* shift to standby after WAIT instruction */
	SYSTEM.SBYCR.BIT.SSBY = 1U;

	/* then shift to deep standby mode */
	SYSTEM.DPSBYCR.BIT.DPSBY = 1U;

	/* set address retained */
	SYSTEM.SBYCR.BIT.OPE = 1U;

	/* retain port state after wake until released */
	SYSTEM.DPSBYCR.BIT.IOKEEP = 1U;

	/* wake from deep standby on falling edge of IRQ8 */
	SYSTEM.DPSIEGR1.BIT.DIRQ8EG = 0U;

	/* release from deep software standby on IRQ8 then do a dummy read */
	SYSTEM.DPSIER1.BIT.DIRQ8E = 1U;
	dummy = SYSTEM.DPSIER0.BYTE;

	/* clear request for release flag for IRQ8, reading it first */
    dummy = SYSTEM.DPSIFR1.BYTE;
	SYSTEM.DPSIFR1.BIT.DIRQ8F = 0U;

    /* enable writing to power control registers */
    SYSTEM.PRCR.WORD = 0xa500U;

    /* wait for this write to reflect */
    while ((SYSTEM.PRCR.WORD & 0x000fU) != 0U)
    {
    	__asm("NOP");
    }

    /* disable software interrupt 1 */
    IEN(ICU, SWINT) = 0U;

    /* set software interrupt 1 priority */
    IPR(ICU, SWINT) = 10U;

    /* clear software interrupt 1 */
    IR(ICU, SWINT) = 0U;

    /* enable software interrupt 1 */
    IEN(ICU, SWINT) = 1U;

    /* create a software interrupt 1 so that the wait instruction is executed in supervisor mode
     * when called in this interrupt's handler */
    __builtin_rx_int(27U);

	lcd_string(0, next_row * ROW_HEIGHT, "Should never get here!", RED);

    while (true)
    {
    }
}

void deep_standby_mode(void)
{
    volatile uint8_t dummy;

    if (0 == SYSTEM.RSTSR0.BIT.DPSRSTF)
    {
    	// go into standby

    	// led off
        PORT7.PODR.BIT.B0 = 1;

        SYSTEM.PRCR.WORD = 0xA503;

        SYSTEM.SBYCR.BIT.SSBY    = 1;
        SYSTEM.DPSBYCR.BIT.DPSBY = 1;

        SYSTEM.SBYCR.BIT.OPE      = 1;

        SYSTEM.DPSBYCR.BIT.IOKEEP = 1;

        SYSTEM.DPSIEGR1.BIT.DIRQ8EG = 0;

        SYSTEM.DPSIER1.BIT.DIRQ8E = 1;
        dummy = SYSTEM.DPSIER0.BYTE;

        dummy = SYSTEM.DPSIFR1.BYTE;
        SYSTEM.DPSIFR1.BIT.DIRQ8F = 0;

        SYSTEM.PRCR.WORD = 0xA500;
        while (0x0000 != (SYSTEM.PRCR.WORD & 0x000F))
        {
        }

        IEN(ICU, SWINT) = 0U;
        IPR(ICU, SWINT) = 10U;
        IR(ICU, SWINT) = 0U;
        IEN(ICU, SWINT) = 1U;


        /* create a software interrupt */
        __builtin_rx_int (27);


    }
    else
    {
    	// come out of standby

    	// led on
 //       PORT7.PODR.BIT.B0 = 0;

 //       SYSTEM.PRCR.WORD = 0xA502;

   //     SYSTEM.DPSBYCR.BIT.IOKEEP = 0;

     //   SYSTEM.PRCR.WORD = 0xA500;
    }
}

void system_clock_config(void)
{
	/* enable all protect register bits */
	SYSTEM.PRCR.WORD = 0xa50bU;

	/* select resonator as main clock source */
	SYSTEM.MOFCR.BIT.MOSEL = 0U;

	/* stop the HOCO, not using it */
	SYSTEM.HOCOCR.BIT.HCSTP = 1U;

	/* turn off power to HOCO */
	SYSTEM.HOCOPCR.BIT.HOCOPCNT = 1U;

    /* disable the sub-clock oscillator as no crystal fitted */
    SYSTEM.SOSCCR.BIT.SOSTP = 1U;

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

void __attribute__((optimize("O3"))) delay_us(uint32_t us)
{
	uint32_t i;
	uint32_t d = us * 60U;

	for (i = 0U; i < d; i++)
	{
		__asm("NOP");
	}
}
