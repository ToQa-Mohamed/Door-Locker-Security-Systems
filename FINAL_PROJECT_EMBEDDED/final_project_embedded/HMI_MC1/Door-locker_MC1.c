/*
 ================================================================================================
 Name        : DoorLocker_MC1.c
 Author      : Toka Mohamed
 Description : MC1 source file
 ================================================================================================
 */
#define F_CPU 8000000

#include"lcd.h"
#include"keypad.h"
#include "uart.h"
#include"timer0.h"
#include <util/delay.h> /* For the delay functions */
#include "avr/io.h" /* To use the SREG Register */

/*******************************************************************************
 *                              Definitions                                  *
 *******************************************************************************/

#define MC_READY 0xFF     // when MC1 is ready to recieve from MC2
#define STORE_EEPROM 0xF1 // when MC2 send password to MC2 to store in eeprom
#define OPEN 0xF2         // rotate the motor CW
#define CLOSE 0xF3        // rotate the motor ACW
#define WAIT 0xF5         // stop the motor
#define BUZZER_ON 0xF6    // turn on the buzzer
#define BUZZER_OFF 0xF7   // turn off the buzzer

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*global variable to count overflows*/
uint32 count1 = 0;

void timer0_count1(void) {
	count1++;
}

void Password(uint8 *new_password) { /* function to take values from keypad and store in array of 5 numbers  */
	uint8 key_num;

	for (int i = 0; i < 5; i++) {
		key_num = KEYPAD_getPressedKey();
		if ((key_num >= 0) && (key_num <= 9)) {
			new_password[i] = key_num;
			LCD_intgerToString(key_num);
		}
		_delay_ms(500);
	}
}

void send_password(uint8 *new_password) /* function to send password to MC2 to save in eeprom */
{
	for (int i = 0; i < 5; i++) {
		UART_sendByte(new_password[i]);
		_delay_ms(100);
	}
}

void newPassword(uint8 match_flag, uint8 *password1, uint8 *password2) { /* function to take password 2 times and check them */

	uint8 correct_flag = 0; /* to check if the two passwords are the same */
	while (correct_flag == 0) {
		LCD_clearScreen();
		LCD_displayString("Enter new password:");
		LCD_moveCursor(1, 0);
		UART_sendByte(STORE_EEPROM);
		Password(password1);

		if (KEYPAD_getPressedKey() == 13) {
			LCD_clearScreen();
			send_password(password1);
		}
		LCD_clearScreen();
		LCD_displayString("Reenter password:");
		LCD_moveCursor(1, 0);
		Password(password2);
		if (KEYPAD_getPressedKey() == 13) {
			LCD_clearScreen();
			for (int i = 0; i < 5; i++) {
				if (password1[i] == password2[i]) {
					match_flag = 0;
				} else {
					match_flag = 1;
					break;
				}
			}
			if (match_flag == 0) {
				LCD_displayString("Saved");
				_delay_ms(500);
				LCD_clearScreen();
				correct_flag = 1;
			} else {
				LCD_clearScreen();
				LCD_displayString("Wrong password");
				_delay_ms(500);
				LCD_clearScreen();
			}

		}
	}
}

int main(void) {

	uint8 new_password[5]; /*new entered password*/
	uint8 new_password_repeated[5]; /* enter the password again */
	uint8 eeprom_password[5]; /*password sent by the MC2 from eeprom*/
	uint8 old_password[5]; /*old password entered by the user*/
	uint8 matched_flag = 0; /* set when the two new passwords don't match*/
	uint8 matched_flag2 = 0; /* set when the two old passwords don't match*/
	uint8 error_flag = 0; /*to count the number of wrongly entered passwords*/
	uint8 correct_flag = 0; /*while it is 0 we repeat the loop again until entering correct password*/

	/* Enable interrupts by setting I-bit */
	SREG |= (1 << 7);

	/* Initialize LCD driver */
	LCD_init();

	/*baud rate=9600bps, No parity, one stop bit, 8 data bits*/
	UART_ConfigType uart_config = { 9600, DISABLE, ONE_BIT, EIGHT_BITS };

	/* Initialize UART driver */
	UART_init(&uart_config);

	/* timer0 Normal mode, prescaler=1024, initial value=0 */
	Timer0_ConfigType timer0_config = { NORMAL_MODE, FCPU_1024, 0, 0 };
	Timer0_setCallBack(timer0_count1);

	/* create new password */
	newPassword(matched_flag, new_password, new_password_repeated);
	matched_flag = 0;

	/* recieve password from eeprom interfaced with MC2  */
	UART_sendByte(MC_READY);
	for (int i = 0; i < 5; i++) {
		eeprom_password[i] = UART_recieveByte();
	}

	while (1) {

		LCD_displayString("to change password:-");
		LCD_moveCursor(1, 0);
		LCD_displayString("to open the door:+");

		if (KEYPAD_getPressedKey() == '-') {
			while ((error_flag < 3) && (correct_flag == 0)) /* we can enter wrong password 3 times only */
			{
				LCD_clearScreen();
				LCD_displayString("enter old password:");
				LCD_moveCursor(1, 0);
				Password(old_password);
				if ((KEYPAD_getPressedKey()) == 13) {
					for (int i = 0; i < 5; i++) {
						if (old_password[i] == eeprom_password[i]) {
							matched_flag2 = 0;
						} else {
							matched_flag2 = 1;
							break;
						}
					}
					if (matched_flag2 == 0) {
						LCD_clearScreen();
						LCD_displayString("correct password");
						_delay_ms(400);
						LCD_clearScreen();
						newPassword(matched_flag, new_password,new_password_repeated);
						matched_flag = 0;
						UART_sendByte(MC_READY);
						for (int i = 0; i < 5; i++) {
							eeprom_password[i] = UART_recieveByte();
						}
						correct_flag = 1;
					} else {
						LCD_clearScreen();
						LCD_displayString("wrong password");
						_delay_ms(400);
						error_flag++;
						if (error_flag == 3) {
							UART_sendByte(BUZZER_ON);
							LCD_clearScreen();
							LCD_displayString("!!!LOCKED!!!");

							Timer0_Init(&timer0_config);
							while(count1 != 1832){}; //1min
							count1=0;
							Timer0_DeInit();

							LCD_clearScreen();
							UART_sendByte(BUZZER_OFF);
						}

					}

				}
			}
			matched_flag2 = 0;
			error_flag = 0;
			correct_flag = 0;
		} else if (KEYPAD_getPressedKey() == '+') {

			while ((error_flag < 3) && (correct_flag == 0)) {
				LCD_clearScreen();
				LCD_displayString("old password :");
				LCD_moveCursor(1, 0);
				Password(old_password);
				if ((KEYPAD_getPressedKey()) == 13) {
					for (int i = 0; i < 5; i++) {
						if (old_password[i] == eeprom_password[i]) {
							matched_flag2 = 0;
						} else {
							matched_flag2 = 1;
							break;
						}
					}
					if (matched_flag2 == 0) {
						LCD_clearScreen();
						LCD_displayString("Opening...");
						UART_sendByte(OPEN);

						Timer0_Init(&timer0_config);
						while(count1 != 458){};  //15 sec
						count1=0;
						Timer0_DeInit();

						LCD_clearScreen();
						LCD_displayString("Opened");
						UART_sendByte(WAIT);

						Timer0_Init(&timer0_config);
						while(count1 != 92){};  // 3 sec
						count1=0;
						Timer0_DeInit();

						LCD_clearScreen();
						LCD_displayString("Closing...");
						UART_sendByte(CLOSE);

						Timer0_Init(&timer0_config);
						while(count1 != 458){};   //15 sec
						count1=0;
						Timer0_DeInit();

						LCD_clearScreen();
						correct_flag = 1;
					} else {
						LCD_clearScreen();
						LCD_displayString("wrong password");
						_delay_ms(400);
						error_flag++;
						if (error_flag == 3) {
							UART_sendByte(BUZZER_ON);
							LCD_clearScreen();
							LCD_displayString("!!!LOCKED!!!");

							Timer0_Init(&timer0_config);
							while(count1 != 1832){}; //1min
							count1=0;
							Timer0_DeInit();

							LCD_clearScreen();
							UART_sendByte(BUZZER_OFF);

						}

					}
				}
			}
			matched_flag2 = 0;
			error_flag = 0;
			correct_flag = 0;
		}
	}
}

