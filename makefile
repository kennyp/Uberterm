CC=gcc
BIN=uberterm
CFLAGS=-o $(BIN) -Wall `pkg-config --cflags --libs gtk+-2.0 vte`

all: $(BIN)

clean:
	rm -f $(BIN)

install: $(BIN)
	cp $(BIN) /usr/local/bin/

$(BIN): main.c main.h
	$(CC) $(CFLAGS) main.c

.PHONY: all clean install
