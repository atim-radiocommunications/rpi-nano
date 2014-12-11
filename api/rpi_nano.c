//! \file **************************************************************
//!
//! \brief Source for controlling the ARM-nano.
//!
//! \author Atim.
//! \version 0.1
//! \date 03.07.2014
//!
//! ********************************************************************

// -- INCLUDE ----------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <rpi_nano.h>

// -- DEFINE -----------------------------------------------------------
#define _RPI_NANO_SIZE_BUF					64			//In bytes
#define _RPI_NANO_TIME_BOOT					100000		//In us
#define _RPI_NANO_READ_TIMEOUT				40000		//In us
#define _RPI_NANO_GPIO_RESET				"4"
//! \todo Vérifier cette valeur
#define _RPI_NANO_MIN_POWER					RPI_NANO_POWER_0dBm

#define RPI_NANO_AT_MODE_ON()	if(_rpi_nano_switch_to_AT(fd))	\
									return -1
#define RPI_NANO_AT_MODE_OFF()	if(_rpi_nano_exit_AT(fd))		\
									return -1

// -- PROTOTYPE --------------------------------------------------------
static int _rpi_nano_set_fd_parameter(	int fd,
										rpi_nano_uart_baudrate_e baudrate,
										rpi_nano_uart_parity_e parity,
										rpi_nano_uart_stopbit_e stopbit);
static int _rpi_nano_switch_to_AT(		int fd);
static int _rpi_nano_exit_AT(			int fd);
static ssize_t _rpi_nano_read(			int fd, char* buf, ssize_t nbyte,
										int firstTimeout, int timeout);
static int _rpi_nano_gpio_reset_export_output();
static int _rpi_nano_gpio_reset_unexport_output();
static int _rpi_nano_set_reg(			int fd, char* reg, int val);
static int _rpi_nano_get_reg(			int fd, char* reg, int* val);
 
// -- FUNCTION ---------------------------------------------------------
int _rpi_nano_set_fd_parameter(	int fd,
								rpi_nano_uart_baudrate_e baudrate,
								rpi_nano_uart_parity_e parity,
								rpi_nano_uart_stopbit_e stopbit)
{
	struct termios cfg;
	
	//Get current attributes
	if(tcgetattr(fd, &cfg) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	//Set speed
	switch(baudrate)
	{
		case RPI_NANO_UART_BAUDRATE_1200:
			cfsetispeed(&cfg, B1200);
			cfsetospeed(&cfg, B1200);
		break;
		
		case RPI_NANO_UART_BAUDRATE_2400:
			cfsetispeed(&cfg, B2400);
			cfsetospeed(&cfg, B2400);
		break;
		
		case RPI_NANO_UART_BAUDRATE_4800:
			cfsetispeed(&cfg, B4800);
			cfsetospeed(&cfg, B4800);
		break;
		
		case RPI_NANO_UART_BAUDRATE_9600:
			cfsetispeed(&cfg, B9600);
			cfsetospeed(&cfg, B9600);
		break;
		
		case RPI_NANO_UART_BAUDRATE_19200:
			cfsetispeed(&cfg, B19200);
			cfsetospeed(&cfg, B19200);
		break;
		
		case RPI_NANO_UART_BAUDRATE_38400:
			cfsetispeed(&cfg, B38400);
			cfsetospeed(&cfg, B38400);
		break;
		
		case RPI_NANO_UART_BAUDRATE_57600:
			cfsetispeed(&cfg, B57600);
			cfsetospeed(&cfg, B57600);
		break;
		
		case RPI_NANO_UART_BAUDRATE_115200:
			cfsetispeed(&cfg, B115200);
			cfsetospeed(&cfg, B115200);
		break;
		
		case RPI_NANO_UART_BAUDRATE_230400:
			cfsetispeed(&cfg, B230400);
			cfsetospeed(&cfg, B230400);
		break;
	}
	
	//8bits of data
	cfg.c_cflag &= ~CSIZE;
	cfg.c_cflag |= CS8;
	
	//No flow.
	cfg.c_cflag &= ~CRTSCTS;
	
	//Set parity
	switch(parity)
	{
		case RPI_NANO_UART_PARITY_NO:
		cfg.c_cflag &= ~PARENB;
		break;
		
		case RPI_NANO_UART_PARITY_ODD:
		cfg.c_cflag |= PARENB|PARODD;
		break;
		
		case RPI_NANO_UART_PARITY_EVEN:
		cfg.c_cflag |= PARENB;
		cfg.c_cflag &= ~PARODD;
		break;
	}
	
	//Set bit stop
	if(stopbit == RPI_NANO_UART_STOPBIT_1)
		cfg.c_cflag &= ~CSTOPB;
	else
		cfg.c_cflag |= CSTOPB;
	
	
	cfg.c_lflag &= ~(ICANON | ISIG | ECHO);
	cfg.c_oflag |= OPOST;
	cfg.c_iflag &= ~(IXON | IXOFF | IXANY);
	
	//Set new attributes.
	tcflush(fd, TCIFLUSH);
	if(tcsetattr(fd, TCSANOW, &cfg) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	return 0;
}

//! \todo modifier le timeout qui est actuellement à 15s
int _rpi_nano_switch_to_AT(int fd)
{
	//Read response.
	char buf[_RPI_NANO_SIZE_BUF];
	
	
	
	//Switch to AT commend mode.
	if(write(fd, "+++", 3) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}

	
	//First timeout 15s
	if(_rpi_nano_read(fd, buf, _RPI_NANO_SIZE_BUF, 15000000, -1) == -1)
	{
		fprintf(stderr, "ERROR - %s:  Can't read correctly.\n", __func__);
		return -1;
	}

	//Parse and check response.
	if(strstr(buf, "+++") != NULL) //If echo ("+++") the ARM-nano is already AT commend mode.
		return 0;
	if(strstr(buf, "WELCOME IN SETUP MODE") == NULL)
	{	
		fprintf(stderr,"Received:%s\n",buf);
		fprintf(stderr, "ERROR - %s: Bad response from ARM-nano.\n", __func__);
		return -1;
	}
	
	return 0;
}

//! \todo remove sleep function
int _rpi_nano_exit_AT(int fd)
{
	//Send AT commend.
	if(write(fd, "ATQ\n", 4) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	//Read response.
	//sleep(4);
	char buf[_RPI_NANO_SIZE_BUF];
	if(_rpi_nano_read(fd, buf, _RPI_NANO_SIZE_BUF, -1, -1) == -1)
	{
		fprintf(stderr, "ERROR - %s:  Can't read correctly.\n", __func__);
		return -1;
	}
	
	if(strstr(buf, "QUIT SETUP") == NULL)
	{
		fprintf(stderr, "ERROR - %s: Bad response from ARM-nano.\n", __func__);
		return -1;
	}
	usleep(100000);
	if (tcflush(fd,TCIFLUSH) == -1){
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	return 0;
}

int rpi_nano_reboot(int fd)
{
	RPI_NANO_AT_MODE_ON();
	
	//Send AT commend.
	if(write(fd, "ATR\n", 4) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		RPI_NANO_AT_MODE_OFF();
		return -1;
	}
	
	//Read response.
	char buf[_RPI_NANO_SIZE_BUF];
	if(_rpi_nano_read(fd, buf, _RPI_NANO_SIZE_BUF, -1, -1) == -1)
	{
		fprintf(stderr, "ERROR - %s: Can't read correctly.\n", __func__);
		RPI_NANO_AT_MODE_OFF();
		return -1;
	}
	RPI_NANO_AT_MODE_OFF();
	
	//Parse and check response.
	if(strstr(buf, "RESTARTING") == NULL)
	{
		fprintf(stderr, "ERROR - %s: Bad response from ARM-nano.\n", __func__);
		return -1;
	}
	
	//Read for wait reboot.
	usleep(_RPI_NANO_TIME_BOOT);
	
	return 0;
}

ssize_t _rpi_nano_read(int fd, char* buf, ssize_t nbyte, int firstTimeout, int timeout)
{
	ssize_t sizeBuf = nbyte-1;
	char *ptrBuf = buf;
	ssize_t n = 0;
	int escape = 0;
	
	struct timeval _timeout;
	_timeout.tv_sec = 0;
	_timeout.tv_usec = firstTimeout==-1?_RPI_NANO_READ_TIMEOUT:firstTimeout;
	
	timeout = timeout==-1?_RPI_NANO_READ_TIMEOUT:timeout;

		
	while(1)
	{		
		fd_set fd_read;
		FD_ZERO(&fd_read);
		FD_SET(fd, &fd_read);
  
		//Wait for reading.
		if(select(fd+1, &fd_read, NULL, NULL, &_timeout) == -1)
		{
			fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
			return -1;
		}
		
		//Read data.
		if(FD_ISSET(fd, &fd_read))
		{
			n = read(fd, ptrBuf, sizeBuf);
			if(n == -1)
			{
				fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
				return -1;
			}
			
			if(n==0)
				break;

			sizeBuf -= n;
			ptrBuf += n;
			
			//check if receive '\n'.
			*ptrBuf = '\0';
			if(strchr(ptrBuf-n, '\n') != NULL)
			{
				escape++;
				if(escape >= 2)
					break;
			}
			
			//The buffer is full ?
			if((ssize_t)(ptrBuf-buf) >= nbyte)
				break;
		}
		//Or Timerout
		else
			break;
			
		//Set timerout
		_timeout.tv_sec = 0;
		_timeout.tv_usec = timeout;
	}
 
	buf[ptrBuf-buf] = '\0';
	return ptrBuf-buf;
}

//! \todo Il y a peut être une meilleur solution que d'attendre
//! (50 ms avec usleep) la création du fichier de direction du gpio...
int _rpi_nano_gpio_reset_export_output()
{
	//Open gpio export.
	int gpio_fd = open("/sys/class/gpio/export", O_WRONLY);
	if(gpio_fd == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	//Export gpio.
	if(write(gpio_fd, _RPI_NANO_GPIO_RESET, 2) == -1)
	{
		//If error is "device or resource bus", the export is already did.
		//So, there is no error.
		if(errno != EBUSY)
		{
			close(gpio_fd);
			fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
			return -1;
		}
	}
	
	//Close gpio export.
	if(close(gpio_fd) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	//Wait the create file direction.
	usleep(50000);
	
	//Open gpio direction.
	gpio_fd = open("/sys/class/gpio/gpio"_RPI_NANO_GPIO_RESET"/direction", O_WRONLY);
	if(gpio_fd == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	//Set to output
	if(write(gpio_fd, "out", 3) == -1)
	{
		close(gpio_fd);
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	//Close gpio direction
	if(close(gpio_fd) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	return 0;
}

int _rpi_nano_gpio_reset_unexport_output()
{
	//Open gpio direction.
	int gpio_fd = open("/sys/class/gpio/gpio"_RPI_NANO_GPIO_RESET"/direction", O_WRONLY);
	if(gpio_fd == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	//Set to output
	if(write(gpio_fd, "in", 2) == -1)
	{
		close(gpio_fd);
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	//Close gpio direction
	if(close(gpio_fd) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	//Open gpio unexport
	gpio_fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if(gpio_fd == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	//Unexport gpio
	if(write(gpio_fd, _RPI_NANO_GPIO_RESET, 3) == -1)
	{
		close(gpio_fd);
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	//Close gpio unexport
	if(close(gpio_fd) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	return 0;
}

int _rpi_nano_set_reg(int fd, char* reg, int val)
{
	//Create AT commend.
	char buf[_RPI_NANO_SIZE_BUF];
	fprintf(stderr, "AT%s=%.2x\n", reg, val);
	sprintf(buf, "AT%s=%.2x\n", reg, val);

	//Send AT commend.
	if(write(fd, buf, strlen(buf)) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	//Read response.
	if(_rpi_nano_read(fd, buf, _RPI_NANO_SIZE_BUF, _RPI_NANO_READ_TIMEOUT*2, -1) == -1)
		return -1;
	
	//Parse response.
	sscanf(buf, "%s", buf);
	
	//Extract the at commend on the value.
	char* bufVal = strchr(buf, '=');
	if(bufVal == NULL)
	{
		fprintf(stderr, "ERROR - %s: Bad response from ARM-nano.\n", __func__);
		return -1;
	}
	*bufVal = '\0';
	bufVal++;
	bufVal[2]= '\0';
	
	//Convert the value.
	int _val = 0;
	sscanf(bufVal, "%x", &_val);
		
		
	if(_val == -1 || _val!=val)
	{
		fprintf(stderr, "ERROR - %s: The value didn't change.\n", __func__);
		return -1;
	}
	
	return 0;
}

int _rpi_nano_get_reg(int fd, char* reg, int* val)
{
	//Create AT commend.
	char buf[_RPI_NANO_SIZE_BUF];
	sprintf(buf, "AT%s\n", reg);

	//Send AT commend.
	if(write(fd, buf, strlen(buf)) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	//Read response.
	if(_rpi_nano_read(fd, buf, _RPI_NANO_SIZE_BUF, -1, -1) == -1)
		return -1;
	
	////Parse response.
	//sscanf(buf, "%s", buf);
	
	//Extract the at commend on the value.
	char* bufVal = strchr(buf, '=');
	if(bufVal == NULL)
	{
		fprintf(stderr, "ERROR - %s: Bad response from ARM-nano.\n", __func__);
		return -1;
	}
	*bufVal = '\0';
	bufVal++;
	bufVal[2]= '\0';
	
	//Convert the value.
	sscanf(bufVal, "%x", val);
		
	return 0;
}

//! \bug Le premier caractère envoyer est bizarre (seulement sur le Raspberry pi ?)
int rpi_nano_init(	const char* path,
					rpi_nano_uart_baudrate_e baudrate,
					rpi_nano_uart_parity_e parity,
					rpi_nano_uart_stopbit_e stopbit)
{
	//Gpio reset in output.
	if(_rpi_nano_gpio_reset_export_output() == -1)
	{
		return -1;
	}
	if(rpi_nano_enable_reset() == -1)
	{
		_rpi_nano_gpio_reset_unexport_output();
		return -1;
	}
	
	//Open serial port
	int rpi_nano_fd;
	if(path != NULL)
		rpi_nano_fd = open(path, O_RDWR);
	else
		rpi_nano_fd = open("/dev/ttyAMA0", O_RDWR);
	
	if(rpi_nano_fd == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	//Set parameter for configure the uart
	if(_rpi_nano_set_fd_parameter(rpi_nano_fd, baudrate, parity, stopbit) == -1)
	{
		close(rpi_nano_fd);
		return -1;
	}
	
	//Enable the module.
	if(rpi_nano_disable_reset() == -1)
	{
		_rpi_nano_gpio_reset_unexport_output();
		close(rpi_nano_fd);
		return -1;
	}
	
	//Booting
	sleep(4);
	//char buf[_RPI_NANO_SIZE_BUF];
	//_rpi_nano_read(rpi_nano_fd, buf, _RPI_NANO_SIZE_BUF, _RPI_NANO_SIZE_BUF, -1);
	if (tcflush(rpi_nano_fd,TCIFLUSH) == -1){
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	return rpi_nano_fd;
}

int rpi_nano_close(int fd)
{
	if(close(fd) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	return 0;
}

rpi_nano_uart_mode_e rpi_nano_get_mode(int fd)
{
	int err = 0;
	RPI_NANO_AT_MODE_ON();
	
	//Get mode
	rpi_nano_uart_mode_e mode = 0;
	err = _rpi_nano_get_reg(fd, "S000", (int*)&mode);
	
	RPI_NANO_AT_MODE_OFF();
	return err?(unsigned int)-1:mode;
}

int rpi_nano_set_mode(int fd, rpi_nano_uart_mode_e mode)
{
	int err = 0;
	RPI_NANO_AT_MODE_ON();
	
	//set mode
	err = _rpi_nano_set_reg(fd, "S000", (int)mode);
	
	RPI_NANO_AT_MODE_OFF();
	return err;
}

rpi_nano_version_e rpi_nano_get_version(int fd)
{
	RPI_NANO_AT_MODE_ON();
		
	//Send AT commend.
	if(write(fd, "ATV\n", 4) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__,  strerror(errno));
		RPI_NANO_AT_MODE_OFF();
		return -1;
	}
	
	//Read response.
	char buf[_RPI_NANO_SIZE_BUF];
	if(_rpi_nano_read(fd, buf, _RPI_NANO_SIZE_BUF, -1, -1) == -1)
	{
		fprintf(stderr, "ERROR - %s: Can't read correctly.\n", __func__);
		RPI_NANO_AT_MODE_OFF();
		return -1;
	}
	
	RPI_NANO_AT_MODE_OFF();
	
	//Parse response.
	char* bufPower = strrchr(buf, ' ');
	if(bufPower == NULL)
	{
		fprintf(stderr, "ERROR - %s: Bad response from ARM-nano.\n", __func__);
		return -1;
	}
	*bufPower='\0';
	bufPower++;
	bufPower[5] = '\0';
	char* bufFrequency = strrchr(buf, ' ');
	if(bufFrequency == NULL)
	{
		fprintf(stderr, "ERROR - %s: Bad response from ARM-nano.\n", __func__);
		return -1;
	}
	bufFrequency++;

	//Read response.
	if(strcmp(bufFrequency, "868MHZ") == 0)
	{
		if(strcmp(bufPower, "27DBM") == 0)
			return RPI_NANO_VERSION_N8LD;
		return RPI_NANO_VERSION_N8LP;
	}
	
	RPI_NANO_AT_MODE_OFF();
	return RPI_NANO_VERSION_N4;
}

unsigned int rpi_nano_get_sigfox_id(int fd)
{
	RPI_NANO_AT_MODE_ON();
	
	//Create AT commend.
	char buf[_RPI_NANO_SIZE_BUF]="AT$V01\n";

	//Send AT commend.
	if(write(fd, buf, strlen(buf)) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		RPI_NANO_AT_MODE_OFF();
		return 0;
	}
	
	//Read response.
	if(_rpi_nano_read(fd, buf, _RPI_NANO_SIZE_BUF, -1, -1) == -1)
	{
		RPI_NANO_AT_MODE_OFF();
		return 0;
	}
	RPI_NANO_AT_MODE_OFF();
		
	//Parse and check response.
	unsigned int id;
	if(sscanf(buf, "%x", &id) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return 0;
	}
	
	return id;
}

int rpi_nano_send_data(int fd, char const* data, ssize_t size)
{
	//Send data.
	if(write(fd, data, size) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	return 0;
}

int rpi_nano_enable_reset()
{
	//Open gpio
	int gpio_fd = open("/sys/class/gpio/gpio"_RPI_NANO_GPIO_RESET"/value", O_WRONLY);
		
	if(gpio_fd == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	//Set gpio to '0'.
	if(write(gpio_fd, "0", 1) == -1)
	{
		close(gpio_fd);
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	//Close gpio
	if(close(gpio_fd) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	return 0;
}

int rpi_nano_disable_reset()
{
	//Open gpio
	int gpio_fd = open("/sys/class/gpio/gpio"_RPI_NANO_GPIO_RESET"/value", O_WRONLY);
		
	if(gpio_fd == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	//Set gpio to '1'.
	if(write(gpio_fd, "1", 1) == -1)
	{
		close(gpio_fd);
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	//Close gpio
	if(close(gpio_fd) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	//Wait for boot
	usleep(_RPI_NANO_TIME_BOOT);
	
	return 0;
}

int rpi_nano_reset_is_enable()
{
	//Open gpio
	int gpio_fd = open("/sys/class/gpio/gpio"_RPI_NANO_GPIO_RESET"/value", O_RDONLY);
		
	if(gpio_fd == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	//read gpio.
	char buf;
	if(read(gpio_fd, &buf, 1) == -1)
	{
		close(gpio_fd);
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	//Close gpio
	if(close(gpio_fd) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	return buf=='0'?1:0;
}

int rpi_nano_get_uart_config(	int fd,
								rpi_nano_uart_baudrate_e* baudrate,
								rpi_nano_uart_parity_e* parity,
								rpi_nano_uart_stopbit_e* stopbit)
{
	RPI_NANO_AT_MODE_ON();
	
	//Get baudrate
	if(_rpi_nano_get_reg(fd, "S012", (int*)baudrate) == -1)
	{
		RPI_NANO_AT_MODE_OFF();
		return -1;
	}
		
	//Get parity
	*parity = RPI_NANO_UART_PARITY_NO;
	int _parity = 0x00;
	if(_rpi_nano_get_reg(fd, "S014", &_parity) == -1)
	{
		RPI_NANO_AT_MODE_OFF();
		return -1;
	}
		
	if(_parity == 0x01)//PARITY_ODD_ENABLE
		*parity = RPI_NANO_UART_PARITY_ODD;
	else if(_parity == 0x03)// RPI_NANO_UART_PARITY_EVEN
		*parity = RPI_NANO_UART_PARITY_NO;
		
	//Get stopBit
	if(_rpi_nano_get_reg(fd, "S015", (int*)stopbit) == -1)
	{
		RPI_NANO_AT_MODE_OFF();
		return -1;
	}
	
	RPI_NANO_AT_MODE_OFF();
	return 0;
}
						
int rpi_nano_set_uart_config(	int fd,
								rpi_nano_uart_baudrate_e baudrate,
								rpi_nano_uart_parity_e parity,
								rpi_nano_uart_stopbit_e stopbit)
{	
	RPI_NANO_AT_MODE_ON();
	
	//Set baudrate
	if(_rpi_nano_set_reg(fd, "S012", (int)baudrate) == -1)
	{
		RPI_NANO_AT_MODE_OFF();
		return -1;
	}

	//Set parity
	int _parity = 0x00;
	switch(parity)
	{
		case RPI_NANO_UART_PARITY_NO:
		_parity = 0x00;
		break;
		
		case RPI_NANO_UART_PARITY_ODD:
		_parity = 0x01;
		break;
		
		case RPI_NANO_UART_PARITY_EVEN:
		_parity = 0x03;
		break;
	}
	if(_rpi_nano_set_reg(fd, "S014", _parity) == -1)
	{
		RPI_NANO_AT_MODE_OFF();
		return -1;
	}
	
	//Set stopBit
	if(_rpi_nano_set_reg(fd, "S015", (int)stopbit) == -1)
	{
		RPI_NANO_AT_MODE_OFF();
		return -1;
	}
	
	RPI_NANO_AT_MODE_OFF();
		
	if(_rpi_nano_set_fd_parameter(fd, baudrate, parity, stopbit) == -1)
	{
		return -1;
	}
	
	return 0;
}

double rpi_nano_channel_to_frequency(int channel)
{
	if(channel < 1 || channel > 560)
		return -1;
		
	return 0.0125*channel+863;
}

int rpi_nano_frequency_to_channel(double frequency)
{
	if(frequency < 863.0125 || frequency > 870)
		return -1;
		
	return (frequency-863)/0.0125;
}

int rpi_nano_set_radio_baudrate(int fd, rpi_nano_radio_baudrate_e bps)
{
	int err = 0;
	RPI_NANO_AT_MODE_ON();
	
	//Set baudrate
	err = _rpi_nano_set_reg(fd, "S008", bps);
	
	RPI_NANO_AT_MODE_OFF();
	return err;
}

rpi_nano_radio_baudrate_e rpi_nano_get_radio_baudrate(int fd)
{
	int err = 0;
	RPI_NANO_AT_MODE_ON();
	
	//Get baudrate
	int bps = -1;
	err = _rpi_nano_get_reg(fd, "S008", &bps);
	
	RPI_NANO_AT_MODE_OFF();
	return err?-1:bps;
}

int rpi_nano_set_channel(int fd, int channel)
{
	RPI_NANO_AT_MODE_ON();
	
	if(channel > 800)
	{
		fprintf(stderr, "ERROR - %s: The channel number is invalid.\n", __func__);
		return -1;
	}
	
	if(_rpi_nano_set_reg(fd, "S002", (channel&0xff)))
	{
		RPI_NANO_AT_MODE_OFF();
		return -1;
	}
	if(_rpi_nano_set_reg(fd, "S003", (channel>>8)&0xff))
	{
		RPI_NANO_AT_MODE_OFF();
		return -1;
	}
		
	RPI_NANO_AT_MODE_OFF();
	return 0;
}
	
int rpi_nano_get_channel(int fd)
{
	RPI_NANO_AT_MODE_ON();
	
	int S002Val = 0;
	int S003Val = 0;
	if(_rpi_nano_get_reg(fd, "S002", &S002Val) == -1)
	{
		RPI_NANO_AT_MODE_OFF();
		return -1;
	}
	if(_rpi_nano_get_reg(fd, "S003", &S003Val) == -1)
	{
		RPI_NANO_AT_MODE_OFF();
		return -1;
	}
		
	RPI_NANO_AT_MODE_OFF();	
	return ((S003Val<<8)&0xff00)|(S002Val&0xff);
}

rpi_nano_radio_power_e rpi_nano_get_max_power(int fd)
{
	RPI_NANO_AT_MODE_ON();
	rpi_nano_version_e version = rpi_nano_get_version(fd);
	RPI_NANO_AT_MODE_OFF();
	
	switch(version)
	{
		case RPI_NANO_VERSION_N4:
		case RPI_NANO_VERSION_N8LP:
		return RPI_NANO_POWER_14dBm;
		break;
		
		case RPI_NANO_VERSION_N8LD:
		return RPI_NANO_POWER_27dBm;
		break;
	}
	
	return -1;
}

rpi_nano_radio_power_e rpi_nano_get_min_power()
{
	return _RPI_NANO_MIN_POWER;
}

rpi_nano_radio_power_e rpi_nano_get_power(int fd)
{
	int err = -1;
	RPI_NANO_AT_MODE_ON();
	
	int val = -1;
	err = _rpi_nano_get_reg(fd, "S004", &val);
		
	RPI_NANO_AT_MODE_OFF();
	return err?-1:val;
}

int rpi_nano_set_power(int fd, rpi_nano_radio_power_e power)
{
	int err = -1;
	RPI_NANO_AT_MODE_ON();
	
	err = _rpi_nano_set_reg(fd, "S004", power);
	
	RPI_NANO_AT_MODE_OFF();
	return err;
}

int rpi_nano_get_modem_address(int fd)
{
	int err = -1;
	RPI_NANO_AT_MODE_ON();
	
	int val = -1;
	err = _rpi_nano_get_reg(fd, "S097", &val);
		
	RPI_NANO_AT_MODE_OFF();
	return err?-1:val;
}

int rpi_nano_set_modem_address(int fd, int address)
{
	int err = -1;
	RPI_NANO_AT_MODE_ON();
	
	err = _rpi_nano_set_reg(fd, "S097", address);
	
	RPI_NANO_AT_MODE_OFF();
	return err;
}

int rpi_nano_get_dest_modem_address(int fd)
{
	int err = -1;
	RPI_NANO_AT_MODE_ON();
	
	int val = -1;
	err = _rpi_nano_get_reg(fd, "S090", &val);
		
	RPI_NANO_AT_MODE_OFF();
	return err?-1:val;
}

int rpi_nano_set_dest_modem_address(int fd, int address)
{
	int err = -1;
	RPI_NANO_AT_MODE_ON();
	
	err = _rpi_nano_set_reg(fd, "S090", address);
	
	RPI_NANO_AT_MODE_OFF();
	return err;
}

rpi_nano_radio_power_e rpi_nano_available_max_power(int fd)
{
	return -1;
}

rpi_nano_radio_power_e rpi_nano_available_max_power_p(
									rpi_nano_radio_baudrate_e bps,
									int channel)
{
	if(	bps == RPI_NANO_RADIO_BAUDRATE_R1200_2GFSK ||
		bps == RPI_NANO_RADIO_BAUDRATE_R2400_2GFSK)
	{
		if(channel >= 513 && channel <= 531)
			return RPI_NANO_POWER_27dBm;
			
		return RPI_NANO_POWER_14dBm;
	}
	else if(	bps == RPI_NANO_RADIO_BAUDRATE_R1200_2GFSK ||
				bps == RPI_NANO_RADIO_BAUDRATE_R2400_2GFSK)
	{
		if(channel >= 513 && channel <= 531)
			return RPI_NANO_POWER_27dBm;
			
		return RPI_NANO_POWER_14dBm;
	}
	
	return -1;
}

int rpi_nano_start_ping_pong(int fd, rpi_nano_mode_ping_pong_e mode)
{
	RPI_NANO_AT_MODE_ON();
	
	//Create AT commend.
	char buf[_RPI_NANO_SIZE_BUF];
	
	if(mode == RPI_NANO_MODE_PING_PONG_MASTER)
		strcpy(buf, "ATT00\n");
	else
		strcpy(buf, "ATT01\n");

	//Send AT commend.
	if(write(fd, buf, strlen(buf)) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		RPI_NANO_AT_MODE_OFF();
		return -1;
	}
	
	//Read response.
	if(_rpi_nano_read(fd, buf, _RPI_NANO_SIZE_BUF, -1, -1) == -1)
	{
		RPI_NANO_AT_MODE_OFF();
		return -1;
	}
		
	//Parse and check response.
	if(strstr(buf, "Entering in ping-pong mode") == NULL)
	{
		fprintf(stderr, "ERROR - %s: Bad response from ARM-nano.\n", __func__);
		RPI_NANO_AT_MODE_OFF();
		return -1;
	}
	
	return 0;
}

int rpi_nano_status_ping_pong(int fd, char rssiResult[], ssize_t sizeRssi, char berResult[], ssize_t sizeBer, char perResult[], ssize_t sizePer )
{
	
	//Check the size status
	if(sizeRssi < 16)
	{
		fprintf(stderr, "ERROR - %s: The size of your 'rssiResult' is too small.\n", __func__);
		return -1;
	}else if(sizeBer<10){
		fprintf(stderr, "ERROR - %s: The size of your 'berResult' is too small.\n", __func__);
		return -1;
	}else if(sizePer<10){
		fprintf(stderr, "ERROR - %s: The size of your 'perResult' is too small.\n", __func__);
		return -1;		
	}
	
	//Read all.
	char buf[_RPI_NANO_SIZE_BUF*2*2+1];
	char buf0[_RPI_NANO_SIZE_BUF*2+1];
	char buf1[_RPI_NANO_SIZE_BUF*2+1];
	char* bufptr = buf0;
	int n0 = 0;
	int n1 = 0;
	int n = 0;
	
	//First timeout 2s.
	int timeout = 2000000;
	
	while(1)
	{
		n = _rpi_nano_read(fd, bufptr, _RPI_NANO_SIZE_BUF*2, timeout, -1);
		timeout = _RPI_NANO_READ_TIMEOUT;
		
		if(n==0)
			break;
			
		bufptr[n] = '\0';
		
		//Switch buffer
		if(bufptr == buf0)
		{
			n0 = n;
			bufptr = buf1;
			
			memcpy(buf, buf1, n1);
			memcpy(buf+n1, buf0, n0);
		}
		else
		{
			n1 = n;
			bufptr = buf0;
			
			memcpy(buf, buf0, n0);
			memcpy(buf+n0, buf1, n1);
		}
	}
	//Parse the last test.
	char* per = strstr(buf, "PER:");
	char* ber = strstr(buf, "BER:");
	char* rssi = strstr(buf, "RSSI:");
	char* dbm1 ;
	char* dbm2;
	if (rssi != NULL){
		dbm1 = strstr(rssi, "dBm/");
		dbm2 = strstr(dbm1+3, "dBm");
	}else{
		dbm1=NULL;
		dbm2=NULL;
	}
	if ((per != NULL) && (perResult != NULL)){
		strncpy(perResult, per+4, 9);
		perResult[9] = '\0';
	}else{
		perResult[0] = '\0';
	}
	if ((ber != NULL) && (berResult != NULL)){
		strncpy(berResult, ber+4, 9);
		berResult[9] = '\0';
	}else{
		berResult[0] = '\0';
	}
	if ((rssi != NULL) && (rssiResult != NULL)){
		printf("%i\n",(dbm2+3)-(rssi+5));
		strncpy(rssiResult, rssi+5, (dbm2+3)-(rssi+5));
		rssiResult[(dbm2+3)-(rssi+5)] = '\0';
	}else{
		rssiResult[0] = '\0';
	}
	printf("%s %s %s\n", perResult, berResult, rssiResult);
	return 0;
}

int rpi_nano_stop_ping_pong(int fd)
{
	//Send stop commend.
	if(write(fd, "a", 1) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}

	//Read all.
	char buf[_RPI_NANO_SIZE_BUF*2+1];
	char buf0[_RPI_NANO_SIZE_BUF+1];
	char buf1[_RPI_NANO_SIZE_BUF+1];
	char* bufptr = buf0;
	int n0 = 0;
	int n1 = 0;
	int n = 0;
	
	while(1)
	{
		//First timeout 4s
		n = _rpi_nano_read(fd, bufptr, _RPI_NANO_SIZE_BUF, 4000000, -1);
		
		if(n==0)
			break;
			
		bufptr[n] = '\0';
		
		if((n == 1) && (bufptr[0] == 'A'))
			break;

		//Switch buffer
		if(bufptr == buf0)
		{
			n0 = n;
			bufptr = buf1;
			
			memcpy(buf, buf1, n1);
			memcpy(buf+n1, buf0, n0);
		}
		else
		{
			n1 = n;
			bufptr = buf0;
			
			memcpy(buf, buf0, n0);
			memcpy(buf+n0, buf1, n1);
		}
		
		//Parse response.
		buf[n0+n1] = '\0';
		if(strstr(buf, "Test mode stopped") != NULL)
		{
			RPI_NANO_AT_MODE_OFF();
			return 0;
		}
			
	}
	
	fprintf(stderr, "ERROR - %s: Bad response from ARM-nano.\n", __func__);
	return -1;
}

int rpi_nano_rssi(int fd)
{
	int rrssi = -1;
	
	RPI_NANO_AT_MODE_ON();
	
	//Create AT commend.
	char buf[_RPI_NANO_SIZE_BUF];
	strcpy(buf, "ATT0A\n");

	//Send AT commend.
	if(write(fd, buf, strlen(buf)) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		RPI_NANO_AT_MODE_OFF();
		return -1;
	}
	
	//Read response.
	if(_rpi_nano_read(fd, buf, _RPI_NANO_SIZE_BUF, _RPI_NANO_READ_TIMEOUT, -1) == -1)
	{
		RPI_NANO_AT_MODE_OFF();
		return -1;
	}
		
	//Parse and check response.
	if(strstr(buf, "Entering in spectrum-analyser mode") == NULL)
	{
		fprintf(stderr, "ERROR - %s: Bad response from ARM-nano.\n", __func__);
		RPI_NANO_AT_MODE_OFF();
		return -1;
	}
	
	//Read response.
	if(_rpi_nano_read(fd, buf, _RPI_NANO_SIZE_BUF, _RPI_NANO_READ_TIMEOUT*50, -1) == -1)
	{
		RPI_NANO_AT_MODE_OFF();
		return -1;
	}
	
	//Parse Rssi value.
	char* ptrbuf = strstr(buf, "RSSI:");
	sscanf(ptrbuf, "RSSI:%idBm", &rrssi);

	
	//Send stop commend.
	if(write(fd, "a", 1) == -1)
	{
		fprintf(stderr, "ERROR - %s: %s\n", __func__, strerror(errno));
		return -1;
	}
	
	//Read response for clear.
	if(_rpi_nano_read(fd, buf, _RPI_NANO_SIZE_BUF, -1, -1) == -1)
	{
		RPI_NANO_AT_MODE_OFF();
		return -1;
	}
	
	RPI_NANO_AT_MODE_OFF();
	
	return rrssi;
}
