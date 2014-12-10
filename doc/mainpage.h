//! \file **************************************************************
//!
//! \brief Main page for the documentation.
//!
//! \author Atim.
//! \version 0.1
//! \date 04.07.2014
//!
//! ********************************************************************

#ifndef MAIN_PAGE_H
#define MAIN_PAGE_H

//! \mainpage RPI_nano Documentation
//!
//! \section intro_sec Introduction:
//! This API allow you to control easily the \b ARM-nano module. You will be able to send and
//! receive data on the sigfox network and local network.\n
//! You can visit web page of <a href="http://www.atim.com/product/rf-oem-module/arm-modules/new-rf-module-arm-nano">ARM-nano</a> for more informations.
//!
//! \section prog_sec Programming guide:
//!	- \ref start_page
//!	- \ref hard_man_page 
//!	- \ref api_ref_group 
//!	- \ref exa_page 


//! \page start_page Getting started
//! \tableofcontents
//! \section DisableTty_sec Disable the uart console on ttyAMA0:
//! - Disable the output boot:\n
//! In the file "/boot/cmdline.txt" replace the following line:
//! \code
//! dwc_otg.lpm_enable=0 console=ttyAMA0,115200 kgdboc=ttyAMA0,115200 console=tty1 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=deadline rootwait
//! \endcode
//! by
//! \code
//! dwc_otg.lpm_enable=0 console=tty1 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=deadline rootwait
//! \endcode
//! - Disable the login prompt:\n
//! In the file "/etc/inittab", remove or comment the following line:
//! \code
//! T0:23:respawn:/sbin/getty -L ttyAMA0 115200 vt100
//! \endcode
//!
//! \section hardware_sec Hardware preview:
//! \image html pi_arm_nano.jpg
//! Please read the \ref hard_man_page before connecting or using this API.
//!
//! \section API_sec API preview:
//! To send data to the sigfox network, you need these functions:
//! \code
//! int rpi_nano_set_modem_address(int fd, int address);
//! int rpi_nano_set_mode(int fd, rpi_nano_uart_mode_e mode);
//! int rpi_nano_send_data(int fd, char const* data, ssize_t size);
//! int rpi_nano_close(int fd);
//! 
//! \endcode
//!
//! \subsection exa_subsec Simple example:
//! \ref exa_send_to_sigfox_sec
//! \include send_to_sigfox.c
//! The result give:
//! \code
//! Init rpi_nano
//! Send to sigfox the datat:
//!         ->0x12
//!         ->0x13
//!         ->0xef
//!         ->0x21
//! ...
//! Close rpi_nano
//! \endcode
//!
//! \subsection exa_build Build and install:
//! First, create a work folder and copy the files rpi_nano.h and
//! rpi_nano.c:
//! \code
//! $ mkdir ~/test_rpi_nano
//! $ cd ~/test_rpi_nano
//! $ cp <rpi_nano>/api/rpi_nano.h .
//! $ cp <rpi_nano>/api/rpi_nano.c .
//! \endcode
//! Create the main.c file and add the code from the previous example.\n
//! Build the example:
//! \code
//! $ <toolchain_directory>gcc rpi_nano.c main.c -o ex1 -I. -std=gnu99
//! \endcode
//! Install and run the example:
//! \code
//! host$ scp ex1 root@192.168.0.167:/usr/bin
//! host$ ssh root@192.168.0.167
//! rpi$ ex1
//! \endcode
//!
//! \see \ref exa_page for more examples


//! \page hard_man_page Hardware manual
//! There are two version of <b> RPI_arm_nano </b> board. One with a UFL connector (picture with the RPi) and the second with SMA connector, like on following picture:
//! \image html pi_arm_nano.jpg
//! The <b> RPI_arm_nano </b> board is compatible with RPi A, A+, B and B+.\n
//! On the RPi A and B this extended board is connected on P1 connector. Like the following picture:
//! \image html rpiab.jpg
//! On the RPi A+ and B+ this extended board is connected on J8 connector. Like the following picture:
//! \image html rpibp.jpg
//! \attention If you want to use an other extended board for RPi, be sure that the other board don't use the uart pins (8, 10 and 7(gpio4 reserved for reset the arm-nano)) from RPi.


//! \page exa_page Examples
//! \tableofcontents
//! \section exa_build_sec Build and install all examples:
//! To build just call 'make' or 'make examples' in rpi_nano folder.
//! To install just call 'make install' in rpi_nano folder.\n
//! To install you need to connect with an ssh client on your host machine. You
//! can specify the IP addresser of your raspberry pi in the IP
//! variable (eg: "make install IP=192.168.0.10"), The default value is pi.local.
//! By default, the examples are install to "/usr/bin" you can change
//! this. Specify your folder in DIRECTORY variable.
//! (eg: "make install DIRECTORY=/usr/local/bin")\n
//! You can call the following command to build and install:
//! \code
//! $ make examples install DIRECTORY=<install_folder_in_rpi> IP=<ip_of_rpi>
//! \endcode
//!
//! \section exa_send_to_sigfox_sec Send data to sigfox:
//! This example send four bytes to sigfox network.
//! \include send_to_sigfox.c
//!
//! \section exa_rssi_sec Get the RSSI value:
//! This example get the rssi (Received Signal Strength Indication) periodically.
//! \include rssi.c

#endif //MAIN_PAGE_H

