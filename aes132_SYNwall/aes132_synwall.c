
/*
 * 
 *  gcc aes132_synwall.c -o aes132_synwall -g -Wall -fsanitize=address -I ../include/ 
 *                 ../build/src/libataes.a
 *  LD_PRELOAD=/usr/lib/arm-linux-gnueabihf/libasan.so.5 ./aes132_synwall -a
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>

#include "aes132_synwall.h"


// FIXME: add a secure wiping for memory (explicit_bzero)
// To test make a core dump
//       *((int*)NULL) = 1;
//        ulimit -c 99999999

void print_help(void)
{

  fprintf(stderr, "Usage: aes312_synwall <OPTIONS>\n\n");
  fprintf(stderr, "\t-l          Load the PSK in SYNwall\n");
  fprintf(stderr, "\t-w          Store a PSK in EEPROM\n");
  fprintf(stderr, "\t-a          Test Authentication\n");
  fprintf(stderr, "\n");
}

uint8_t term_disable_echo(void)
{
  tcgetattr(fileno(stdin), &oflags);
  nflags = oflags;
  nflags.c_lflag &= ~ECHO;
  nflags.c_lflag |= ECHONL;
  if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
      perror("tcsetattr");
      return(EXIT_FAILURE);
  }

  return(EXIT_SUCCESS);
}

uint8_t term_enable_echo(void)
{
  if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
      perror("tcsetattr");
      return(EXIT_FAILURE);
  }

  return(EXIT_SUCCESS);
}

void read_noecho(char *inkey, int len, char *prompt)
{
  term_disable_echo();

  printf(prompt);
  fgets(inkey, len, stdin);
  inkey[strlen(inkey)] = 0;

  term_enable_echo();
}

uint8_t load_key_array(char *data_buffer, uint8_t length)
{

  size_t count;
  uint8_t c;

  // FIXME: do some sanitization: even number of char, only 09af
  if ( strlen(data_buffer) != 32 )
  {
    fprintf(stderr,"[!] Key must be 32 HEX chars\n");
    return(EXIT_FAILURE);
  }

  char *pos = data_buffer;

  for (count = 0; count < strlen(data_buffer) / 2; count++)
  {
    sscanf(pos, "%2hhx", &SYNkey00[count]);
    pos += 2;
  }

  // Discard other STDIN chars
  while((c = fgetc(stdin)) != '\n' && c != EOF);

  return(EXIT_SUCCESS);
}

uint8_t test_auth(void)
{
  char inkey[INKEY_BUFFER] = { 0 };

  read_noecho(inkey, INKEY_BUFFER, key_prompt);
  if ( load_key_array(inkey, INKEY_BUFFER) != 0 )
  {
    return(EXIT_FAILURE);
  }

  // Clean temporary buffer
  explicit_bzero(inkey,INKEY_BUFFER);

  return(authentication(0,(uint8_t *) SYNkey00));
}

uint8_t authentication(uint8_t key_address, uint8_t *key)
{
  uint8_t ret = 0;

  ret = aes132_nonce();
  if (ret == AES132_DEVICE_RETCODE_SUCCESS)
  {
     // printf("[] Nonce OK: 0x%04x\n",ret);
  } else {
     printf("[] Nonce FAILED\n");
  }

  ret = aes132_inbound_auth_key(key_address, key, (AES132_AUTH_USAGE_READ_OK | AES132_AUTH_USAGE_WRITE_OK | AES132_AUTH_USAGE_KEY_USE));
  if (ret == AES132_DEVICE_RETCODE_SUCCESS)
  {
     printf("[] Authentication OK: 0x%04x\n",ret);
     return(EXIT_SUCCESS);
  } else {
     printf("[] Authentcation  FAILED\n");
     return(EXIT_FAILURE);
  }
}

uint8_t test_connection()
{
    uint8_t config[4] = {0};

    uint8_t ret = aes132_read_size(config,  AES132_LOCK_KEYS_ADDR, 4);

    if (ret == AES132_DEVICE_RETCODE_SUCCESS)
    {
       // fprintf(stdio,"[] Connection OK: 0x%04x\n",ret);
    } else {
       fprintf(stderr,"[] Connection to ATAES132 FAILED\n");
    }

    return(ret);
}

uint8_t store_PSK(void)
{
  char inkey[INKEY_BUFFER] = { 0 };
  char psk[PSK_BUFFER] = { 0 };

  int psklen;
  int mem_addr = 0;

  // Ask the key (authentication is required to store keys)
  read_noecho(inkey, INKEY_BUFFER, key_prompt);
  if ( load_key_array(inkey, INKEY_BUFFER) != 0 )
  {
    // Clean up temporary buffer
    explicit_bzero(inkey,INKEY_BUFFER);
    return(EXIT_FAILURE);
  }

  if ( authentication(0,(uint8_t *) SYNkey00) != 0 )
  {
    // Clean up temporary buffer
    explicit_bzero(inkey,INKEY_BUFFER);
  	return(EXIT_FAILURE);
  }
  // Clean up temporary buffer
  explicit_bzero(inkey,INKEY_BUFFER);

  // Ask the PSK
  // TODO: may be we can accept just hex representation of key...
  read_noecho(psk, PSK_BUFFER, psk_prompt);

  // Check PSK min length
  psklen = strlen(psk);
  if ( psklen < 32 )
  {
    fprintf(stderr,"[!] PSK must be at least 32 chars\n");
    return(EXIT_FAILURE);
  }

  // Store PSK (chunck of 32 bytes)
  
  uint8_t in_seed[12] = {0};
  for (int i = 0; i < 12; i++) {
    in_seed[i] = rand() % 0xFF;
  }
  aes132_encrypt_encwrite((uint8_t *) psk,32,SYNkey00,mem_addr,in_seed,0);
  psklen -= 32;
  while ( ( psklen % 32 ) > 0 )
  {
    mem_addr += 32;
    psklen -= 32;
    if ( psklen < 0 )
    {
      psklen = 0;
    }
    aes132_encrypt_encwrite((uint8_t *) psk + mem_addr,32,SYNkey00,mem_addr,in_seed,0);
  }

  // Clean up temporary buffer
  explicit_bzero(psk,PSK_BUFFER);
  return(EXIT_SUCCESS);
}

uint8_t load_PSK(void)
{
  char inkey[INKEY_BUFFER] = { 0 };
  char psk[PSK_BUFFER] = { 0 };

  int chunck_len;
  int mem_addr = 0;

  // Ask the key (authentication is required to read keys)
  read_noecho(inkey, INKEY_BUFFER, key_prompt);
  if ( load_key_array(inkey, INKEY_BUFFER) != 0 )
  {
    // Clean up temporary buffer
    explicit_bzero(inkey,INKEY_BUFFER);
    return(EXIT_FAILURE);
  }

  if ( authentication(0,(uint8_t *) SYNkey00) != 0 )
  {
    // Clean up temporary buffer
    explicit_bzero(inkey,INKEY_BUFFER);
    return(EXIT_FAILURE);
  }

  // Clean up temporary buffer
  explicit_bzero(inkey,INKEY_BUFFER);

  // Read PSK from EEPROM (in 32 bytes chunks)
  aes132_encread_decrypt(SYNkey00,mem_addr,(uint8_t *) psk,0);
  chunck_len = strlen(psk);
  
  while ( chunck_len >= 32 && mem_addr < PSK_BUFFER )
  {
    mem_addr += 32;
    aes132_encread_decrypt(SYNkey00,mem_addr,(uint8_t *) psk + mem_addr,0);
    chunck_len = strlen(psk+mem_addr);
  }
  psk[mem_addr + chunck_len - 1] = '\0';

  // Load the PSK in the SYNwall module
  if ( inject_PSK(psk, strlen(psk)) == EXIT_FAILURE )
  {
    // Clean up temporary buffer
    explicit_bzero(psk,PSK_BUFFER);
    return(EXIT_FAILURE);
  }

  // Clean up temporary buffer
  explicit_bzero(psk,PSK_BUFFER);
  return(EXIT_SUCCESS);
}

uint8_t inject_PSK(char *psk, int len)
{
  // This is the paramter file used to inject the PSK
  //     "/sys/module/SYNwall/parameters/psk"
  int fsynwall;

  fsynwall = open(SYNWALL_PSK_FILE, O_WRONLY);
  if (fsynwall == -1)
  {
    fprintf(stderr, "[!] Cannot open the SYNwall parameters file.\n"); 
    return(EXIT_FAILURE);
  }

  if ( write(fsynwall,psk,len) <= 0 )
  {
    fprintf(stderr, "[!] Cannot write to the SYNwall parameters file.\n"); 
    close(fsynwall);
    return(EXIT_FAILURE);
  }

  close(fsynwall);
  return(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{

  uint8_t res;

  // Open the ATAE132 connection via I2C
  fd = open("/dev/i2c-1", O_RDWR);
  if (ioctl(fd, I2C_SLAVE, ((uint8_t)0xA0)>>1) < 0) {
    close(fd);
    return(EXIT_FAILURE);
  }

  if ( test_connection() != 0)
  {
  	return(EXIT_FAILURE);
  }

  for (;;)
  {
    int opt = getopt(argc, argv, ":walh");
    
    if (opt == -1)
    {
      break;
    }

    switch (opt)
    {
      case '?':
        fprintf(stderr, "[!] %s: Unexpected option: %c\n", argv[0], optopt);
        res = 1;
        break;
      case ':':
        fprintf(stderr, "[!] %s: Missing value for: %c\n", argv[0], optopt);
        res = 1;
        break;
      case 'l':
        // Load PSK in SYNwall
        res = load_PSK();
        break;
      case 'w':
        // Store a new PSK
        res = store_PSK();
        break;
      case 'a':
      	// Test Authentication
        res = test_auth();
        break;
      case 'h':
        print_help();
        res = 1;
        break;
      default:
        print_help();
        res = 1;
        break;
    }
  }

  // Deauth
  fprintf(stdout, "[] Deauthenticating\n");
  aes132_reset_auth();

  close(fd);

  // Clean up memory
  explicit_bzero(SYNkey00,16);

  // Core dump
  *((int*)NULL) = 1;

  if ( res != 0)
  {
    return(EXIT_FAILURE);
  } 
  else
  {
    return(EXIT_SUCCESS);	
  }
}


// da486088e264ff4945e9a9fe38b5841f
// 12345678901234567890123456789012
// da486088e264ff4945e9a9fe38b5841c