/*
 * 概要：フロントエンド制御
 */
#include <Arduino.h>
#include <SPI.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>
#include <EEPROM.h>

#include "backend/controller.h"
#include "frontend.h"

// note:
// 画面サイズ 320 x 240
// タッチスクリーンは 0~4096 x 0~4096 で取得される

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

class toggle_button {
  public:
    toggle_button( const char* label, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t on_color, uint16_t off_color );
    void draw( Adafruit_ILI9341& display );
    bool is_touched( int16_t touch_x, int16_t touch_y ) {
        return ( touch_x >= x && touch_x < x + w && touch_y >= y && touch_y < y + h );
    }
    void toggle_state() {
        state = !state;
    }

    bool state; // ボタンの状態 (ON/OFF)
  private:
    char label[10];               // ラベルの最大長は9文字 + 終端文字1文字
    uint16_t x, y, w, h;          // ボタンの位置とサイズ
    uint16_t on_color, off_color; // ボタンの色
};

/***********************************/
/* Local Variables                 */
/***********************************/
Adafruit_ILI9341 display = Adafruit_ILI9341( PIN_CS, PIN_DC, PIN_RST );
XPT2046_Touchscreen touch( PIN_TOUCH_CS );

static toggle_button button_prev_screen( "<", 0, 0, 30, 240, ILI9341_LIGHTGREY, ILI9341_LIGHTGREY );
static toggle_button button_next_screen( ">", 290, 0, 30, 240, ILI9341_LIGHTGREY, ILI9341_LIGHTGREY );

static toggle_button button_ac_out1( "AC_OUT1", 35, 35, 123, 97, ILI9341_MAGENTA, ILI9341_LIGHTGREY );
static toggle_button button_ac_out2( "AC_OUT2", 35, 137, 123, 97, ILI9341_MAGENTA, ILI9341_LIGHTGREY );
static toggle_button button_dc12v_out1( "DC12V_OUT1", 162, 35, 123, 46, ILI9341_ORANGE, ILI9341_LIGHTGREY );
static toggle_button button_dc12v_out2( "DC12V_OUT2", 162, 85, 123, 46, ILI9341_ORANGE, ILI9341_LIGHTGREY );
static toggle_button button_dc5v_out1( "DC5V_OUT1", 162, 137, 123, 46, ILI9341_PURPLE, ILI9341_LIGHTGREY );
static toggle_button button_dc5v_out2( "DC5V_OUT2", 162, 188, 123, 46, ILI9341_PURPLE, ILI9341_LIGHTGREY );

static bool calibration_mode = false;
static int calibration_step = 0;
static int16_t calibration_data[4][2]; // 4点分の生座標を保持

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
    display.setCursor( ( x + w / 2 ) - ( strlen( label ) * 12 / 2 ), ( y + h / 2 ) + 4 );
    display.setTextColor( ILI9341_BLACK );
    display.print( label );
}

/***********************************/
/* Local functions                 */
/***********************************/
bool get_calibrated_touch( int16_t& screen_x, int16_t& screen_y ) {
    if ( !touch.touched() )
        return false;
    TS_Point p = touch.getPoint();

    // 簡易的に2点だけでマッピング(例示)
    screen_x = map( p.x, calibration_data[0][0], calibration_data[1][0], 0, 320 );
    screen_y = map( p.y, calibration_data[0][1], calibration_data[2][1], 0, 240 );

    return true;
}

static void draw_title( const char* title ) {
    display.fillRect( 30, 0, 260, 30, ILI9341_DARKGREY );
    display.setCursor( ( 30 + 260 / 2 ) - ( strlen( title ) * 9 / 2 ), 30 / 2 + 4 );
    display.setTextColor( ILI9341_BLACK );
    display.println( title );
}

static int screen_no = 0;
static void reflesh_screen() {
    // 前回タッチ状態を保持
    static bool last_touched = false;
    int16_t touch_x = 0, touch_y = 0;
    // 現在タッチされていれば座標を取得し curr_touched=true
    bool curr_touched = get_calibrated_touch( touch_x, touch_y );

    switch ( screen_no ) {
    case 0:
        // 初期画面
        display.fillScreen( ILI9341_WHITE );
        draw_title( "Manual Control" );
        button_prev_screen.draw( display );
        button_next_screen.draw( display );
        button_ac_out1.draw( display );
        button_ac_out2.draw( display );
        button_dc12v_out1.draw( display );
        button_dc12v_out2.draw( display );
        button_dc5v_out1.draw( display );
        button_dc5v_out2.draw( display );
        screen_no = 1;
        break;
    case 1:
        // タッチ開始(立ち上がり)時のみ処理
        if ( curr_touched && !last_touched ) {
            if ( button_ac_out1.is_touched( touch_x, touch_y ) ) {
                button_ac_out1.toggle_state();
                button_ac_out1.draw( display );
                controller_set_AC_OUT1( button_ac_out1.state ? AC_ON : AC_OFF );
            }
            if ( button_ac_out2.is_touched( touch_x, touch_y ) ) {
                button_ac_out2.toggle_state();
                button_ac_out2.draw( display );
                controller_set_AC_OUT2( button_ac_out2.state ? AC_ON : AC_OFF );
            }
            if ( button_dc12v_out1.is_touched( touch_x, touch_y ) ) {
                button_dc12v_out1.toggle_state();
                button_dc12v_out1.draw( display );
                controller_set_DC12V_OUT1( button_dc12v_out1.state ? 255 : 0 );
            }
            if ( button_dc12v_out2.is_touched( touch_x, touch_y ) ) {
                button_dc12v_out2.toggle_state();
                button_dc12v_out2.draw( display );
                controller_set_DC12V_OUT2( button_dc12v_out2.state ? 255 : 0 );
            }
            if ( button_dc5v_out1.is_touched( touch_x, touch_y ) ) {
                button_dc5v_out1.toggle_state();
                button_dc5v_out1.draw( display );
                controller_set_DC5V_OUT1( button_dc5v_out1.state ? 255 : 0 );
            }
            if ( button_dc5v_out2.is_touched( touch_x, touch_y ) ) {
                button_dc5v_out2.toggle_state();
                button_dc5v_out2.draw( display );
                controller_set_DC5V_OUT2( button_dc5v_out2.state ? 255 : 0 );
            }
        }
        break;
    case 99: {
        if ( calibration_step < 4 ) {
            if ( calibration_step == 0 ) {
                display.fillCircle( 10, 10, 5, ILI9341_RED );
                display.setCursor( 20, 10 );
                display.setTextColor( ILI9341_BLACK );
            } else if ( calibration_step == 1 ) {
                display.fillCircle( 310, 10, 5, ILI9341_RED );
                display.setCursor( 220, 10 );
                display.setTextColor( ILI9341_BLACK );
            } else if ( calibration_step == 2 ) {
                display.fillCircle( 10, 230, 5, ILI9341_RED );
                display.setCursor( 20, 210 );
                display.setTextColor( ILI9341_BLACK );
            } else {
                display.fillCircle( 310, 230, 5, ILI9341_RED );
                display.setCursor( 220, 210 );
                display.setTextColor( ILI9341_BLACK );
            }
            if ( touch.touched() ) {
                // 座標を記録
                TS_Point tmp = touch.getPoint();
                calibration_data[calibration_step][0] = tmp.x;
                calibration_data[calibration_step][1] = tmp.y;
                calibration_step++;
            }
        } else {
            EEPROM.begin( 64 );
            EEPROM.put( 0, calibration_data );
            EEPROM.commit();
            EEPROM.end();
            calibration_mode = false;
            screen_no = 0;
        }
        break;
    }
    default:
        break;
    }
    // 現在のタッチ状態を保存
    last_touched = curr_touched;
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

    EEPROM.begin( 64 );
    EEPROM.get( 0, calibration_data );
    EEPROM.end();
    Serial.println( "Calibration Data:" );
    for ( int i = 0; i < 4; i++ ) {
        Serial.printf( "Point %d: (%d, %d)\r\n", i, calibration_data[i][0], calibration_data[i][1] );
    }

    // タッチされたまま起動された場合はタッチスクリーンのキャリブレーションモードに入る
    if ( touch.touched() ) {
        screen_no = 99;
        calibration_mode = true;
        // 離されるまで待機
        while ( touch.touched() ) {
            delay( 10 );
        }
        display.fillScreen( ILI9341_WHITE );
        draw_title( "Touch Calibration" );
    }

    // 初期画面表示
    display.fillScreen( ILI9341_BLACK );
    display.setCursor( 20, 50 );
    display.setTextColor( ILI9341_WHITE );
    display.setFont( &FreeSans9pt7b );
    display.setTextSize( 1 );
}

void frontend_task() {
    reflesh_screen();
}
