TARGET = server
CC = gcc
CFLAGS = -g -Wall

.PHONY: default all clean

default: $(TARGET)
all: default

SRV_OBJECTS = $(patsubst %.c, %.o, $(wildcard src/server/*.c))
SRV_HEADERS = $(wildcard src/server/*.h)

UNIT_TEST_OBJECTS = $(patsubst %.c, %.o, $(wildcard test/*.c))
UNIT_TEST_HEADERS = $(wildcard test/*.h)

%.o: %.c $(SRV_HEADERS) $(UNIT_TEST_HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(SRV_OBJECTS)

$(TARGET): $(SRV_OBJECTS)
	mkdir -p build
	$(CC) $(SRV_OBJECTS) -Wall -o build/$@ -lpthread

unit_test: $(UNIT_TEST_OBJECTS) src/server/map.o
	mkdir -p build
	$(CC) $(UNIT_TEST_OBJECTS) src/server/map.o -Wall -o build/$@

run_unit_test: unit_test
	build/unit_test
clean:
	-rm -rf build
	-rm -rf src/server/*.o
	-rm -rf test/*.o
