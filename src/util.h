#pragma once

#include <Arduino.h>

/**
 * @brief HSVカラーをRGB565形式に変換する関数
 *
 * @param hue 色相 (0〜359)
 * @param sat 彩度 (0〜255)
 * @param val 明度 (0〜255)
 * @return uint16_t RGB565形式の16ビットカラー値
 *
 * @note
 * - 色相 (hue) は0〜359の範囲で指定します。それ以外の値は360で割った余りとして扱われます。
 * - 彩度 (sat) と明度 (val) は0〜255の範囲で指定します。
 * - 戻り値はRGB565形式 (5ビット赤、6ビット緑、5ビット青) の16ビット整数です。
 */
static uint16_t hsv_to_rgb( uint16_t hue, uint8_t sat, uint8_t val ) {
    uint16_t r, g, b;
    uint8_t i, f, p, q, t;

    if ( sat == 0 ) {
        r = g = b = val; // achromatic (grey)
        return ( r << 11 ) | ( g << 5 ) | b;
    }

    i = hue / 60; // sector 0 to 5
    f = hue % 60; // factorial part of hue
    p = ( val * ( 255 - sat ) ) >> 8;
    q = ( val * ( 255 - ( ( sat * f ) >> 8 ) ) ) >> 8;
    t = ( val * ( 255 - ( ( sat * ( 60 - f ) ) >> 8 ) ) ) >> 8;
    switch ( i ) {
    case 0:
        r = val;
        g = t;
        b = p;
        break; // red to yellow
    case 1:
        r = q;
        g = val;
        b = p;
        break; // yellow to green
    case 2:
        r = p;
        g = val;
        b = t;
        break; // green to cyan
    case 3:
        r = p;
        g = q;
        b = val;
        break; // cyan to blue
    case 4:
        r = t;
        g = p;
        b = val;
        break; // blue to magenta
    default:
        r = val;
        g = p;
        b = q;
        break; // magenta to red
    }
    return ( r << 11 ) | ( g << 5 ) | b;
}