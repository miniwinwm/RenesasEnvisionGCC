#ifndef QSPI_H_
#define QSPI_H_

typedef enum
{
	QSPI_OK,
	QSPI_ERROR
} qspi_status_t;

typedef enum
{
	QSPI_COMMAND_AND_READ,
	QSPI_COMMAND_AND_WRITE,
	QSPI_COMMAND_ONLY
} qspi_operation_t;

/**
 * Open the QSPI connection
 */
void qspi_open(void);

/**
 * Close and clean up the QSPI connection
 */
void qspi_close(void);

/**
 * Send a command and subsequently optionally send or receive data via QSPI
 *
 * @param command Pointer to buffer containing command data. Must not be NULL.
 * @param command_length Length of data in bytes in command buffer.
 * @param operation Command only, command plus read or command plus write
 * @param data Pointer to buffer containing data to read or write after command write. Must be NULL if sending command only. Must be non-NULL if reading or writing data.
 * @param data_length Length of data to read or write after sending command. Must be 0 is sending command only. Must be non-zero if reading or writing data.
 * @return
 */
qspi_status_t qspi_command(uint8_t *command,
		uint32_t command_length,
		qspi_operation_t operation,
		uint8_t *data,
		uint32_t data_length);

#endif /* QSPI_H_ */
