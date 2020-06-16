//
// Need to clone the library package before:
//    git clone https://github.com/RiddleAndCode/ATAES132
//
// Compile with 
//    gcc -g aes_tst.c -o aes_tst -I ../include/ libataes.a
//

#include "aes_tst.h"

int fd = 0;

uint8_t mykey00[16] = {0xda, 0x48, 0x60, 0x88, 0xe2, 0x64, 0xff, 0x49, 0x45, 0xe9, 0xa9, 0xfe, 0x38, 0xb5, 0x84, 0x1f};
uint8_t mykey01[16] = {0xdb, 0x48, 0x60, 0x88, 0xe2, 0x64, 0xff, 0x49, 0x45, 0xe9, 0xa9, 0xfe, 0x38, 0xb5, 0x84, 0x1f};
uint8_t mykey02[16] = {0xdc, 0x48, 0x60, 0x88, 0xe2, 0x64, 0xff, 0x49, 0x45, 0xe9, 0xa9, 0xfe, 0x38, 0xb5, 0x84, 0x1f};

uint8_t test_connection()
{
    uint8_t config[4] = {0};

    uint8_t ret = aes132_read_size(config,  AES132_LOCK_KEYS_ADDR, 4);

    if (ret == AES132_DEVICE_RETCODE_SUCCESS)
    {
       printf("[] Connection OK: 0x%04x\n",ret);
    } else {
       printf("[] Connection FAILED\n");
    }

    return ret;
}

void write_key(uint8_t *key)
{
  uint8_t ret = 0;
  uint8_t config[4] = {0};

  config[0] = AES132_KEY_CONFIG_CHANGE_KEYS | AES132_KEY_CONFIG_RANDOM_NONCE; // Check datasheet for more config examples. Just an example

  ret = aes132m_write_memory(AES132_KEY_CONFIG_LENGTH, AES132_KEY_CONFIG_ADDR(0), config);
  if (ret == AES132_DEVICE_RETCODE_SUCCESS)
  {
     printf("[] Write Key Config OK: 0x%04x\n",ret);
  } else {
     printf("[] Write Key Config  FAILED\n");
  }

  ret = aes132m_write_memory(AES132_KEY_LENGTH, AES132_KEY_ADDR(0), key);
  if (ret == AES132_DEVICE_RETCODE_SUCCESS)
  {
     printf("[] Write Key OK: 0x%04x\n",ret);
  } else {
     printf("[] Write Key FAILED\n");
  }

}

void write_data()
{
  uint8_t ret = 0;
  uint8_t data[32] = {0};
  uint8_t config[4] = {0};

  // If uncommented this would require authentication to read/write from this data slot. More on the datasheet
/*
  config[0] = AES132_ZONE_CONFIG_AUTH_READ | AES132_ZONE_CONFIG_AUTH_WRITE; 
  ret =  aes132m_write_memory(AES132_ZONE_CONFIG_LENGTH, AES132_ZONE_CONFIG_ADDR(0), config);
  if (ret == AES132_DEVICE_RETCODE_SUCCESS)
  {
     printf("[] Config Zone OK: 0x%04x\n",ret);
  } else {
     printf("[] Config Zone FAILED\n");
  }
*/

  ret = aes132m_write_memory(32, AES132_USER_ZONE_ADDR(0), "deadbeefcafefeeddeadbeefcafefeed");
  if (ret == AES132_DEVICE_RETCODE_SUCCESS)
  {
     printf("[] Write Zone OK: 0x%04x\n",ret);
  } else {
     printf("[] Write Zone FAILED\n");
  }
  
  ret = aes132_read_size(data,  AES132_USER_ZONE_ADDR(0), 32);
  if (ret == AES132_DEVICE_RETCODE_SUCCESS)
  {
     printf("[] Read Zone OK: %32s\n",data);
  } else {
     printf("[] Read Zone FAILED\n");
  }

}

void authentication(uint8_t *key)
{
  uint8_t ret = 0;

  ret = aes132_nonce();
  if (ret == AES132_DEVICE_RETCODE_SUCCESS)
  {
     printf("[] Nonce OK: 0x%04x\n",ret);
  } else {
     printf("[] Nonce FAILED\n");
  }

  ret = aes132_inbound_auth_key(0, key, (AES132_AUTH_USAGE_READ_OK | AES132_AUTH_USAGE_WRITE_OK | AES132_AUTH_USAGE_KEY_USE));
  if (ret == AES132_DEVICE_RETCODE_SUCCESS)
  {
     printf("[] Authentication OK: 0x%04x\n",ret);
  } else {
     printf("[] Authentcation  FAILED\n");
  }
}

int main(void) {

  fd = open("/dev/i2c-1", O_RDWR);
  if (ioctl(fd, I2C_SLAVE, ((uint8_t)0xA0)>>1) < 0) {
    close(fd);
    return -1;
  }

  test_connection();

  // Write auth key, this is not required all the time
  write_key(mykey00);

  // Authentication required for read-write operations done
  authentication(mykey00);
  // Test Write and Read operations on the device
  write_data();

  // EncWrite
  uint8_t in_seed[12]	= {0};
  uint8_t in_data[32]	= {0x42,0x65,0x20,0x53,0x75,0x72,0x65,0x20,0x54,0x6F,0x20,0x44,0x72,0x69,0x6E,0x6B,0x20,0x59,0x6F,0x75,0x72,0x20,0x4F,0x76,0x61,0x6C,0x74,0x69,0x6E,0x65,0x00,0x00};
  
  for (int i = 0; i < 12; i++) {
    in_seed[i] = rand() % 0xFF;
  }
  aes132_encrypt_encwrite(in_data,32,mykey00,0,in_seed);
  
  // EncRead
  aes132_encread_decrypt(mykey00);

  // Deauth
  aes132_reset_auth();

  close(fd);

  return 0;
}
