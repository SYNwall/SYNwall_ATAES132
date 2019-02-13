/*
 * Galois/Counter Mode (GCM) and GMAC with AES
 *
 * Copyright (c) 2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */


#include "tests.h"


void test_connection()
{
    TEST_ASSERT_EQUAL(0,0);
    uint8_t config[4] = {0};
    uint8_t ret = aes132_read_size(config,  AES132_LOCK_KEYS_ADDR, 4);
    TEST_ASSERT_EQUAL(ret, AES132_DEVICE_RETCODE_SUCCESS);

}

int main(void) {

  UNITY_BEGIN();
  RUN_TEST(test_connection);
  return UNITY_END();
}
