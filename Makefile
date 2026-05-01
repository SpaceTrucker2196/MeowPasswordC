# MeowPassword - Cat Name Based Secure Password Generator
# Simple Makefile wrapper for building without CMake

CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -O2
LDFLAGS = -lm

# Source files
SRCDIR = src
TESTDIR = tests
SOURCES = $(SRCDIR)/main.c \
          $(SRCDIR)/config.c \
          $(SRCDIR)/password.c \
          $(SRCDIR)/complexity.c \
          $(SRCDIR)/catnames.c \
          $(SRCDIR)/display.c \
          $(SRCDIR)/update.c \
          $(SRCDIR)/random.c \
          $(TESTDIR)/test_meowpass.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Target executable
TARGET = meowpass

# Install directories
PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man/man1

.PHONY: all clean install uninstall test debug

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Debug build
debug: CFLAGS = -Wall -Wextra -pedantic -std=c11 -g -O0
debug: clean $(TARGET)

# Run tests
test: $(TARGET)
	./$(TARGET) --test

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -rf build/

# Install
install: $(TARGET)
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 $(TARGET) $(DESTDIR)$(BINDIR)/
	install -d $(DESTDIR)$(MANDIR)
	install -m 644 meowpass.1 $(DESTDIR)$(MANDIR)/

# Uninstall
uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(TARGET)
	rm -f $(DESTDIR)$(MANDIR)/meowpass.1

# CMake build (alternative)
cmake-build:
	mkdir -p build
	cd build && cmake .. && make

# Dependencies
$(SRCDIR)/main.o: $(SRCDIR)/meowpass.h
$(SRCDIR)/config.o: $(SRCDIR)/meowpass.h
$(SRCDIR)/password.o: $(SRCDIR)/meowpass.h
$(SRCDIR)/complexity.o: $(SRCDIR)/meowpass.h
$(SRCDIR)/catnames.o: $(SRCDIR)/meowpass.h
$(SRCDIR)/display.o: $(SRCDIR)/meowpass.h
$(SRCDIR)/update.o: $(SRCDIR)/meowpass.h
$(SRCDIR)/random.o: $(SRCDIR)/meowpass.h
$(TESTDIR)/test_meowpass.o: $(SRCDIR)/meowpass.h
