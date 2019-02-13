
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
