//! \file **************************************************************
//!
//! \brief Header for controlling the ARM-nano.
//!
//! \author Atim.
//! \version 0.1
//! \date 03.07.2014
//!
//! ********************************************************************

#ifndef RPI_NANO_H
#define RPI_NANO_H

//! \addtogroup api_ref_group API reference
//!
//! You can use the functions in this section to communicate with the
//! \b ARM-nano.\n
//! You can read first \ref start_page and/or \ref exa_page .
//! \{

// -- DEFINE -----------------------------------------------------------

// -- ENUM -------------------------------------------------------------
//! \brief The ARM-NANO version.
typedef enum
{
	RPI_NANO_VERSION_N4						= 0x00,	//!< ARM-nano N4 433MHz
	RPI_NANO_VERSION_N8LP					= 0x01,	//!< ARM-nano N8LP 868MHz low power.
	RPI_NANO_VERSION_N8LD					= 0x02	//!< ARM-nano N8LD 868MHz long distance.
}rpi_nano_version_e;

//! \brief The operating mode.
typedef enum
{
	RPI_NANO_MODE_SEND_TO_LOCAL				= 0x10,	//!< Mode for send data to local network.
	RPI_NANO_MODE_SEND_TO_SIGFOX			= 0x50	//!< Mode for send data to sigfox network.
}rpi_nano_uart_mode_e;

//! \brief The possible values for uart baudrate.
typedef enum
{
	RPI_NANO_UART_BAUDRATE_1200				= 0x00,	//!< baudrate 1200 bps
	RPI_NANO_UART_BAUDRATE_2400				= 0x01,	//!< baudrate 2400 bps
	RPI_NANO_UART_BAUDRATE_4800				= 0x02,	//!< baudrate 4800 bps
	RPI_NANO_UART_BAUDRATE_9600				= 0x03,	//!< baudrate 9600 bps
	RPI_NANO_UART_BAUDRATE_19200			= 0x04,	//!< baudrate 19200 bps
	RPI_NANO_UART_BAUDRATE_38400			= 0x05,	//!< baudrate 38400 bps
	RPI_NANO_UART_BAUDRATE_57600			= 0x06,	//!< baudrate 57600 bps
	RPI_NANO_UART_BAUDRATE_115200			= 0x07,	//!< baudrate 115200 bps
	RPI_NANO_UART_BAUDRATE_230400			= 0x08	//!< baudrate 230400 bps
}rpi_nano_uart_baudrate_e;

//! \brief The possible values for uart parity.
typedef enum
{
	RPI_NANO_UART_PARITY_NO					= 0x00,	//!< No parity
	RPI_NANO_UART_PARITY_ODD				= 0x01,	//!< Odd parity
	RPI_NANO_UART_PARITY_EVEN				= 0x02	//!< Even parity
}rpi_nano_uart_parity_e;

//! \brief The possible values for uart stop bit.
typedef enum
{
	RPI_NANO_UART_STOPBIT_1					= 0x01,	//!< 1 stop bit
	RPI_NANO_UART_STOPBIT_2					= 0x02	//!< 2 stop bit
}rpi_nano_uart_stopbit_e;

//! \brief The possible values for radio baudrate.
typedef enum
{
	RPI_NANO_RADIO_BAUDRATE_UNDEFINED		= 0xff,	//!< baudrate undefined
	RPI_NANO_RADIO_BAUDRATE_R1200_2GFSK		= 0x01,	//!< baudrate 1200 bps in 2GFSK
	RPI_NANO_RADIO_BAUDRATE_R2400_2GFSK		= 0x02,	//!< baudrate 2400 bps in 2GFSK
	RPI_NANO_RADIO_BAUDRATE_R4800_2GFSK   	= 0x03,	//!< baudrate 4800 bps in 2GFSK
	RPI_NANO_RADIO_BAUDRATE_R9600_4GFSK   	= 0x04,	//!< baudrate 9600 bps in 4GFSK
	RPI_NANO_RADIO_BAUDRATE_R19200_4GFSK  	= 0x05,	//!< baudrate 19200 bps in 4GFSK
	RPI_NANO_RADIO_BAUDRATE_R38400_4GFSK  	= 0x06,	//!< baudrate 38400 bps in 4GFSK
	RPI_NANO_RADIO_BAUDRATE_R57600_4GFSK  	= 0x07,	//!< baudrate 57600 bps in 4GFSK
	RPI_NANO_RADIO_BAUDRATE_R115200_4GFSK  	= 0x08	//!< baudrate 115200 bps in 4GFSK
}rpi_nano_radio_baudrate_e;

//! \brief The possible values for radio power.
//! \note The values in mW are approximative.
typedef enum
{
	RPI_NANO_POWER_0dBm       				= 0x01,		//!< 0 dBm	-> 1 mW	
	RPI_NANO_POWER_5dBm       				= 0x02,		//!< 5 dBm	-> 3 mW	
	RPI_NANO_POWER_7dBm       				= 0x03,		//!< 7 dBm	-> 5 mW
	RPI_NANO_POWER_10dBm       				= 0x04,		//!< 10 dBm	-> 10 mW
	RPI_NANO_POWER_12dBm       				= 0x05,		//!< 12 dBm	-> 16 mW
	RPI_NANO_POWER_14dBm       				= 0x06,		//!< 14 dBm	-> 25 mW
	RPI_NANO_POWER_20dBm       				= 0x07,		//!< 20 dBm	-> 100 mW
	RPI_NANO_POWER_23dBm       				= 0x08,		//!< 23 dBm	-> 200 mW
	RPI_NANO_POWER_27dBm       				= 0x09		//!< 27 dBm	-> 500 mW
}rpi_nano_radio_power_e;

//! \brief Mode for the ping pong test.
typedef enum
{
	RPI_NANO_MODE_PING_PONG_MASTER       	= 0x00,		//!< Master mode
	RPI_NANO_MODE_PING_PONG_SLAVE       	= 0x01		//!< Salve mode
}rpi_nano_mode_ping_pong_e;
  
  
// -- FUNCTION ---------------------------------------------------------

//! \brief Initialise the uart to communicate with ARM-nano module.
//!
//! This function open the uart port and configure this 
//!
//! \param path path to uart device. Generally on the \b RPI the path is
//! "/dev/ttyAMA0". If you pass \b NULL, the path "/dev/ttyAMA0" will be used.
//! \param baudrate baudrate for uart of Raspberry pi. The default value
//! from ARM-nano is 9600bps, \ref RPI_NANO_UART_BAUDRATE_9600 will be used if
//! you don't change this parameter on you ARM-nano.
//! \param parity parity for uart of Raspberry pi. By default there is no
//! parity from ARM-nano, \ref RPI_NANO_UART_PARITY_NO willo be used if
//! you don't change this parameter on you ARM-nano.
//! \param stopbit stopbit for uart of Raspberry pi. By default there is
//! 1 stop bit from ARM-nano, \ref RPI_NANO_UART_STOPBIT_1 will be used if
//! you don't change this parameter on you ARM-nano.
//!
//! \return -1 if error and a file descriptor if success. The error is
//! print on standard error output.
//! \see rpi_nano_close()
//! \see rpi_nano_set_uart_config()
int rpi_nano_init(	const char* path,
					rpi_nano_uart_baudrate_e baudrate,
					rpi_nano_uart_parity_e parity,
					rpi_nano_uart_stopbit_e stopbit);

//! \brief Close the uart port.
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \return -1 if error and 0 if success. The error is
//! print on standard error output.
//! \see rpi_nano_init()
int rpi_nano_close(int fd);

//! \brief Get the operating mode.
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \return -1 if error or the mode if success. The error is
//! print on standard error output.
//! \see rpi_nano_init()
rpi_nano_uart_mode_e rpi_nano_get_mode(int fd);

//! \brief Set the operating mode.
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \param mode the mode.
//! \return -1 if error and 0 if success. The error is
//! print on standard error output.
//! \see rpi_nano_init()
int rpi_nano_set_mode(int fd, rpi_nano_uart_mode_e mode);

//! \brief Get version of ARM-nano.
//!
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \return -1 if error or the version if success. The error is
//! print on standard error output.		
rpi_nano_version_e rpi_nano_get_version(int fd);

//! \brief Reboot the ARM-nano.
//!
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \return -1 if error and 0 if success. The error is
//! print on standard error output.
int rpi_nano_reboot(int fd);

//! \brief Send data to network.
//!
//! When you are in \ref RPI_NANO_MODE_SEND_TO_LOCAL mode, the data is 
//! send to local network and when you are
//! \ref rpi_nano_send_data_TO_SIGFOX mode, the data is send to sigfox
//! network. With sigfox the data are cut in 12 bytes.
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \param data the data to send.
//! \param size the size in byte of data.
//! \return -1 if error and 0 if success. The error is
//! print on standard error output.
//! \see rpi_nano_get_mode()
int rpi_nano_send_data(int fd, char const* data, ssize_t size);

//! \brief Get uart configuration
//!
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \param baudrate baudrate from ARM-nano.
//! \param parity parity from ARM-nano.
//! \param stopbit stopbit from ARM-nano.
//!
//! \return -1 if error and 0 if success. The error is
//! print on standard error output.
//! \see rpi_nano_set_uart_config()
int rpi_nano_get_uart_config(	int fd,
								rpi_nano_uart_baudrate_e* baudrate,
								rpi_nano_uart_parity_e* parity,
								rpi_nano_uart_stopbit_e* stopbit);
								
//! \brief Set uart configuration
//!
//! \attention The parameters are saved in ARM-nano. So if you switch off 
//! the ARM-nano  and switch on later, the parameters are still active.
//! When you call the \ref rpi_nano_init() function, you need to specify the
//! new parameters.
//!
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \param baudrate baudrate for uart of Raspberry pi and ARM-nano.
//! \param parity parity for uart of Raspberry pi and ARM-nano.
//! \param stopbit stopbit for uart of Raspberry pi and ARM-nano.
//!
//! \return -1 if error and 0 if success. The error is
//! print on standard error output.
//! \see rpi_nano_get_uart_config()
int rpi_nano_set_uart_config(	int fd,
								rpi_nano_uart_baudrate_e baudrate,
								rpi_nano_uart_parity_e parity,
								rpi_nano_uart_stopbit_e stopbit);
								
//! \brief Converter channel to frequency.
//! \return -1 if the bad channel and the frequency if success.
//! \todo Revoire cette fonction pour le N8.
double rpi_nano_channel_to_frequency(int channel);
								
//! \brief Converter frequency to channel.
//! \return -1 if the bad frequency and the channel if success.
//! \todo Revoire cette fonction pour le N8.
int rpi_nano_frequency_to_channel(double frequency);

//----------------------------------------------------------------------

//! \name Reset
//!
//! If you want use the ARM-nano module, you need disable reset in call
//! \ref rpi_nano_disable_reset(). This function is already call in the 
//! \ref rpi_nano_init(). Normally you shouldn't use the reset functions.
//! \{

//! \brief Enable reset (use the gpio pin).
//!
//! \return -1 if error and 0 if success. The error is
//! print on standard error output.
int rpi_nano_enable_reset();

//! \brief Disable reset (use the gpio pin).
//!
//! \return -1 if error and 0 if success. The error is
//! print on standard error output.
int rpi_nano_disable_reset();

//! \brief Reset is enable?
//!
//! \return 0 if disable, 1 if enable and -1 if error. The error is
//! print on standard error output.
int rpi_nano_reset_is_enable();

//! \} //Reset

//----------------------------------------------------------------------

//! \name Sigfox network
//! \{

//! \brief Get the sigfox id from ARM-nano.
//!
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \return 0 if error or id value if success. The error is
//! print on standard error output.
unsigned int rpi_nano_get_sigfox_id(int fd);

//! \} //Sigfox network

//----------------------------------------------------------------------

//! \name Local network
//! \{


//! \brief Set radio baudrate for local network.
//!
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \param bps the radio baudrate.
//! \return -1 if error and 0 if success. The error is
//! print on standard error output.
int rpi_nano_set_radio_baudrate(int fd, rpi_nano_radio_baudrate_e bps);

//! \brief Get radio baudrate for local network.
//!
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \return -1 if error or the radio baudrate if success. The error is
//! print on standard error output.
rpi_nano_radio_baudrate_e rpi_nano_get_radio_baudrate(int fd);

//! \brief Set channel number for local network to ARM-nano.
//!
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \param channel the channel number of 0 to 560 for the N8 and 0 to
//! 160 for the N4.
//! \return -1 if error and 0 if success. The error is
//! print on standard error output.
//! \todo revoir la documentation de cette fonction.
int rpi_nano_set_channel(int fd, int channel);

//! \brief Get channel number for local network from ARM-nano.
//!
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \return -1 if error or the channel number if success. The error is
//! print on standard error output.
int rpi_nano_get_channel(int fd);

//! \brief Get the maximal power ARM-nano.
//!
//! \note The returned value is not necessarily usable in your actual
//! configuration. This value is function from your arm-nano version.
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \return -1 if error or the power if success. The error is
//! print on standard error output.
rpi_nano_radio_power_e rpi_nano_get_max_power(int fd);

//! \brief Get the minimum power ARM-nano.
rpi_nano_radio_power_e rpi_nano_get_min_power();


//! \brief Get the power from ARM-nano.
//!
//! \note The returned value is not necessarily usable in your actual
//! configuration.
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \return -1 if error or the power if success. The error is
//! print on standard error output.
rpi_nano_radio_power_e rpi_nano_get_power(int fd);

//! \brief Set the power to ARM-nano.
//!
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \return -1 if error or 0 if success. The error is
//! print on standard error output.
int rpi_nano_set_power(int fd, rpi_nano_radio_power_e power);

//! \brief Get address of your ARM-nano modem.
//!
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \return -1 if error or the address if success (1 byte). The error is
//! print on standard error output.
int rpi_nano_get_modem_address(int fd);

//! \brief Set address of your ARM-nano modem.
//!
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \param address the new address (1 byte).
//! \return -1 if error and 0 if success. The error is
//! print on standard error output.
int rpi_nano_set_modem_address(int fd, int address);

//! \brief Get address of destination ARM-nano modem.
//!
//! The data send with \ref rpi_nano_send_data() (if you are local
//! mode \ref RPI_NANO_MODE_SEND_TO_LOCAL) are transmit to this address.
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \return -1 if error or the address if success (1 byte). The error is
//! print on standard error output.
int rpi_nano_get_dest_modem_address(int fd);

//! \brief Set address of destination ARM-nano modem.
//!
//! The data send with \ref rpi_nano_send_data() (if you are local
//! mode \ref RPI_NANO_MODE_SEND_TO_LOCAL) are transmit to this address.
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \param address the new address (1 byte).
//! \return -1 if error and 0 if success. The error is
//! print on standard error output.
int rpi_nano_set_dest_modem_address(int fd, int address);

//! \} //Local network

//----------------------------------------------------------------------

//! \name Available parameter
//! \todo is not implemented
//! \{

//! \brief 
rpi_nano_radio_power_e rpi_nano_available_max_power(int fd);
rpi_nano_radio_power_e rpi_nano_available_max_power_p(
									rpi_nano_radio_baudrate_e bps,
									int channel);
//! \} //Local network

//----------------------------------------------------------------------

//! \name Local network ping pong test
//! \{

//! \brief Start the test ping pong.
//!
//! \attention After call this function you should call 
//! \ref rpi_nano_status_ping_pong() or \ref rpi_nano_stop_ping_pong().
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \return -1 if error or 0 if success. The error is
//! print on standard error output.
int rpi_nano_start_ping_pong(int fd, rpi_nano_mode_ping_pong_e mode);

//! \brief Get the \b PER (Packet Error Rate) end \b BER (Bit error rate).
//!
//! \attention You must call this function only if the ping pong test is
//! started \ref rpi_nano_start_ping_pong(). You don't must call this
//! this function after call of \ref rpi_nano_status_ping_pong().
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \param rssiResult the string that will be written with 
//! the value of the rssi.
//! \param sizeRssi The size of your rssi string. It should be >=
//! 16.
//! \param berResult the string that will be written with 
//! the value of the ber.
//! \param sizeBer The size of your ber string. It should be >=
//! 10.
//! \param perResult the string that will be written with 
//! the value of the per.
//! \param sizePer The size of your per string. It should be >=
//! 10.
//! \return -1 if error or 0 if success. The error is
//! print on standard error output.
int rpi_nano_status_ping_pong(int fd, char rssiResult[], ssize_t sizeRssi, char berResult[], ssize_t sizeBer, char perResult[], ssize_t sizePer );

//! \brief Stop the ping pong test.
//!
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \return -1 if error or 0 if success. The error is
//! print on standard error output.
int rpi_nano_stop_ping_pong(int fd);

//! \} //Local network test ping pong

//! \name Other functions
//! \{

//! \brief Get RSSI value on concurrent channel.
//!
//! \param fd file descriptor returned from \ref rpi_nano_init().
//! \return -1 if error or the value in dBm.
int rpi_nano_rssi(int fd);

//! \} //Other functions

//! \}

#endif //RPI_NANO_H
