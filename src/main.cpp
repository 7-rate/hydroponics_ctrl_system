#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>

// SPI Pin Definitions for Display
#define PIN_SCK 2  // GPIO2
#define PIN_MOSI 3 // GPIO3
#define PIN_MISO 4 // GPIO4
#define PIN_CS 5   // GPIO5
#define PIN_DC 7   // GPIO6 for DC (Data/Command)
#define PIN_RST -1 // GPIO7 for Reset

// Touch Controller Pin
#define PIN_TOUCH_CS 6 // GPIO8 for Touch Controller CS

Adafruit_ILI9341 display = Adafruit_ILI9341( PIN_CS, PIN_DC, PIN_RST );
XPT2046_Touchscreen touch( PIN_TOUCH_CS );

uint16_t lastX, lastY;
bool penDown = false;
uint16_t currentColor = ILI9341_RED; // 初期色：赤

// パレットボタンのY座標
#define PALETTE_Y 10

void draw_clear_button() {
    // 消しゴムボタン (黒)
    display.fillRect( 270, PALETTE_Y, 40, 20, ILI9341_BLACK );
    display.drawRect( 270, PALETTE_Y, 40, 20, ILI9341_WHITE );
    display.setCursor( 273, PALETTE_Y + 6 );
    display.setTextColor( ILI9341_WHITE );
    display.setTextSize( 1 );
    display.print( "Clear" );
}

void setup() {
    // シリアル通信の初期化（デバッグ用）
    Serial.begin( 115200 );
    delay( 1000 );
    Serial.println( "Adafruit ILI9341 TouchScreen Drawing on Raspberry Pi Pico W" );

    // SPI設定
    SPI.setRX( PIN_MISO );
    SPI.setTX( PIN_MOSI );
    SPI.setSCK( PIN_SCK );

    // ディスプレイの初期化
    display.begin();

    // ディスプレイの向きを上下反対に設定（3）
    display.setRotation( 3 );

    // タッチスクリーンの初期化
    touch.begin();
    touch.setRotation( 1 ); // タッチスクリーンも同じ向きに設定

    // 初期画面表示
    display.fillScreen( ILI9341_BLACK );
    display.setCursor( 20, 50 );
    display.setTextColor( ILI9341_WHITE );
    display.setTextSize( 2 );
    display.println( "Touch to draw!" );
    display.println( "Select a color from palette" );

    // 色パレットの描画
    draw_clear_button();

    Serial.println( "Setup complete" );
    delay( 2000 );

    // お絵かき用キャンバスを準備
    display.fillRect( 0, 40, display.width(), display.height() - 40, ILI9341_BLACK );
}

void loop() {
    // タッチスクリーンの読み取り
    if ( touch.touched() ) {
        TS_Point p = touch.getPoint();

        // ディスプレイの解像度に合わせて座標を変換
        uint16_t x = map( p.x, 0, 4095, 0, display.width() );
        uint16_t y = map( p.y, 0, 4095, 0, display.height() );

        // タッチ位置のデバッグ出力
        Serial.print( "Touch at x=" );
        Serial.print( x );
        Serial.print( ", y=" );
        Serial.println( y );

        // クリアボタンの判定
        if ( x >= 270 && x <= 310 && y >= PALETTE_Y && y <= PALETTE_Y + 20 ) {
            display.fillRect( 0, 40, display.width(), display.height() - 40, ILI9341_BLACK );
            delay( 100 );
            return;
        } else {
            // お絵かきエリアでの描画
            if ( !penDown ) {
                // 最初のタッチポイント
                lastX = x;
                lastY = y;
                penDown = true;
            } else {
                // 線を描画
                display.drawLine( lastX, lastY, x, y, currentColor );
                lastX = x;
                lastY = y;
            }
        }
    } else {
        penDown = false;
    }

    delay( 10 ); // 安定化のための短い遅延
}