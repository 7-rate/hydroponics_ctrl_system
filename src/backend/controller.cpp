/*
 * 概要：各ポートの制御を行うクラス
 */
#include <Arduino.h>

#include "controller.h"

/******************************************************************/
/* Definitions                                                    */
/******************************************************************/
/***********************************/
/* Local definitions               */
/***********************************/
#define AC_OUT1 ( 8 )
#define AC_OUT2 ( 9 )
#define DC12V_OUT1 ( 20 )
#define DC12V_OUT2 ( 19 )
#define DC5V_OUT1 ( 18 )
#define DC5V_OUT2 ( 17 )

/***********************************/
/* Local Variables                 */
/***********************************/
static ac_state_t AC_OUT1_state = AC_OFF;
static ac_state_t AC_OUT2_state = AC_OFF;
static int DC12V_OUT1_state = 0;
static int DC12V_OUT2_state = 0;
static int DC5V_OUT1_state = 0;
static int DC5V_OUT2_state = 0;

/***********************************/
/* Global Variables                */
/***********************************/

/******************************************************************/
/* Implementation                                                 */
/******************************************************************/
/***********************************/
/* Local functions                 */
/***********************************/
static PinStatus ac_pin_status( ac_state_t state ) {
    return ( state == AC_ON ) ? HIGH : LOW;
}

/***********************************/
/* Class implementions             */
/***********************************/

/***********************************/
/* Global functions                */
/***********************************/
void controller_set_AC_OUT1( ac_state_t state ) {
    AC_OUT1_state = state;
    digitalWrite( AC_OUT1, ac_pin_status( AC_OUT1_state ) );
}

void controller_set_AC_OUT2( ac_state_t state ) {
    AC_OUT2_state = state;
    digitalWrite( AC_OUT2, ac_pin_status( AC_OUT2_state ) );
}

void controller_set_DC12V_OUT1( dc_state_t state ) {
    int value = 0;
    switch ( state ) {
    case DC_ON:
        value = 255;
        break;
    case DC_F1_FLUCT:
        value = 127; // TODO 1/f揺らぎの実装
        break;
    case DC_OFF:
    default:
        value = 0;
        break;
    }
    DC12V_OUT1_state = value;
    analogWrite( DC12V_OUT1, DC12V_OUT1_state );
}

void controller_set_DC12V_OUT2( dc_state_t state ) {
    int value = 0;
    switch ( state ) {
    case DC_ON:
        value = 255;
        break;
    case DC_F1_FLUCT:
        value = 127; // TODO 1/f揺らぎの実装
        break;
    case DC_OFF:
    default:
        value = 0;
        break;
    }
    DC12V_OUT2_state = value;
    analogWrite( DC12V_OUT2, DC12V_OUT2_state );
}

void controller_set_DC5V_OUT1( dc_state_t state ) {
    int value = 0;
    switch ( state ) {
    case DC_ON:
        value = 255;
        break;
    case DC_F1_FLUCT:
        value = 127; // TODO 1/f揺らぎの実装
        break;
    case DC_OFF:
    default:
        value = 0;
        break;
    }
    DC5V_OUT1_state = value;
    analogWrite( DC5V_OUT1, DC5V_OUT1_state );
}

void controller_set_DC5V_OUT2( dc_state_t state ) {
    int value = 0;
    switch ( state ) {
    case DC_ON:
        value = 255;
        break;
    case DC_F1_FLUCT:
        value = 127; // TODO 1/f揺らぎの実装
        break;
    case DC_OFF:
    default:
        value = 0;
        break;
    }
    DC5V_OUT2_state = value;
    analogWrite( DC5V_OUT2, DC5V_OUT2_state );
}

void controller_init( void ) {
    pinMode( AC_OUT1, OUTPUT );
    pinMode( AC_OUT2, OUTPUT );
    pinMode( DC12V_OUT1, OUTPUT );
    pinMode( DC12V_OUT2, OUTPUT );
    pinMode( DC5V_OUT1, OUTPUT );
    pinMode( DC5V_OUT2, OUTPUT );

    digitalWrite( AC_OUT1, HIGH );
    digitalWrite( AC_OUT2, HIGH );
    analogWrite( DC12V_OUT1, 0 );
    analogWrite( DC12V_OUT2, 0 );
    analogWrite( DC5V_OUT1, 0 );
    analogWrite( DC5V_OUT2, 0 );
}

void controller_task() {
}
