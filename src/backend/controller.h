#pragma once
/*
 * 概要：各ポートの制御を行うクラス
 */
/******************************************************************/
/* Definitions                                                    */
/******************************************************************/
/***********************************/
/* Global definitions              */
/***********************************/
typedef enum { AC_OFF, AC_ON } ac_state_t;

/***********************************/
/* Class                           */
/***********************************/

/***********************************/
/* Global functions                */
/***********************************/
void controller_set_AC_OUT1( ac_state_t state );
void controller_set_AC_OUT2( ac_state_t state );
void controller_set_DC12V_OUT1( int value );
void controller_set_DC12V_OUT2( int value );
void controller_set_DC5V_OUT1( int value );
void controller_set_DC5V_OUT2( int value );
void controller_init( void );
void controller_task();

/***********************************/
/* Global Variables                */
/***********************************/
