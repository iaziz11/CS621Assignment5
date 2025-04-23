# Name of the kernel module (without .c)
obj-m := ticket.o

# The directory of the kernel build
KDIR := /lib/modules/$(shell uname -r)/build

# The current directory
PWD := $(shell pwd)

# Default target to build the kernel module
all:
	make -C $(KDIR) M=$(PWD) modules

# Clean target to remove build artifacts
clean:
	make -C $(KDIR) M=$(PWD) clean