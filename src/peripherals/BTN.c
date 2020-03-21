#include <p32xxxx.h>
#include <inttypes.h>
#include <GPIODrv.h>

void BTN_init(){
	BTNUSER_TRISbits.BTNUSER_TRISPIN = 1;					// Set as input
	#ifdef BTNUSER_PULLREG
		BTNUSER_PULLREG = BTNUSER_PULLREG | BTNUSER_PULLBIT;	// Enable pull-up (in this case)
	#endif
}

void BTN_update(){
	// TODO use if updating via SysTick or something
}

uint8_t BTN_getStatus(){
	// Inverted logic - button pressed gives 0
	if (BTNUSER_PORTbits.BTNUSER_PORTPIN){
		return 0;
	}
	else{
		return 1;
	}

}
