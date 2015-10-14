LDFLAGS := -lidn -Wl,-rpath,/opt/libjit/lib -L/opt/libjit/lib -ljit
CFLAGS ?= -g -Wall --optimize
CFLAGS += -I/opt/libjit/include

.PHONY: all

all: main
	./main $(DOMAIN)

main.c: Makefile
	touch $@

main: main.c
