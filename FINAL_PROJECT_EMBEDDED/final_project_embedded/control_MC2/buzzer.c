 /******************************************************************************
 *
 * Module: 	Buzzer
 *
 * File Name: buzzer.c
 *
 * Description: source file for the buzzer.c driver
 *
 * Author: Toka Mohamed
 *
 *******************************************************************************/

#include "buzzer.h"
#include "gpio.h"


/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/


void Buzzer_Init(void)
{
	/*set the pin as output pin*/
	GPIO_setupPinDirection(BUZZER_PORT_ID,BUZZER_PIN_ID,PIN_OUTPUT);
	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID,LOGIC_LOW);
}
 void Buzzer_On(void)
 {
	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID,LOGIC_HIGH);
 }
 void Buzzer_Off(void)
 {
	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID,LOGIC_LOW);
 }

