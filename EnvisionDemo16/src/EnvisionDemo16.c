#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "iodefine.h"
#include "qspi_MX25L.h"

void system_clock_config(void);
bool qspi_test(void);

__attribute__ ((aligned (4))) uint8_t test_data_write[256];
__attribute__ ((aligned (4))) uint8_t test_data_read[256];

int main(void)
{
	qspi_status_t status;

	/* init system clock */
	system_clock_config();

	qspi_open();

	(void)qspi_test();

	qspi_close();

	while (true)
	{
	}
}

bool qspi_test(void)
{
	bool write_in_progress;
	uint16_t c;

	/* check chip id */
	if (qspi_MX25L_check_chip_id() == QSPI_ERROR)
	{
		return false;
	}

	/* erase a sector */
	if (qspi_MX25L_sector_erase(0x1000) == QSPI_ERROR)
	{
		return false;
	}

	/* wait for erase to finish */
	do
	{
		qspi_MX25L_get_write_in_progress(&write_in_progress);
	}
	while (write_in_progress);

	/* erase a sector */
	if (qspi_MX25L_sector_erase(0x2000) == QSPI_ERROR)
	{
		return false;
	}

	/* wait for erase to finish */
	do
	{
		qspi_MX25L_get_write_in_progress(&write_in_progress);
	}
	while (write_in_progress);

	/* erase a sector */
	if (qspi_MX25L_sector_erase(0x3000) == QSPI_ERROR)
	{
		return false;
	}

	/* wait for erase to finish */
	do
	{
		qspi_MX25L_get_write_in_progress(&write_in_progress);
	}
	while (write_in_progress);

	/* create test data */
	for (c = 0U; c < 256U; c++)
	{
		test_data_write[c] = c;
	}

	/* write a page */
	if (qspi_MX25L_page_write(0x1000, 256, test_data_write) == QSPI_ERROR)
	{
		return false;
	}

	/* wait for write to finish */
	do
	{
		qspi_MX25L_get_write_in_progress(&write_in_progress);
	}
	while (write_in_progress);

	/* read page back */
	if (qspi_MX25L_read(0x1000, 256, test_data_read) == QSPI_ERROR)
	{
		return false;
	}

	/* compare written and read back */
	if (memcmp(test_data_read, test_data_write, 256) != 0)
	{
		return false;
	}

	memset(test_data_read, 0, 256);

	/* write a page */
	if (qspi_MX25L_page_write(0x2000, 250, test_data_write) == QSPI_ERROR)
	{
		return false;
	}

	/* wait for write to finish */
	do
	{
		qspi_MX25L_get_write_in_progress(&write_in_progress);
	}
	while (write_in_progress);

	/* read page back */
	if (qspi_MX25L_read(0x2000, 250, test_data_read) == QSPI_ERROR)
	{
		return false;
	}

	/* compare written and read back */
	if (memcmp(test_data_read, test_data_write, 250) != 0)
	{
		return false;
	}

	memset(test_data_read, 0, 256);

	/* write a page */
	if (qspi_MX25L_page_write(0x3000, 10, test_data_write) == QSPI_ERROR)
	{
		return false;
	}

	/* wait for write to finish */
	do
	{
		qspi_MX25L_get_write_in_progress(&write_in_progress);
	}
	while (write_in_progress);

	/* read page back */
	if (qspi_MX25L_read(0x3000, 10, test_data_read) == QSPI_ERROR)
	{
		return false;
	}

	/* compare written and read back */
	if (memcmp(test_data_read, test_data_write, 10) != 0)
	{
		return false;
	}

	return true;
}

/**
 * Configure the system and peripheral clocks
 */
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
