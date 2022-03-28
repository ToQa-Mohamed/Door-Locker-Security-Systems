 /******************************************************************************
 *
 * Module: 	timer0
 *
 * File Name: timer0.c
 *
 * Description: source file for the timer.c driver
 *
 * Author: Toka Mohamed
 *
 *******************************************************************************/
#include"timer0.h"
#include"avr/io.h"
#include <avr/interrupt.h>

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/
  ISR(TIMER0_COMP_vect)
  {
 	if(g_callBackPtr != NULL_PTR)
 	{
  		/* Call the Call Back function in the application */
  		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
  	}
  }

  ISR(TIMER0_OVF_vect)
  {
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
  }

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void Timer0_Init(const Timer0_ConfigType * Config_Ptr)
{
	SET_BIT(TCCR0,FOC0); /*non PWM mode*/
	if((Config_Ptr->timer_mode)==NORMAL_MODE)
	{
		/*choosing normal mode*/
		CLEAR_BIT(TCCR0,WGM00);
		CLEAR_BIT(TCCR0,WGM01);
		/*enable normal mode interrupt*/
		SET_BIT(TIMSK,TOIE0);
	}
	else if((Config_Ptr->timer_mode)==COMPARE_MODE)
	{
		/*choosing compare mode*/
		CLEAR_BIT(TCCR0,WGM00);
		SET_BIT(TCCR0,WGM01);
		/*enable compare mode interrupt*/
		SET_BIT(TIMSK,OCIE0);
		/*Compare value of the timer*/
		OCR0 = Config_Ptr->compare_value;
	}
	/*Normal port operation, OC0 disconnected*/
	CLEAR_BIT(TCCR0,COM00);
	CLEAR_BIT(TCCR0,COM01);

	/*prescaler*/
	TCCR0 = (TCCR0 & 0xF8) | (Config_Ptr->prescaler);

	/*initial value of the timer*/
	TCNT0 = Config_Ptr->initial_value;


}

void Timer0_DeInit(void)
{
	/*clear all the timer registers*/
	TCCR0=0;
	TCNT0=0;
	OCR0=0;
    /*Disable the interrupts*/
	CLEAR_BIT(TIMSK,OCIE0);
	CLEAR_BIT(TIMSK,TOIE0);
}

void Timer0_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}















