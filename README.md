
# SYNwall interface for ATAES132A

This is a PoC project. It is forked from the original project at [ATAES132](https://github.com/RiddleAndCode/ATAES132). 

I just built a little C program using this library, to show how inject a PSK in SYNwall getting it directly from a secure EEPROM connected via I2C. Tests has been performed on a Raspberry Pi with a [SecureClick](https://www.mikroe.com/secure-click) device.

Original library has been modified in some parts.

Installation
---

Clone the repository, then build the original library:

```
mkdir build 
cd build
cmake ..
make
```

Then go in "aes132_SYNwall" folder and build the program

```
cd aes132_SYNwall
gcc aes132_synwall.c -o aes132_synwall -Wall -I ../include/ ../build/src/libataes.a
```

This is the help:

```
Usage: aes312_synwall <OPTIONS>

        -l          Load the PSK in SYNwall
        -w          Store a PSK in EEPROM
        -a          Test Authentication
```

The usage of the EEPROM is not really straightforward, you may want to review the [DataSheet](http://ww1.microchip.com/downloads/en/devicedoc/Atmel-8760-CryptoAuth-ATAES132-Datasheet.pdf) and to the *tests* folder to see some examples.

# ATAES132A
Inside the RiddleAndCode secure element, one can find the ATAES132A chip. This library provides functions to interact with it. For extensive information about the chip and its configuration please check [ATAES132A](http://ww1.microchip.com/downloads/en/DeviceDoc/ATAES132A-Data-Sheet-40002023A.pdf).

One usage is the secure storage of sensitive data. For more, read below..

Instructions
---

```
mkdir build 
cd build
```

For tests run (RPI support only*):
```
cmake .. -DTARGET_GROUP=test
make
ctest --verbose
```
\* I2C on the raspberry pi should be activated. Please check [RiddleAndCode/EClet](https://github.com/RiddleAndCode/EClet) for more info.

For static library:
```
cmake .. 
make
```


Usage
---
One can check tests/tests.c for example use cases. Note: for provisioning the device needs to be locked:

```
  ret = aes132_lock_zone(AES132_LOCK_CONFIG);
  ret = aes132_lock_zone(AES132_LOCK_KEYMEMORY);
```
