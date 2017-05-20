CC = gcc
CPPFLAGS := $(CPPFLAGS)
CFLAGS := -O3 -ggdb $(CFLAGS)
LDFLAGS = -lSDL2 -lGL -lGLEW -lGLU -lm

SOURCES = main.c physics.c
SHADERS = vertex.glsl fragment.glsl
OBJECTS = $(patsubst %.c,%.o,$(SOURCES)) $(patsubst %.glsl,%.o,$(SHADERS))
HEADERS = $(wildcard *.h)
BINARY = maxwell

all: $(BINARY)

clean:
	rm -f $(OBJECTS) $(BINARY)

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $(CPPFLAGS) $(CFLAGS) $<

empty.o:
	echo > empty.c
	$(CC) -c -o $@ empty.c

%.o: %.glsl empty.o
	cp empty.o $@
	objcopy $@ --update-section=.data=$< --add-symbol=$(subst .,_,$<)=.data:0

$(BINARY): $(OBJECTS)
	$(CC) -o $@ $(LDFLAGS) $+

.PHONY: all clean
