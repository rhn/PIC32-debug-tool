#ifndef GPIODRV_H_573f2ca8b43e44209acc90f0f9b7de5d
#define GPIODRV_H_573f2ca8b43e44209acc90f0f9b7de5d


////////
// MX270
////////
#if defined(__32MX270F256D__)
// LED
#define LEDUSER_TRISbits		TRISCbits
#define LEDUSER_TRISPIN			TRISC4
#define LEDUSER_LATbits			LATCbits
#define LEDUSER_LATPIN			LATC4
#define LEDUSER_LATINV			LATCINV
#define LEDUSER_MASK			(1<<4)

// Button
#define BTNUSER_TRISbits		TRISCbits
#define BTNUSER_TRISPIN			TRISC5
#define BTNUSER_PORTbits		PORTCbits
#define BTNUSER_PORTPIN			RC5
#define BTNUSER_PULLREG			CNPUC	// Call also pull down with CNPDx
#define BTNUSER_PULLBIT			(1<<5)

// UART
#define UART_TX_TRISbits		TRISCbits
#define UART_TX_TRISPIN			TRISC8
#define UART_TX_LATbits			LATCbits
#define UART_TX_LATPIN			LATC8
#define UART_TX_RP_REG			RPC9R
#define UART_TX_RP_VAL			0b0010	// U2TX

#define UART_RX_TRISbits		TRISCbits
#define UART_RX_TRISPIN			TRISC9
#define UART_RX_PULLREG			CNPUC	// UARTs are idle high, so pull-up
#define UART_RX_PULLBIT			(1<<9)
#define UART_RX_REMAP_VAL		0b0110

#define UART_MODE_bits			U2MODEbits
#define UART_STA_bits			U2STAbits
#define UART_BRG_reg			U2BRG
#define UART_TX_reg				U2TXREG
#define UART_RX_reg				U2RXREG

#define UART_INT_IPC_bits			IPC9bits
#define UART_INT_IPC_PRIORITY		U2IP
#define UART_INT_IPC_SUBPRIORITY	U2IS
#define UART_INT_IEC_bits			IEC1bits
#define UART_INT_IEC_RXIE			U2RXIE
#define UART_INT_IFS_bits			IFS1bits
#define UART_INT_IFS_RXIF				U2RXIF


////////
// MX440
////////
#elif defined(__32MX440F256H__)
// LED
#define LEDUSER_TRISbits		TRISDbits
#define LEDUSER_TRISPIN			TRISD1
#define LEDUSER_LATbits			LATDbits
#define LEDUSER_LATPIN			LATD1
#define LEDUSER_LATINV			LATDINV
#define LEDUSER_MASK			(1<<1)

// Button
#define BTNUSER_TRISbits		TRISDbits
#define BTNUSER_TRISPIN			TRISD0
#define BTNUSER_PORTbits		PORTDbits
#define BTNUSER_PORTPIN			RD0
// No pull-up/down available on this pin

// UART
#define UART_TX_TRISbits		TRISDbits
#define UART_TX_TRISPIN			TRISD3
#define UART_TX_LATbits			LATDbits
#define UART_TX_LATPIN			LATD3
// No remapping available

#define UART_RX_TRISbits		TRISDbits
#define UART_RX_TRISPIN			TRISD2

#define UART_MODE_bits			U1MODEbits
#define UART_STA_bits			U1STAbits
#define UART_BRG_reg			U1BRG
#define UART_TX_reg				U1TXREG
#define UART_RX_reg				U1RXREG

#define UART_INT_IPC_bits			IPC6bits
#define UART_INT_IPC_PRIORITY		U1IP
#define UART_INT_IPC_SUBPRIORITY	U1IS
#define UART_INT_IEC_bits			IEC0bits
#define UART_INT_IEC_RXIE			U1RXIE
#define UART_INT_IFS_bits			IFS0bits
#define UART_INT_IFS_RXIF			U1RXIF



#endif

#endif
