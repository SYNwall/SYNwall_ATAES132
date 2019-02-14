#ifndef _UECC_TESTS_H_
#define _UECC_TESTS_H_

#include "aes132.h"
#include "keys.h"

#include "aes132_helper.h"
#include "aes132_impl.h"
#include "unity.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port
void test_gcm(void);

#endif //_UECC_TESTS_H_
