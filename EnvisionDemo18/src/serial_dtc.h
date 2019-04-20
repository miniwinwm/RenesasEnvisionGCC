#ifndef SERIAL_H_
#define SERIAL_H_

void serial_init(void);
void serial_send(uint8_t *const tx_buf, uint16_t tx_num);
bool serial_get_send_complete(void);
void serial_receive(uint8_t *const rx_buf, uint16_t rx_num);
bool serial_get_receive_complete(void);
void serial_close(void);

#endif /* SERIAL_H_ */
