//! \file **************************************************************
//!
//! \brief Source for send_to_sigfox example.
//!
//! \author Atim.
//! \version 0.1
//! \date 03.07.2014
//!
//! ********************************************************************

// -- INCLUDE ----------------------------------------------------------
#include <stdio.h>
#include <rpi_nano.h>

// -- FUNCTION ---------------------------------------------------------
int main()
{
	//Initialise API and ARM-nano.
	printf("Init rpi_nano\n");
	int fd_rpi_nano = rpi_nano_init(NULL,
									RPI_NANO_UART_BAUDRATE_19200,
									RPI_NANO_UART_PARITY_NO,
									RPI_NANO_UART_STOPBIT_1);
									
	//Set the sigfox mode.
	rpi_nano_set_mode(fd_rpi_nano, RPI_NANO_MODE_SEND_TO_SIGFOX);
	
	//Prepares the data.
	char data[]={	0x12,
					0x13,
					0xef,
					0x21};
	printf("Send to sigfox the data:\n");
	for(int i = 0; i < 4; i++)
		printf("\t->0x%x\n", data[i]);
	printf("...\n");

	//Send data to sigfox.
	rpi_nano_send_data(fd_rpi_nano, data, 4);

	//Close API
	printf("Close rpi_nano\n");
	rpi_nano_close(fd_rpi_nano);
	
	return 0;
}

