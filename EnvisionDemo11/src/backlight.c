#include <stdint.h>
#include <stdbool.h>
#include "iodefine.h"
#include "backlight.h"

void backlight_init(void)
{
	/* enable writing to power control bit */
	SYSTEM.PRCR.WORD = 0xa502U;

	/* release the MTU module from stop */
	MSTP(MTU) = 0U;

	/* disable writing to power control bit */
	SYSTEM.PRCR.WORD = 0xa500;

	/* set up pins in pin controller */

	/* enable writing to MPC */
    MPC.PWPR.BIT.B0WI = 0;
    MPC.PWPR.BIT.PFSWE = 1;

    /* P6 not used as interrupt input */
    MPC.P66PFS.BYTE &= 0xbfU;

    /* set MTIOC7D pin to P66 */
    MPC.P66PFS.BIT.PSEL = 8U;

    /* set P66 to peripheral i/o */
    PORT6.PMR.BIT.B6 = 1U;

    /* disable writing MPC */
    MPC.PWPR.BIT.PFSWE = 0U;
    MPC.PWPR.BIT.B0WI = 1U;

    /* enable writing to MTU6/7 */
    MTU.TRWERB.BIT.RWE = 1U;

    /* stop counters 6 and 7 */
    MTU.TSTRB.BIT.CST6 = 0U;
    MTU.TSTRB.BIT.CST7 = 0U;

    /* clock select to internal clock with divider of 16 */
    MTU6.TCR.BIT.TPSC = 2U;
    MTU6.TCR2.BIT.TPSC2 = 0U;

    /* count at rising edge of clock */
    MTU6.TCR.BIT.CKEG = 0U;

    /* clear counter on general register 6A match */
    MTU6.TCR.BIT.CCLR = 1U;

    /* general registers 6B and 7A match values not used (these are buffered by general registers 6D and 7C, so clear those) */
    MTU6.TGRD = 0U;
    MTU7.TGRC = 0U;

    /* specify reset synchronized pwm mode */
    MTU6.TMDR1.BIT.MD = 0x8U;

    /* output 7D enabled, this is the output pin connected to backlight enable */
    MTU.TOERB.BIT.OE7D = 1U;

    /* set buffering on for this PWM mode */
    MTU6.TMDR1.BIT.BFB = 1U;

    /* 6A, 6C, 7A and 7C pins not used so disable all outputs */
    MTU6.TIORH.BYTE = 0U;
    MTU6.TIORL.BYTE = 0U;
    MTU7.TIORH.BYTE = 0U;
    MTU7.TIORL.BYTE = 0U;

    /* disable toggle output */
    MTU.TOCR1B.BIT.PSYE = 0U;

    /* select output negative level select at start of cycle - output negative is connected to backlight enable */
    MTU.TOCR1B.BIT.OLSN = 0U;

    /* set value counter must reach for clear in general register 6A */
    MTU6.TGRA = 32701U;

    /* initialize counts of 6 and 7 to 0 */
    MTU6.TCNT = 0U;
    MTU7.TCNT = 0U;
}

void backlight_set(uint8_t duty_percent)
{
	/* limit parameter */
	if (duty_percent > 100U)
	{
		duty_percent = 100U;
	}

    /* set duty match value in general register 7D which is a buffer for general register 7A */
    MTU7.TGRD = duty_percent * 327;

    /* start counting of timers 6 and 7 */
    MTU.TSTRB.BIT.CST6 = 1U;
}
