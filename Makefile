TARGET = server
CC = gcc
CFLAGS = -g -Wall

.PHONY: default all clean

default: $(TARGET)
all: default

SRV_OBJECTS = $(patsubst %.c, %.o, $(wildcard src/server/*.c))
SRV_HEADERS = $(wildcard src/server/*.h)

%.o: %.c $(SRV_HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(SRV_OBJECTS)

$(TARGET): $(SRV_OBJECTS)
	mkdir -p build
	$(CC) $(SRV_OBJECTS) -Wall -o build/$@ -lpthread

clean:
	-rm -rf build
	-rm -rf src/server/*.o
