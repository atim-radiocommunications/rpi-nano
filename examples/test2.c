//! \file **************************************************************
//!
//! \brief Source for send_to_sigfox example.
//!
//! \author Atim.
//! \version 0.1
//! \date 09.07.2014
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
	if(fd_rpi_nano == -1)
		return -1;
		
	//rpi_nano_set_mode(fd_rpi_nano, RPI_NANO_MODE_SEND_TO_SIGFOX);
	printf("%x\n", rpi_nano_get_sigfox_id(fd_rpi_nano));
	printf("%x\n", rpi_nano_get_version(fd_rpi_nano));
		
//------------------------------------------------------------------------------
		
	//printf("Baudrate before 0x%.2x\n", rpi_nano_get_radio_baudrate(fd_rpi_nano));
	//rpi_nano_set_radio_baudrate(fd_rpi_nano, RPI_NANO_RADIO_BAUDRATE_R38400_4GFSK);
	//printf("Baudrate after 0x%.2x\n", rpi_nano_get_radio_baudrate(fd_rpi_nano));
	
	//printf("Channel before 0x%.4x\n", rpi_nano_get_channel(fd_rpi_nano));
	//rpi_nano_set_channel(fd_rpi_nano, 0x01bc);
	//printf("Channel after 0x%.4x\n", rpi_nano_get_channel(fd_rpi_nano));
	
//------------------------------------------------------------------------------
	
	//rpi_nano_uart_baudrate_e 	baudrate	=0;
	//rpi_nano_uart_parity_e 		parity		=0;
	//rpi_nano_uart_stopbit_e 	stopBit		=0;

	//if(rpi_nano_get_uart_config(fd_rpi_nano, &baudrate, &parity, &stopBit) == -1)
		//return -1;
	//printf("Before:\n");
	//printf("\tbaudrate:%.2x\n", baudrate);
	//printf("\tparity:%.2x\n", parity);
	//printf("\tstopBit:%.2x\n", stopBit);
	
	//if(rpi_nano_set_uart_config(	fd_rpi_nano,
									//RPI_NANO_UART_BAUDRATE_115200,
									//RPI_NANO_UART_PARITY_EVEN,
									//RPI_NANO_UART_STOPBIT_2) == -1)
		//return -1;
	
	//baudrate	=0;
	//parity		=0;
	//stopBit		=0;
	//if(rpi_nano_get_uart_config(fd_rpi_nano, &baudrate, &parity, &stopBit) == -1)
		//return -1;
	//printf("After 1:\n");
	//printf("\tbaudrate:%.2x\n", baudrate);
	//printf("\tparity:%.2x\n", parity);
	//printf("\tstopBit:%.2x\n", stopBit);
	
	//if(rpi_nano_set_uart_config(	fd_rpi_nano,
									//RPI_NANO_UART_BAUDRATE_9600,
									//RPI_NANO_UART_PARITY_NO,
									//RPI_NANO_UART_STOPBIT_1) == -1)
		//return -1;
	
	//baudrate	=0;
	//parity		=0;
	//stopBit		=0;
	//if(rpi_nano_get_uart_config(fd_rpi_nano, &baudrate, &parity, &stopBit) == -1)
		//return -1;
	//printf("After 2:\n");
	//printf("\tbaudrate:%.2x\n", baudrate);
	//printf("\tparity:%.2x\n", parity);
	//printf("\tstopBit:%.2x\n", stopBit);
	
	
	
//------------------------------------------------------------------------------
	//if(rpi_nano_start_ping_pong(fd_rpi_nano, RPI_NANO_MODE_PING_PONG_MASTER) == -1)
		//return -1;
		
	//printf("Press 'q' + 'enter' key for stop + status and quit the test.\n");
	//printf("Press 's'+'enter' key for status.\n");
	
	//while(1)
	//{
		//char buf[8];
		//scanf("%s", buf);
		
		//if(*buf=='s')
		//{
			//char status[30];
			//rpi_nano_status_ping_pong(fd_rpi_nano, status, sizeof status);
			//printf("Status ping pong: %s\n", status);
		//}
		//else
			//break;
	//}

	//char status[30];
	//rpi_nano_status_ping_pong(fd_rpi_nano, status, sizeof status);
	//printf("End:\n\tStatus ping pong: %s\n", status);
	//rpi_nano_stop_ping_pong(fd_rpi_nano);
	//------------------------------------------------------------------------------

	//rpi_nano_set_mode(fd_rpi_nano, RPI_NANO_MODE_SEND_TO_LOCAL);
	//printf("%.2x\n", rpi_nano_get_version(fd_rpi_nano));
	
	//rpi_nano_reboot(fd_rpi_nano);

	////rpi_nano_reboot(fd_rpi_nano);
	//printf("id: %x\n", rpi_nano_get_sigfox_id(fd_rpi_nano));
	
//------------------------------------------------------------------------------

	
	//rpi_nano_enable_reset();
	//Close API
	printf("Close rpi_nano\n");
	if(rpi_nano_close(fd_rpi_nano) == -1)
		return -1;
	
	//int channel = 1;
		
	//for(int bps = RPI_NANO_RADIO_BAUDRATE_R1200_2GFSK; bps<=RPI_NANO_RADIO_BAUDRATE_R115200_4GFSK; bps++)
	//{
		//printf("%d\t", rpi_nano_available_max_power_p(bps, channel));
	//}
	//printf("\n");
	
	return 0;
}
