CC=g++
CFLAGS=-O2 -lwiringPi -lssd1306 -lnfc -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s 

SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(SOURCES:src/%.cpp=obj/%.o)

all: bin/heard

bin/heard: $(OBJECTS)
	$(CC) $(CFLAGS) $< -o $@

$(OBJECTS): obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj/* bin/*
