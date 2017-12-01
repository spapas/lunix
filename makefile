CC = gcc

# Find out the directories where the kernel lives

KERNELDIR := /lib/modules/$(shell uname -r)/build
INSTALLDIR := /lib/modules/$(shell uname -r)/kernel/drivers/char

include $(KERNELDIR)/.config

# Get the architecture-specific options to the C compiler
ARCH_CFLAGS=$(shell cd $(KERNELDIR) ; make script 'SCRIPT=@echo $$(CFLAGS)' ARCH=um)

CFLAGS = $(ARCH_CFLAGS)
CFLAGS += -D__KERNEL__ -DMODULE -I$(KERNELDIR)/include 
CFLAGS += -O2 
CFLAGS += -Wall

# Take care of compiling for an SMP enabled kernel
ifdef CONFIG_SMP
	CFLAGS += -D__SMP__ -DSMP
endif

# Prepend modversions.h if compiling for a kernel using module versioning
ifdef CONFIG_MODVERSIONS
	CFLAGS += -DMODVERSIONS -include $(KERNELDIR)/include/linux/modversions.h
endif

OBJS = lunix.o

all: $(OBJS)

install: $(OBJS)
	install -d $(INSTALLDIR)
	install -c -m 644 $(OBJS) $(INSTALLDIR)
	depmod -a

clean:
	rm -f $(OBJS) *~ core

# Dependencies follow
lunix.o: lunix.c lunix.h

