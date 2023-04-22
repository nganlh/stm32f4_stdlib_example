# ADC Example

## Necessary modifications:

1. Insert this to Makefile
```
CFLAGS += -specs=nano.specs -specs=nosys.specs
```

2. Remove this because we do not use the FPU.
```
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
```

3. Add this to enable displaying float number.
```
CFLAGS += -u _printf_float
```
