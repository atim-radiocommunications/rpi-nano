########################################################################
# 
# Makfile for generate the documentation with doxygen and compilation
# the examples
# for using the api for controlling the arm-nano module.
#
# Auteur: Atim
# Created: 03.07.2014
#
# Utilisation:
# 	- make doc 		-> for generate the documentation (in doc).
# 	- make example 	-> for generate the executing file for the rpi. You
# 					can use TOOLCHAIN variable for choose the folder
#					direction of the toolchain.
#					(eg: make example TOOLCHAIN=/usr/arm).
# 	- make install 	-> for install example on your rpi in "/usr/bin", if
#					you use the cross compiling. You can use IP variable
#					for choose the ip of raspberrypi.
#					(eg: make install IP=192.168.0.10).
#					direction of the toolchain.
# 	- make cleandoc	-> for erase the doc.
# 	- make clean 	-> for erase the compilation files.
#
########################################################################


########################################################################
# Select a toolchain
########################################################################
TOOLCHAIN=
ifeq ($(TOOLCHAIN),)
	UNAME_P := $(shell uname -p)
	ifeq ($(UNAME_P),x86_64)
		CC=../../tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-gcc
	else ifeq ($(UNAME_P),i686)
		CC=../../tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/arm-linux-gnueabihf-gcc
	else
		CC=gcc
	endif
else
	CC=$(TOOLCHAIN)gcc
endif


########################################################################
# Install directory
########################################################################
DIRECTORY=/usr/bin


########################################################################
# IP
########################################################################
IP=pi.local
USER=root

########################################################################
# Sources files
########################################################################
EXAMPLES=send_to_sigfox
EXAMPLES+=rssi
EXAMPLES+=test2


########################################################################
# Binary folder
########################################################################
OBJ_DIR = obj


########################################################################
# Sources files
########################################################################
SRC=$(patsubst %, examples/%.c, $(EXAMPLES))
SRC+=api/rpi_nano.c

########################################################################
# Objs files
########################################################################
OBJS=$(patsubst %.c,$(OBJ_DIR)/%.o, $(SRC))


########################################################################
# Example rule
########################################################################
all:examples
examples: $(OBJS)
	$(CC) -o send_to_sigfox $(OBJ_DIR)/examples/send_to_sigfox.o $(OBJ_DIR)/api/rpi_nano.o -W -Wall -Wextra -std=gnu99 -g 
	$(CC) -o rssi $(OBJ_DIR)/examples/rssi.o $(OBJ_DIR)/api/rpi_nano.o -W -Wall -Wextra -std=gnu99 -g
	$(CC) -o test2 $(OBJ_DIR)/examples/test2.o $(OBJ_DIR)/api/rpi_nano.o -W -Wall -Wextra -std=gnu99 -g
	

########################################################################
# Build
########################################################################
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(patsubst /%,%, $(@D))
	$(CC) -o $@ -c $< $(INCLUDE_DIRS) -Wall -std=gnu99 -Iapi


########################################################################
# Install rule
########################################################################
install:
	scp $(EXAMPLES) $(USER)@$(IP):$(DIRECTORY)


########################################################################
# Doc rule
########################################################################
.PHONY: doc
doc:
	@cd doc && doxygen


########################################################################
# Cleandoc rule
########################################################################
.PHONY: cleandoc
cleandoc:
	rm -r doc/html

	
########################################################################
# Clean rule
########################################################################
.PHONY: clean
clean:
	rm -r $(OBJ_DIR)
	rm $(EXAMPLES)






