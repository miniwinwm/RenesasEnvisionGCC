#include <stdint.h>
#include <iodefine.h>
#include "rtc.h"

void delay_us(uint32_t us);
static int bcd_to_dec(uint8_t value);
static uint8_t dec_to_bcd(uint8_t value);

static int bcd_to_dec(uint8_t value)
{
    return (int)((((value & 0xf0U) >> 4) * 10U) + (value & 0x0fU));
}

static uint8_t dec_to_bcd(uint8_t value)
{
    return (((value / 10U) << 4) & 0xf0U) | (value % 10U);
}

void rtc_set_time(tm_t *new_time)
{
	/* stop clock */
	RTC.RCR2.BIT.START = 0U;
	while (RTC.RCR2.BIT.START != 0U)
	{
		__asm("NOP");
	}

	/* reset RTC */
	RTC.RCR2.BIT.RESET = 1U;
	while (RTC.RCR2.BIT.RESET != 0U)
	{
		__asm("NOP");
	}

	/* set 24 hour mode */
    RTC.RCR2.BIT.HR24 = 1U;

	RTC.RSECCNT.BYTE = dec_to_bcd((uint8_t)new_time->tm_sec);
	while(RTC.RSECCNT.BYTE != dec_to_bcd((uint8_t)new_time->tm_sec))
	{
		__asm("NOP");
	}

	RTC.RMINCNT.BYTE = dec_to_bcd((uint8_t)new_time->tm_min);
	while(RTC.RMINCNT.BYTE != dec_to_bcd((uint8_t)new_time->tm_min))
	{
		__asm("NOP");
	}

	RTC.RHRCNT.BYTE = dec_to_bcd((uint8_t)new_time->tm_hour);
	while(RTC.RHRCNT.BYTE != dec_to_bcd((uint8_t)new_time->tm_hour))
	{
		__asm("NOP");
	}

	RTC.RDAYCNT.BYTE = dec_to_bcd((uint8_t)new_time->tm_mday);
	while(RTC.RDAYCNT.BYTE != dec_to_bcd((uint8_t)new_time->tm_mday))
	{
		__asm("NOP");
	}

	RTC.RMONCNT.BYTE = dec_to_bcd((uint8_t)new_time->tm_mon + 1U);
	while(RTC.RMONCNT.BYTE != dec_to_bcd((uint8_t)new_time->tm_mon + 1U))
	{
		__asm("NOP");
	}

    RTC.RYRCNT.WORD = (uint16_t)(dec_to_bcd((uint8_t)((new_time->tm_year + 1900) % 100)));
	while(RTC.RYRCNT.WORD != (uint16_t)(dec_to_bcd((uint8_t)((new_time->tm_year + 1900) % 100))))
	{
		__asm("NOP");
	}

	/* start clock */
	RTC.RCR2.BIT.START = 1U;
	while (RTC.RCR2.BIT.START != 1U)
	{
		__asm("NOP");
	}
}

void rtc_get_time(tm_t *current_time)
{
    uint16_t bcd_years;

	/* disable carry interrupt in icu */
    IEN(PERIB, INTB176) = 0;

    /* enable carry interrupt in rtc */
	RTC.RCR1.BIT.CIE = 1U;

    do
    {
        /* clear carry flag in icu */
    	IR(PERIB, INTB176) = 0U;

        /* seconds. (0-59) */
        current_time->tm_sec  = bcd_to_dec(RTC.RSECCNT.BYTE & 0x7fU);

        /* minutes. (0-59) */
        current_time->tm_min  = bcd_to_dec(RTC.RMINCNT.BYTE & 0x7fU);

        /* hours. (0-23) */
        current_time->tm_hour = bcd_to_dec(RTC.RHRCNT.BYTE & 0x3fU);

        /* day of the month (1-31) */
        current_time->tm_mday = bcd_to_dec(RTC.RDAYCNT.BYTE);

        /* months since January (0-11) */
        current_time->tm_mon  = bcd_to_dec(RTC.RMONCNT.BYTE) - 1;

        /* dears since 2000 */
        bcd_years = RTC.RYRCNT.WORD;

        /* years since 1900 (100-199) */
        current_time->tm_year = bcd_to_dec(bcd_years & 0xffU) + 100;

        /* days since Sunday (0-6) */
        current_time->tm_wday = (int)(RTC.RWKCNT.BYTE & 0x07U);
    }
    while (IR(PERIB, INTB176) == 1U); /* re-read if carry occurs during read */
}

void rtc_init(void)
{
	/* select main clock as source */
	RTC.RCR4.BIT.RCKSEL = 1U;

	/* delay 6 clock cycles */
	delay_us(1);

	/* stop clock */
	RTC.RCR2.BIT.START = 0U;
	while (RTC.RCR2.BIT.START != 0U)
	{
		__asm("NOP");
	}

	/* set divider */
	RTC.RFRH.WORD = 1U;
	RTC.RFRL.WORD = 0xe847;

	/* select count mode to calendar */
	RTC.RCR2.BIT.CNTMD = 0U;

	/* reset RTC */
	RTC.RCR2.BIT.RESET = 1U;
	while (RTC.RCR2.BIT.RESET != 0U)
	{
		__asm("NOP");
	}

	/* set rtc carry interrupt to software configurable interrupt 176 */
    ICU.SLIBR176.BYTE = 49;

    /* disable all rtc interrupts */
    IEN(RTC, PRD) = 0;
    IEN(RTC, ALM) = 0;
    IEN(PERIB, INTB176) = 0;
}
