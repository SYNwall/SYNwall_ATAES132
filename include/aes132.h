

#ifndef AES132_TEST_H
#define AES132_TEST_H
#include <stdint.h>
int test_aes132();
uint8_t aes132_write_to_user_zone(uint32_t location, uint8_t *data, uint8_t size);
uint8_t aes132_read_from_user_zone(uint32_t location, uint8_t *data, uint8_t size);
#endif
