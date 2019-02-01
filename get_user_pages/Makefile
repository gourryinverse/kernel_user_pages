# kbuild part of makefile
obj-m := ukm.o

KDIR ?= /lib/modules/`uname -r`/build

EXTRA_CFLAGS += -fomit-frame-pointer
EXTRA_CFLAGS += -O0

ORIG_CFLAGS := $(KBUILD_CFLAGS)
KBUILD_CFLAGS := $(subst -pg,,$(ORIG_CFLAGS))

default:
	$(MAKE) -C $(KDIR) M=$$PWD modules
	gcc -o tester tester.c
	gcc -o memspiker mem.c

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) $@
	rm -f memspiker
	rm -f tester
