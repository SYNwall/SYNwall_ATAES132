#include "aes132.h"
#include "keys.h"

#include "aes132_helper.h"
#include "aes132_impl.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>                     //Needed for I2C port
#include <fcntl.h>                      //Needed for I2C port
#include <sys/ioctl.h>                  //Needed for I2C port
#include <linux/i2c-dev.h>              //Needed for I2C port

#define INKEY_BUFFER  33
#define PSK_BUFFER   256

void print_help(void);
uint8_t term_disable_echo(void);
uint8_t term_enable_echo(void);
void read_noecho(char *inkey, int len, char *prompt);
uint8_t test_auth(void);
uint8_t load_key_array(char *data_buffer, uint8_t length);
uint8_t authentication(uint8_t key_address, uint8_t *key);
uint8_t store_PSK(void);
uint8_t load_PSK(void);
uint8_t test_connection(void);

//I2C file descriptor
int fd = 0;

// Authentication Key
uint8_t SYNkey00[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Term I/O variables
char key_prompt[] = "Enter Key: ";
char psk_prompt[] = "Enter PSK: ";

struct termios oflags, nflags;
