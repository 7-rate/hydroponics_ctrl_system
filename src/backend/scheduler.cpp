/*
 * 概要：スケジュールされたタスクを実行する(定時実行)
 */
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <time.h>

#include "config.h"
#include "scheduler.h"

// スケジュールによって各ポートになんのタスクを行うかを通知する

/******************************************************************/
/* Definitions                                                    */
/******************************************************************/
/***********************************/
/* Local definitions               */
/***********************************/
static const long GMT_OFFSET = 9;

/***********************************/
/* Local Variables                 */
/***********************************/

/***********************************/
/* Global Variables                */
/***********************************/

/******************************************************************/
/* Implementation                                                 */
/******************************************************************/
/***********************************/
/* Local functions                 */
/***********************************/

/***********************************/
/* Class implementions             */
/***********************************/

/***********************************/
/* Global functions                */
/***********************************/
void scheduler_init() {
    // WiFi接続
    WiFi.begin( WIFI_SSID, WIFI_PASS );
    unsigned long timeout = millis() + 10000; // 10秒タイムアウト
    while ( WiFi.status() != WL_CONNECTED && millis() < timeout ) {
        delay( 500 );
        Serial.print( "." );
    }
    Serial.println( "Connected to WiFi" );
    Serial.printf( "IP Address:%s\r\n", WiFi.localIP().toString().c_str() );
    Serial.printf( "Subnet Mask:%s\r\n", WiFi.subnetMask().toString().c_str() );
    Serial.printf( "Gateway:%s\r\n", WiFi.gatewayIP().toString().c_str() );

    // NTPサーバーから時刻取得
    NTP.begin( "ntp.nict.jp" );
    NTP.waitSet(); // 10秒待機
    time_t now = time( nullptr );
    // タイムゾーン(+9h)を反映
    now += GMT_OFFSET * 3600;
    struct tm* timeinfo = gmtime( &now );
    Serial.println( "Synchronized time" );
    Serial.printf( "Current time: %04d/%02d/%02d %02d:%02d:%02d\r\n", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
                   timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec );
}

void scheduler_task() {
    // スケジュールされたタスクを実行する(定時実行)
}