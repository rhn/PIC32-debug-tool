#include <p32xxxx.h>
#include <system.h>     // System setup
#include <const.h>      // MIPS32
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <newlib.h>
#include <errno.h>

// Config bits
#include <configBits.h>

// Drivers for HW
#include <GPIODrv.h>
#include <UARTDrv.h>
#include <BTN.h>
#include <LED.h>
// USB
#include <usb.h>
#include <usb_config.h>
#include <usb_ch9.h>
#include <usb_cdc.h>


volatile char tempArray[128];
volatile uint8_t lengthArray = 0;
#ifdef MULTI_CLASS_DEVICE
static uint8_t cdc_interfaces[] = { 0 };
#endif

// TODO - run the timer, like in MX440 example (SysTick style)
void simpleDelay(unsigned int noOfLoops){
    unsigned int i = 0;
    while (i<noOfLoops){
        i++;
        asm("nop");
    }
}

/// Results in a tick every 9765.625 Hz (CPU @ 20MHz, PBCLK @ 1/8)
void timer_init() {
	T2CON = 0;
	T3CON = 0;
	T2CON = 0x0078;
    TMR2 = 0;
    PR2 = 0xffffffff; // max period
    T2CON = 0x8078; // ON=1 | TCKPS=0b111 (1:256), use PBCLK, T32=timer3
}

uint32_t timer_read() {
	return TMR2;
}

void timer_reset() {
	timer_init();
}

// ~20 min in timer ticks
//const uint32_t timer_trigger = (20 * 60) * 9765.625;
const uint32_t timer_trigger = 10 * 60	 * 9765.625;

// One second to hold
const uint32_t timer_hold = 9765.625;

void setup(){
	// What is the equivalent of SYSTEMConfigPerformance?
	// -> Setting up the system for the required System Clock
	// -> Seting up the Wait States
	// -> Setting up PBCLK
	// -> Setting up Cache module (not presenf on MX1/2, but is on MX4)
	// Also of interest: https://microchipdeveloper.com/32bit:mx-arch-exceptions-processor-initialization
	// See Pic32 reference manual, for CP0 info http://ww1.microchip.com/downloads/en/devicedoc/61113e.pdf

	// DO NOT setup KSEG0 (cacheable area) on MX1/MX2, debugging will NOT work

	BMXCONbits.BMXWSDRM = 0;	// Set wait-states to 0
	
	// System config, call with desired CPU freq. and PBCLK divisor
#if defined (__32MX270F256D__)
	SystemConfig(48000000L, 1);	// Set to 48MHz, with PBCLK with divider 1 (same settings as DEVCFG)
#elif defined(__32MX440F256H__)
	SystemConfig(20000000L, 8);
#endif

	LED_init();
	BTN_init();
	timer_init();

	// Enable DMA. This was enabled during testing USB, TODO check.
	DMACONbits.ON = 1;

	// Enable interrupts
	INTEnableSystemMultiVectoredInt();

	// Copied for USB, from hardware.c
	// TODO, make generic, make proper.
#if defined (__32MX270F256D__)
	IPC7bits.USBIP = 4;
#elif defined(__32MX440F256H__)
	IPC11bits.USBIP = 4;
#endif


}

/*
static void send_string_sync(uint8_t endpoint, const char *str)
{
	char *in_buf = (char*) usb_get_in_buffer(endpoint);

	while (usb_in_endpoint_busy(endpoint)){
	}

	strcpy(in_buf, str);
	// Hack: Get the length from strlen(). This is inefficient, but it's
	// just a demo. strlen()'s return excludes the terminating NULL.
	usb_send_in_buffer(endpoint, strlen(in_buf));
}
*/

const char hex_tab[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'a', 'b', 'c', 'd', 'e', 'f'};

void to_string_16b(char buf[4], uint16_t data) {
	buf[3] = hex_tab[data & 0xf];
	buf[2] = hex_tab[(data >> 4) & 0xf];
	buf[1] = hex_tab[(data >> 8) & 0xf];
	buf[0] = hex_tab[(data >> 12) & 0xf];
}

void to_string_32b(char buf[8], uint32_t data) {
	to_string_16b(buf, data >> 16);
	to_string_16b(buf + 4, data);
}

int main(){
	bool needs_info = false;
	uint32_t last_check = 0;
	setup();
	usb_init();

	// A very basic USB-UART example.
	// Currently everything is hardcoded. Will be expanded later.

	for(;;){
		uint32_t time = timer_read();
		if (time > timer_trigger) {
			LED_setState(0);
		} else {
			LED_setState(1);
		}
		
		// Don't hold the reset wire forever, we may want to turn the device back on
		if (time > timer_trigger + timer_hold) {
			timer_reset();
		}

		// Send data to the PC if anything in buffer
		if (usb_is_configured() && !usb_in_endpoint_halted(2) && !usb_in_endpoint_busy(2)) {
			uint32_t i;
			uint8_t *buf = usb_get_in_buffer(2);
			if (needs_info) {
				const char *pre = "0x";
				strncpy(buf, pre, strlen(pre));
				to_string_32b(buf + strlen(pre), last_check);
				const char *post = "\r\n";
				strncpy(buf + strlen(pre) + 8, post, strlen(post));
				usb_send_in_buffer(2, strlen(pre) + 8 + strlen(post));
				needs_info = false;
			}
			/*
			// Send a zero-length packet if the transaction length was the same as the endpoint
			// length. This is for demo purposes. In real life, you only need to do this if the data
			// you're transferring ends on a multiple of the endpoint length.
			if (out_buf_len == EP_2_LEN) {
				// Wait until the IN endpoint can accept it
				while (usb_in_endpoint_busy(2)){
				}
				usb_send_in_buffer(2, 0);
			}
			*/
		}

		// Handle data received from the host
		if (usb_is_configured() && !usb_out_endpoint_halted(2) && usb_out_endpoint_has_data(2)) {
			needs_info = true;
			last_check = time;
			const unsigned char *out_buf;
			size_t out_buf_len;
			// Check for an empty transaction.
			out_buf_len = usb_get_out_buffer(2, &out_buf);
			if (out_buf_len > 0){
				// TODO: send out echo
				timer_reset();
			}
			usb_arm_out_endpoint(2);
		}

		#ifndef USB_USE_INTERRUPTS
		usb_service();
		#endif
	}

    return(0);
    
}




/////////////////////////////////////////////////////////////7


/* Callbacks. These function names are set in usb_config.h. */
void app_set_configuration_callback(uint8_t configuration)
{

}

uint16_t app_get_device_status_callback()
{
	return 0x0000;
}

void app_endpoint_halt_callback(uint8_t endpoint, bool halted)
{

}

int8_t app_set_interface_callback(uint8_t interface, uint8_t alt_setting)
{
	return 0;
}

int8_t app_get_interface_callback(uint8_t interface)
{
	return 0;
}

void app_out_transaction_callback(uint8_t endpoint)
{

}

void app_in_transaction_complete_callback(uint8_t endpoint)
{

}

int8_t app_unknown_setup_request_callback(const struct setup_packet *setup)
{
	/* To use the CDC device class, have a handler for unknown setup
	 * requests and call process_cdc_setup_request() (as shown here),
	 * which will check if the setup request is CDC-related, and will
	 * call the CDC application callbacks defined in usb_cdc.h. For
	 * composite devices containing other device classes, make sure
	 * MULTI_CLASS_DEVICE is defined in usb_config.h and call all
	 * appropriate device class setup request functions here.
	 */
	return process_cdc_setup_request(setup);
}

int16_t app_unknown_get_descriptor_callback(const struct setup_packet *pkt, const void **descriptor)
{
	return -1;
}

void app_start_of_frame_callback(void)
{

}

void app_usb_reset_callback(void)
{

}

/* CDC Callbacks. See usb_cdc.h for documentation. */

int8_t app_send_encapsulated_command(uint8_t interface, uint16_t length)
{
	return -1;
}

int16_t app_get_encapsulated_response(uint8_t interface,
                                      uint16_t length, const void **report,
                                      usb_ep0_data_stage_callback *callback,
                                      void **context)
{
	return -1;
}

int8_t app_set_comm_feature_callback(uint8_t interface,
                                     bool idle_setting,
                                     bool data_multiplexed_state)
{
	return -1;
}

int8_t app_clear_comm_feature_callback(uint8_t interface,
                                       bool idle_setting,
                                       bool data_multiplexed_state)
{
	return -1;
}

int8_t app_get_comm_feature_callback(uint8_t interface,
                                     bool *idle_setting,
                                     bool *data_multiplexed_state)
{
	return -1;
}

static struct cdc_line_coding line_coding =
{
	115200,
	CDC_CHAR_FORMAT_1_STOP_BIT,
	CDC_PARITY_NONE,
	8,
};

int8_t app_set_line_coding_callback(uint8_t interface,
                                    const struct cdc_line_coding *coding)
{
	line_coding = *coding;
	return 0;
}

int8_t app_get_line_coding_callback(uint8_t interface,
                                    struct cdc_line_coding *coding)
{
	/* This is where baud rate, data, stop, and parity bits are set. */
	*coding = line_coding;
	return 0;
}

int8_t app_set_control_line_state_callback(uint8_t interface,
                                           bool dtr, bool dts)
{
	return 0;
}

int8_t app_send_break_callback(uint8_t interface, uint16_t duration)
{
	return 0;
}




