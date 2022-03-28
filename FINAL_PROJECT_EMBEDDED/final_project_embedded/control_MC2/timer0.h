 /******************************************************************************
 *
 * Module: 	timer0
 *
 * File Name: timer0.h
 *
 * Description: header file for the timer.h driver
 *
 * Author: Toka Mohamed
 *
 *******************************************************************************/

#ifndef TIMER0_H_
#define TIMER0_H_

#include"std_types.h"
#include"common_macros.h"

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/
typedef enum{
	NORMAL_MODE , COMPARE_MODE
}Timer0_Mode;

typedef enum{
	NO_CLOCK , FCPU , FCPU_8 , FCPU_64 , FCPU_256 , FCPU_1024
}Timer0_Prescaler;

typedef struct{
	Timer0_Mode timer_mode;
	Timer0_Prescaler prescaler;
	uint8 initial_value;
	uint8 compare_value;
}Timer0_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * choose the mode (normal / compare)
 * choose the prescaler
 * choose the initial value of the timer
 * choose the compare value in case of compare mode
 */
void Timer0_Init(const Timer0_ConfigType * Config_Ptr);
/*
 * Description:
 * stop the timer
 */
void Timer0_DeInit(void);
/*
 * Description:
 * set the callback function address
 */
void Timer0_setCallBack(void(*a_ptr)(void));

#endif /* TIMER0_H_ */
