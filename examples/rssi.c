//! \file **************************************************************
//!
//! \brief Source for rssi example.
//!
//! \author Atim.
//! \version 0.1
//! \date 03.07.2014
//!
//! ********************************************************************

// -- INCLUDE ----------------------------------------------------------
#include <stdio.h>
#include <fcntl.h>
#include <rpi_nano.h>
#include <errno.h>

// -- FUNCTION ---------------------------------------------------------
int main()
{
	//Initialise API and ARM-nano.
	printf("Init rpi_nano\n");
	int fd_rpi_nano = rpi_nano_init(NULL,
									RPI_NANO_UART_BAUDRATE_19200,
									RPI_NANO_UART_PARITY_NO,
									RPI_NANO_UART_STOPBIT_1);
	
	//Print minimal help.
	printf("Press 'Entry' to quit!\n");
	
	//Timerout of 1s
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1000;
	
	if (rpi_nano_start_ping_pong(fd_rpi_nano, 0x00) == -1){
		fprintf(stderr, "ERROR - rpi_nano_start_ping_pong\n");
	}
	
	
	//Infinity loop
	while(1)
	{
		//File descriptor in reading
		fd_set fd_read;
		FD_ZERO(&fd_read);
		FD_SET(0, &fd_read);
		
		//Wait for reading or Timerout.
		select(0+1, &fd_read, NULL, NULL, &timeout);
		
		//Read rssi en print value.
		if(FD_ISSET(0, &fd_read))
		{
			break;
		}
		//Or Timerout
		else
		{
			char per[10];
			char ber[10];
			char rssi[16];
			char res[100];
			if (rpi_nano_status_ping_pong(fd_rpi_nano,rssi, sizeof rssi, ber, sizeof ber, per, sizeof per) == -1){
				fprintf(stderr, "ERROR - rpi_nano_status_ping_pong\n");
			};
			snprintf(res, sizeof(res),"%s%s%s%s%s%s%s","{\"per\":",per,",\"ber\":",ber,",\"rssi\":",rssi,"}");
			printf("%s\n",res);
			timeout.tv_usec = 1000;
		}
	}
	if (rpi_nano_stop_ping_pong(fd_rpi_nano) == -1){
		fprintf(stderr, "ERROR - rpi_nano_stop_ping_pong\n");
	}
	//Close API
	printf("Close rpi_nano\n");
	rpi_nano_close(fd_rpi_nano);
	
	return 0;
}
