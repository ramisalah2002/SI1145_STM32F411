
UDEFS += -DUSE_MBEDSHIELD

LIBSHIELD = libshield

SRC += $(LIBSHIELD)/lm75.c $(LIBSHIELD)/gesture.c $(LIBSHIELD)/sunlight.c
