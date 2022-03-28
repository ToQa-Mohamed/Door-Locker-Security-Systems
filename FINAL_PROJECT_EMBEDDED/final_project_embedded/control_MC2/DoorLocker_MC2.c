/*
 ================================================================================================
 Name        : DoorLocker_MC2.c
 Author      : Toka Mohamed
 Description : MC2 source file
 ================================================================================================
 */
#define F_CPU 8000000

#include "uart.h"
#include "dc_motor.h"
#include "buzzer.h"
#include "external_eeprom.h"
#include "timer0.h"
#include "util/delay.h"
#include "twi.h"
#include "avr/io.h" /* To use the SREG Register */

/*******************************************************************************
 *                               Definitions                                  *
 *******************************************************************************/

#define MC_READY 0xFF     // when MC1 is ready to recieve from MC2
#define STORE_EEPROM 0xF1 // when MC2 send password to MC2 to store in eeprom
#define OPEN 0xF2         // rotate the motor CW
#define CLOSE 0xF3        // rotate the motor ACW
#define WAIT 0xF5         // stop the motor
#define BUZZER_ON 0xF6    // turn on the buzzer
#define BUZZER_OFF 0xF7   // turn off the buzzer

/*global variable to count overflows*/
uint32 count2 = 0;
void timer0_count2(void) {
	count2++;
}

int main(void) {
	uint8 data; /*data from eeprom*/
	uint8 command; /*command sent by MC1 to MC2 in the UART*/
	uint8 recieved_password[5];/*the recieved password from MC1*/
	uint8 pass_to_send[5]; /*password loaded from eeprom to send to MC1*/

	DcMotor_Init(); /*initialize the motor*/
	Buzzer_Init(); /*initialize the buzzer*/

	/*baud rate=9600bps, No parity, one stop bit, 8 data bits*/
	UART_ConfigType uart_config = { 9600, DISABLE, ONE_BIT, EIGHT_BITS };

	/* Bit Rate: 400.000 kbps using zero pre-scaler TWPS=00 and F_CPU=8Mhz and master_address=0b0000001*/
	TWI_ConfigType TWI_config = { 400000, 0, 0b0000001 };

	/* Enable interrupts by setting I-bit */
	SREG |= (1 << 7);

	/* Initialize UART driver */
	UART_init(&uart_config);

	/* initialize TWI driver */
	TWI_init(&TWI_config);

	/* timer0 Normal mode, prescaler=1024, initial value=0 */
	Timer0_ConfigType timer0_config = { NORMAL_MODE, FCPU_1024, 0, 0 };
	/* set the callback function of the timer */
	Timer0_setCallBack(timer0_count2);


	while (1) {

		command = UART_recieveByte(); /* receive command from MC1 through the UART*/
		if (command == STORE_EEPROM) {
			for (int i = 0; i < 5; i++) {
				recieved_password[i] = UART_recieveByte(); /* take the password from MC1  */
			}
			for (int i = 0; i < 5; i++) {
				EEPROM_writeByte(0x0311 + i, recieved_password[i]);/* store this password in eeprom starting from address 0x0311 */
				_delay_ms(10);
			}
			_delay_ms(100);
			for (int i = 0; i < 5; i++) {
				EEPROM_readByte(0x0311 + i, &data); /* read the password from eeprom */
				_delay_ms(10);
				pass_to_send[i] = data; /* store it in array to send it to MC1 */
			}
			while (UART_recieveByte() != MC_READY) { /* wait until MC1 is ready to receive password */
			};
			for (int i = 0; i < 5; i++) {
				UART_sendByte(pass_to_send[i]);
				_delay_ms(100);
			}
		} else if (command == BUZZER_ON) { /* if received command from MC1 to turn on the buzzer */
			Buzzer_On();
			while (UART_recieveByte() != BUZZER_OFF) { /* wait until MC1 send a command to turn off buzzer */
			};
			Buzzer_Off();
		} else if (command == OPEN) { /* if received from MC1 to open the door */
			DcMotor_Rotate(1); /* rotate the motor CW */

		} else if (command == CLOSE) { /* if received from MC1 to close the door */
			DcMotor_Rotate(2); /* rotate the motor ACW */

			Timer0_Init(&timer0_config);
			while(count2 != 458){}; //15 sec
			count2=0;
			Timer0_DeInit();

			DcMotor_Rotate(0); /* stop the motor */
		} else if (command == WAIT) { /* leave the door open  */
			DcMotor_Rotate(0); /* stop the motor */
		}

	}
}

