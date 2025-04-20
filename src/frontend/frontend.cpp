/*
 * 概要：フロントエンド制御
 */
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>

#include "frontend.h"

// note:
// 画面サイズ 320 x 240

/******************************************************************/
/* Definitions                                                    */
/******************************************************************/
/***********************************/
/* Local definitions               */
/***********************************/
// SPI Pin Definitions for Display
#define PIN_SCK 2  // GPIO2
#define PIN_MOSI 3 // GPIO3
#define PIN_MISO 4 // GPIO4
#define PIN_CS 5   // GPIO5
#define PIN_DC 7   // GPIO6 for DC (Data/Command)
#define PIN_RST -1 // GPIO7 for Reset

// Touch Controller Pin
#define PIN_TOUCH_CS 6 // GPIO8 for Touch Controller CS

// パレットボタンのY座標
#define PALETTE_Y 10

class toggle_button {
  public:
    toggle_button( const char* label, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t on_color, uint16_t off_color );
    void draw( Adafruit_ILI9341& display );
    void set_color( uint16_t color );
    void set_state( bool state ) {
        this->state = state;
    }

  private:
    char label[10];               // ラベルの最大長は9文字 + 終端文字1文字
    uint16_t x, y, w, h;          // ボタンの位置とサイズ
    uint16_t on_color, off_color; // ボタンの色
    bool state;                   // ボタンの状態 (ON/OFF)
};

/***********************************/
/* Local Variables                 */
/***********************************/
Adafruit_ILI9341 display = Adafruit_ILI9341( PIN_CS, PIN_DC, PIN_RST );
XPT2046_Touchscreen touch( PIN_TOUCH_CS );

static toggle_button button_prev_screen( "<", 0, 0, 30, 240, ILI9341_LIGHTGREY, ILI9341_LIGHTGREY );
static toggle_button button_next_screen( ">", 290, 0, 30, 240, ILI9341_LIGHTGREY, ILI9341_LIGHTGREY );

/***********************************/
/* Global Variables                */
/***********************************/

/******************************************************************/
/* Implementation                                                 */
/******************************************************************/
/***********************************/
/* Class implementions             */
/***********************************/
toggle_button::toggle_button( const char* label, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t on_color, uint16_t off_color ) {
    strncpy( this->label, label, sizeof( this->label ) - 1 );
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->on_color = on_color;
    this->off_color = off_color;
    this->state = false; // 初期状態はOFF
}

void toggle_button::draw( Adafruit_ILI9341& display ) {
    // ボタンの描画
    display.fillRect( x, y, w, h, state ? on_color : off_color );
    display.setCursor( ( x + w / 2 ) - ( strlen( label ) * 6 / 2 ), ( y + h / 2 ) );
    display.setTextColor( ILI9341_BLACK );
    display.setTextSize( 2 );
    display.print( label );
}

/***********************************/
/* Local functions                 */
/***********************************/
static void draw_title( const char* title ) {
    display.fillRect( 30, 0, 260, 30, ILI9341_DARKGREY );
    display.setCursor( ( 30 + 260 / 2 ) - ( strlen( title ) * 8 / 2 ), 30 / 2 - 8 );
    display.setTextColor( ILI9341_BLACK );
    display.setTextSize( 2 );
    display.println( title );
}

static int screen_no = 0;
static void reflesh_screen() {
    display.fillScreen( ILI9341_WHITE );
    switch ( screen_no ) {
    case 0:
        draw_title( "Manual Control" );
        button_prev_screen.draw( display );
        button_next_screen.draw( display );
        break;
    case 1:
        break;
    default:
        break;
    }
}

/***********************************/
/* Global functions                */
/***********************************/
void frontend_init() {
    SPI.setRX( PIN_MISO );
    SPI.setTX( PIN_MOSI );
    SPI.setSCK( PIN_SCK );
    display.begin();
    display.setRotation( 3 );
    touch.begin();
    touch.setRotation( 1 );

    // 初期画面表示
    display.fillScreen( ILI9341_BLACK );
    display.setCursor( 20, 50 );
    display.setTextColor( ILI9341_WHITE );
    display.setTextSize( 2 );
    display.println( "Hello!" );
    delay( 2000 );
}

void frontend_task() {
    if ( touch.touched() ) {
        reflesh_screen();
    }
}
