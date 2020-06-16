#include "aes132.h"
#include "keys.h"

#include "aes132_helper.h"
#include "aes132_impl.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port
