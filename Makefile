CC       ?= gcc
RAYLIB   = third_party/raylib-6.0/src
RAYGUI   = third_party/raygui-5.0/src
TARGET   = agte
SRC      = agte.c
HEADERS  = third_party/font_data.h
STATIC   = $(RAYLIB)/libraylib.a

CFLAGS   += -std=c99 -Wall -Wextra -O2 -I$(RAYLIB) -I$(RAYGUI) -Ithird_party
LDLIBS   = -lGL -lm -lpthread -ldl -lrt -lX11

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS) $(STATIC)
	$(CC) $(CFLAGS) $(SRC) -o $@ $(STATIC) $(LDLIBS)

$(STATIC): FORCE
	$(MAKE) -C $(RAYLIB) PLATFORM=PLATFORM_DESKTOP

FORCE:

clean:
	rm -f $(TARGET)

distclean: clean
	$(MAKE) -C $(RAYLIB) clean

.PHONY: all clean distclean FORCE
