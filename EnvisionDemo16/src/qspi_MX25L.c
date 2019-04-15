#include <stdlib.h>
#include "qspi_MX25L.h"

qspi_status_t qspi_MX25L_read(uint32_t address, uint16_t count, uint8_t *data)
{
	uint8_t command[4];
	bool write_in_progress;

	/* check that there's not a write in progress */
	if (qspi_MX25L_get_write_in_progress(&write_in_progress) == QSPI_ERROR)
	{
		return QSPI_ERROR;
	}

	if (write_in_progress)
	{
		return QSPI_ERROR;
	}

	command[0] = 0x03U;
	command[1] = (uint8_t)((address & 0x00ff0000) >> 16);
	command[2] = (uint8_t)((address & 0x0000ff00) >> 8);
	command[3] = (uint8_t)(address & 0x000000ff);

	if (qspi_command(command, 4U, QSPI_COMMAND_AND_READ, data, count) != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

qspi_status_t qspi_MX25L_page_write(uint32_t address, uint16_t count, uint8_t *data)
{
	uint8_t command[4];

	if (count == 0U || count > 256U)
	{
		return QSPI_ERROR;
	}

	/* all data to be written must be within same 256 byte page */
	if (address / 256U != (address + count - 1U) / 256U)
	{
		return QSPI_ERROR;
	}

	if (qspi_MX25L_write_enable() == QSPI_ERROR)
	{
		return QSPI_ERROR;
	}

	command[0] = 0x02U;
	command[1] = (uint8_t)((address & 0x00ff0000) >> 16);
	command[2] = (uint8_t)((address & 0x0000ff00) >> 8);
	command[3] = (uint8_t)(address & 0x000000ff);

	if (qspi_command(command, 4U, QSPI_COMMAND_AND_WRITE, data, count) != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

qspi_status_t qspi_MX25L_get_status(uint8_t *mx25l_status)
{
	uint8_t command;

	/* send get status command */
	command = 0x05U;

	if (qspi_command(&command, 1U, QSPI_COMMAND_AND_READ, mx25l_status, 1U) != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

qspi_status_t qspi_MX25L_check_chip_id(void)
{
	uint8_t command;
	uint8_t response[3];

	/* send get chip id command */
	command = 0x9fU;

	if (qspi_command(&command, 1U, QSPI_COMMAND_AND_READ, response, 3U) != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	/* check chip id */
	if (memcmp(response, "\xc2\x20\x16", 3U) != 0)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

qspi_status_t qspi_MX25L_write_enable(void)
{
	uint8_t command;
	bool write_in_progress;
	uint8_t chip_status;

	/* check write in progress */
	if (qspi_MX25L_get_write_in_progress(&write_in_progress) == QSPI_ERROR)
	{
		return QSPI_ERROR;
	}

	if (write_in_progress == true)
	{
		return QSPI_ERROR;
	}

	/* send write enable command */
	command = 0x06U;

	if (qspi_command(&command, 1U, QSPI_COMMAND_ONLY, NULL, 0U) != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	/* check write enable set */
	if (qspi_MX25L_get_status(&chip_status) == QSPI_ERROR)
	{
		return QSPI_ERROR;
	}

	/* check write enable in bit 1 */
	if (chip_status & 0x02U == 0U)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

qspi_status_t qspi_MX25L_get_write_in_progress(bool *result)
{
	uint8_t chip_status;

	if (qspi_MX25L_get_status(&chip_status) == QSPI_ERROR)
	{
		return QSPI_ERROR;
	}

	if (chip_status & 0x01U)
	{
		*result = true;
	}
	else
	{
		*result = false;
	}

	return QSPI_OK;
}

qspi_status_t qspi_MX25L_sector_erase(uint32_t address)
{
	uint8_t command[4];

	if (qspi_MX25L_write_enable() == QSPI_ERROR)
	{
		return QSPI_ERROR;
	}

	command[0] = 0x20U;
	command[1] = (uint8_t)((address & 0x00ff0000) >> 16);
	command[2] = (uint8_t)((address & 0x0000ff00) >> 8);
	command[3] = (uint8_t)(address & 0x000000ff);

	if (qspi_command(command, 4U, QSPI_COMMAND_ONLY, NULL, 0U) != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}
