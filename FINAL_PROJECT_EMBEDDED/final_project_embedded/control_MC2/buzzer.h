 /******************************************************************************
 *
 * Module: 	Buzzer
 *
 * File Name: buzzer.h
 *
 * Description: header file for the buzzer.h driver
 *
 * Author: Toka Mohamed
 *
 *******************************************************************************/


#ifndef BUZZER_H_
#define BUZZER_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define BUZZER_PORT_ID  PORTD_ID
#define BUZZER_PIN_ID   PIN2_ID

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/*
 * Description :
 * set the pin as output pin
 */
void Buzzer_Init(void);
/*
 * Description :
 * turn on the buzzer
 */
void Buzzer_On(void);
/*
 * Description :
 * turn off the buzzer
 */
void Buzzer_Off(void);


#endif /* BUZZER_H_ */
