/*
 * Galois/Counter Mode (GCM) and GMAC with AES
 *
 * Copyright (c) 2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

// http://ww1.microchip.com/downloads/en/DeviceDoc/ATAES132A-Data-Sheet-40002023A.pdf
// Read datasheet for more information

#include "tests.h"

void test_connection()
{
    uint8_t config[4] = {0};

    uint8_t ret = aes132_read_size(config,  AES132_LOCK_KEYS_ADDR, 4);
    TEST_ASSERT_EQUAL(AES132_DEVICE_RETCODE_SUCCESS, ret) ;

}

void test_write_key()
{
  uint8_t ret = 0;
  uint8_t config[4] = {0};

  config[0] = AES132_KEY_CONFIG_CHANGE_KEYS | AES132_KEY_CONFIG_RANDOM_NONCE; // Check datasheet for more config examples. Just an example

  ret = aes132m_write_memory(AES132_KEY_CONFIG_LENGTH, AES132_KEY_CONFIG_ADDR(0), config);
  TEST_ASSERT_EQUAL(AES132_DEVICE_RETCODE_SUCCESS, ret) ;

  ret = aes132m_write_memory(AES132_KEY_LENGTH, AES132_KEY_ADDR(0), key00);
  TEST_ASSERT_EQUAL(AES132_DEVICE_RETCODE_SUCCESS, ret) ;

}

void test_write_data()
{
  uint8_t ret = 0;
  uint8_t config[4] = {0}, data[16] = {0};

  // If uncommented this would require authentication to read/write from this data slot. More on the datasheet
  // data[0] = AES132_ZONE_CONFIG_AUTH_READ | AES132_ZONE_CONFIG_AUTH_WRITE; 

  ret =  aes132m_write_memory(AES132_KEY_CONFIG_LENGTH, AES132_ZONE_CONFIG_ADDR(0), config);
  TEST_ASSERT_EQUAL(AES132_DEVICE_RETCODE_SUCCESS, ret) ;

  ret = aes132m_write_memory(16, AES132_USER_ZONE_ADDR(0), key00);
  TEST_ASSERT_EQUAL(AES132_DEVICE_RETCODE_SUCCESS, ret) ;  
  
  ret = aes132_read_size(data,  AES132_USER_ZONE_ADDR(0), 16);
  TEST_ASSERT_EQUAL(AES132_DEVICE_RETCODE_SUCCESS, ret) ;

  TEST_ASSERT_EQUAL(memcmp(data, key00, 16), 0);
}

/*
 * In accordance with test_write_key
*/
void test_authentication()
{
  uint8_t ret = 0;

  ret = aes132_nonce();
  TEST_ASSERT_EQUAL(AES132_DEVICE_RETCODE_SUCCESS, ret) ;

  ret = aes132_inbound_auth_key(0, key00, (AES132_AUTH_USAGE_READ_OK | AES132_AUTH_USAGE_WRITE_OK | AES132_AUTH_USAGE_KEY_USE));
  TEST_ASSERT_EQUAL(AES132_DEVICE_RETCODE_SUCCESS, ret) ;
}

int main(void) {

  UNITY_BEGIN();
  RUN_TEST(test_connection);
  RUN_TEST(test_write_key);
  RUN_TEST(test_write_data);
  RUN_TEST(test_authentication);

  return UNITY_END();
}
