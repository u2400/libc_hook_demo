obj-m   := core.o

KDIR    := /usr/src/linux-headers-4.19.0-parrot4-28t-amd64/
PWD    := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules