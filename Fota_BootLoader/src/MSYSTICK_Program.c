/*
 * SYSTICK_program.c
 *
 *       Author: Yasmeen
 */

#include "BIT_Math.h"
#include "STD.h"
#include "LERROR_STATES.h"
#include "MSYSTICK_private.h"
#include "MSYSTICK_interface.h"
#include "MSYSTICK_config.h"



void ( * MSTK_CallBack ) ( void );

#define MSTK_SINGLE_INTERVAL        1

/* Define Variable for interval mode */
static uint8 MSTK_u8ModeOfInterval ;

void SysTick_Handler(void)
{

	uint8 Local_u8Temporary;

	if (MSTK_u8ModeOfInterval == MSTK_SINGLE_INTERVAL)
	{
		/* Disable STK Interrupt */
		CLR_BIT(MSTK->CTRL, 1);

		/* Stop Timer */
		SET_BIT(MSTK->CTRL, 0);
		MSTK -> LOAD = 0;
		MSTK -> VAL  = 0;
	}

	/* Callback notification */
	MSTK_CallBack();

	/* Clear interrupt flag */
	Local_u8Temporary = GET_BIT(MSTK->CTRL,16);

}

void MSTK_voidInit( void )
{
	// Disable Systick Interrupt  -  Clock = AHB / 8 - Stop Systic
	MSTK->CTRL = 0x00000000;
}

void MSTK_voidStart( uint32 Copy_PreloadValue )
{

	// AHB = EXT = 8MHZ & CLOCK OF MSTK = AHB / 8 = 8MHZ / 8 = 1MHZ = 1 Micro For Each Count
	//Load Reload Value
	MSTK->LOAD = Copy_PreloadValue - 1;
	//Clear Val Register
	MSTK->VAL  = 0 ;
	//Enable Systick
	SET_BIT( MSTK->CTRL , 0 );

}

void MSTK_voidINTStatus( uint8 Copy_u8Status )
{
	MSTK->CTRL &= ~( 1 << 1 );
	MSTK->CTRL |=  ( Copy_u8Status << 1 );
}

uint8 MSTK_u8ReadFlag( void )
{
	return ( GET_BIT( MSTK->CTRL , 16 ) );
}


void MSTK_voidSetCallBack( void (*ptr)(void) )
{
	MSTK_CallBack = ptr;
}

void MSTK_voidSetIntervalSingle( uint32 Copy_u32Ticks, void (*Copy_ptr)(void) )
{
	/* Disable Timer */
	CLR_BIT(MSTK->CTRL,0);
	MSTK -> VAL = 0;

	/* Load ticks to load register */
	MSTK -> LOAD = Copy_u32Ticks*2;

	/* Start Timer */
	SET_BIT(MSTK->CTRL, 0);

	/* Save CallBack */
	MSTK_CallBack = Copy_ptr;

	/* Set Mode to Single */
	MSTK_u8ModeOfInterval = MSTK_SINGLE_INTERVAL;

	/* Enable STK Interrupt */
	SET_BIT(MSTK->CTRL, 1);
}


uint32  MSTK_u32GetElapsedTime(void)
{
	uint32 Local_u32ElapsedTime;

	Local_u32ElapsedTime = MSTK -> LOAD - MSTK -> VAL ;

	return Local_u32ElapsedTime/2;
}

/* Suppose That The Clock System (AHB) = 16MHZ EXT & The Systick Clock Is = AHB/8 */
void _delay_us( uint32 Copy_u32Time ){

	MSTK_voidInit();
	/* Disable INT */
	MSTK_voidINTStatus( MSTK_INT_DIS );
	MSTK_voidStart( Copy_u32Time * 2 );
	/* Wiat Flag Polling */
	while( MSTK_u8ReadFlag() == 0 );

}


void _delay_ms( uint32 Copy_u32Time ){

	MSTK_voidInit();
	/* Disable INT */
	MSTK_voidINTStatus( MSTK_INT_DIS );
	MSTK_voidStart( Copy_u32Time * 2000 );
	/* Wiat Flag Polling */
	while( MSTK_u8ReadFlag() == 0 );

}

void MSTK_voidStopTimer( void ){


	//Disable Systick
	CLR_BIT( MSTK->CTRL , 0 );
	/* Disable INT */
	MSTK_voidINTStatus( MSTK_INT_DIS ) ;

}
