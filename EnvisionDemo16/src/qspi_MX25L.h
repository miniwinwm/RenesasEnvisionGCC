#ifndef QSPI_MX25L_H_
#define QSPI_MX25L_H_

#include <stdint.h>
#include <stdbool.h>
#include "qspi.h"

/**
 * Write an identification request to the MX25L and check that the chip is as expected
 *
 * @return QSPI_OK if identified correctly else QSPI_ERROR
 */
qspi_status_t qspi_MX25L_check_chip_id(void);

/**
 * Send a write enable command to the MX25L
 *
 * @return QSPI_OK if write command accepted else QSPI_ERROR
 */

qspi_status_t qspi_MX25L_write_enable(void);

/**
 * Query MX25L chip to find out if a write is in progress
 *
 * @param result True if write in progress
 * @return QSPI_OK if query command accepted else QSPI_ERROR
 */
qspi_status_t qspi_MX25L_get_write_in_progress(bool *result);

/**
 * Get the status byte from the MX25L
 *
 * @param mx25l_status The status byte from the chip
 * @return QSPI_OK if query command accepted else QSPI_ERROR
 */
qspi_status_t qspi_MX25L_get_status(uint8_t *mx25l_status);

/**
 * Erase a 4k sector on MX25L
 *
 * @param address Address in the sector to be erased
 * @return QSPI_OK if erase command accepted else QSPI_ERROR
 */
qspi_status_t qspi_MX25L_sector_erase(uint32_t address);

/**
 * Write up to 256 bytes of data to MX25L
 *
 * @param address Start address to write to
 * @param count Number of bytes to write
 * @param data Pointer to data to write
 * @return QSPI_OK if write command accepted else QSPI_ERROR
 * @note address + count must not cross a 256 byte page boundary
 */
qspi_status_t qspi_MX25L_page_write(uint32_t address, uint16_t count, uint8_t *data);

/**
 * Read data from the MX25L
 *
 * @param address Address to start reading from
 * @param count Number of bytes to read
 * @param data Pointer to buffer to hold read data
 * @return QSPI_OK if read command accepted else QSPI_ERROR
 */
qspi_status_t qspi_MX25L_read(uint32_t address, uint16_t count, uint8_t *data);

#endif /* QSPI_MX25L_H_ */
