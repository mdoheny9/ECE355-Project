#include <stdio.h>
#include "diag/Trace.h"
#include "cmsis/cmsis_device.h"
#include <string.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

/* Clock prescaler for TIM2 timer: no prescaling */
#define myTIM2_PRESCALER ((uint16_t)0x0000)
/* Maximum possible setting for overflow */
#define myTIM2_PERIOD ((uint32_t)0xFFFFFFFF)
/* Clock prescaler for TIM3 timer: no prescaling */
#define myTIM3_PRESCALER ((uint16_t)0x0000)
/* Maximum possible setting for overflow */
#define myTIM3_PERIOD ((uint32_t)(7200000-1)) // should be 100ms delay ***

void myGPIOA_Init(void);
void myGPIOB_Init(void);
void myTIM2_Init(void);
void myTIM3_Init(void);
void myEXTI_Init(void);

void oled_Write(unsigned char);
void oled_Write_Cmd(unsigned char);
void oled_Write_Data(unsigned char);
void oled_config(void);
void refresh_OLED(void);

SPI_HandleTypeDef SPI_Handle;


//
// LED Display initialization commands
//
unsigned char oled_init_cmds[] = {
    0xAE,
    0x20, 0x00,
    0x40,
    0xA0 | 0x01,
    0xA8, 0x40 - 1,
    0xC0 | 0x08,
    0xD3, 0x00,
    0xDA, 0x32,
    0xD5, 0x80,
    0xD9, 0x22,
    0xDB, 0x30,
    0x81, 0xFF,
    0xA4,
    0xA6,
    0xAD, 0x30,
    0x8D, 0x10,
    0xAE | 0x01,
    0xC0,
    0xA0
};

//
// Character specifications for LED Display (1 row = 8 bytes = 1 ASCII character)
// Example: to display '4', retrieve 8 data bytes stored in Characters[52][X] row
//          (where X = 0, 1, ..., 7) and send them one by one to LED Display.
// Row number = character ASCII code (e.g., ASCII code of '4' is 0x34 = 52)
//
unsigned char Characters[][8] = {
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b01011111, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // !
    {0b00000000, 0b00000111, 0b00000000, 0b00000111, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // "
    {0b00010100, 0b01111111, 0b00010100, 0b01111111, 0b00010100,0b00000000, 0b00000000, 0b00000000},  // #
    {0b00100100, 0b00101010, 0b01111111, 0b00101010, 0b00010010,0b00000000, 0b00000000, 0b00000000},  // $
    {0b00100011, 0b00010011, 0b00001000, 0b01100100, 0b01100010,0b00000000, 0b00000000, 0b00000000},  // %
    {0b00110110, 0b01001001, 0b01010101, 0b00100010, 0b01010000,0b00000000, 0b00000000, 0b00000000},  // &
    {0b00000000, 0b00000101, 0b00000011, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // '
    {0b00000000, 0b00011100, 0b00100010, 0b01000001, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // (
    {0b00000000, 0b01000001, 0b00100010, 0b00011100, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // )
    {0b00010100, 0b00001000, 0b00111110, 0b00001000, 0b00010100,0b00000000, 0b00000000, 0b00000000},  // *
    {0b00001000, 0b00001000, 0b00111110, 0b00001000, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // +
    {0b00000000, 0b01010000, 0b00110000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // ,
    {0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // -
    {0b00000000, 0b01100000, 0b01100000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // .
    {0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010,0b00000000, 0b00000000, 0b00000000},  // /
    {0b00111110, 0b01010001, 0b01001001, 0b01000101, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // 0
    {0b00000000, 0b01000010, 0b01111111, 0b01000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // 1
    {0b01000010, 0b01100001, 0b01010001, 0b01001001, 0b01000110,0b00000000, 0b00000000, 0b00000000},  // 2
    {0b00100001, 0b01000001, 0b01000101, 0b01001011, 0b00110001,0b00000000, 0b00000000, 0b00000000},  // 3
    {0b00011000, 0b00010100, 0b00010010, 0b01111111, 0b00010000,0b00000000, 0b00000000, 0b00000000},  // 4
    {0b00100111, 0b01000101, 0b01000101, 0b01000101, 0b00111001,0b00000000, 0b00000000, 0b00000000},  // 5
    {0b00111100, 0b01001010, 0b01001001, 0b01001001, 0b00110000,0b00000000, 0b00000000, 0b00000000},  // 6
    {0b00000011, 0b00000001, 0b01110001, 0b00001001, 0b00000111,0b00000000, 0b00000000, 0b00000000},  // 7
    {0b00110110, 0b01001001, 0b01001001, 0b01001001, 0b00110110,0b00000000, 0b00000000, 0b00000000},  // 8
    {0b00000110, 0b01001001, 0b01001001, 0b00101001, 0b00011110,0b00000000, 0b00000000, 0b00000000},  // 9
    {0b00000000, 0b00110110, 0b00110110, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // :
    {0b00000000, 0b01010110, 0b00110110, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // ;
    {0b00001000, 0b00010100, 0b00100010, 0b01000001, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // <
    {0b00010100, 0b00010100, 0b00010100, 0b00010100, 0b00010100,0b00000000, 0b00000000, 0b00000000},  // =
    {0b00000000, 0b01000001, 0b00100010, 0b00010100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // >
    {0b00000010, 0b00000001, 0b01010001, 0b00001001, 0b00000110,0b00000000, 0b00000000, 0b00000000},  // ?
    {0b00110010, 0b01001001, 0b01111001, 0b01000001, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // @
    {0b01111110, 0b00010001, 0b00010001, 0b00010001, 0b01111110,0b00000000, 0b00000000, 0b00000000},  // A
    {0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b00110110,0b00000000, 0b00000000, 0b00000000},  // B
    {0b00111110, 0b01000001, 0b01000001, 0b01000001, 0b00100010,0b00000000, 0b00000000, 0b00000000},  // C
    {0b01111111, 0b01000001, 0b01000001, 0b00100010, 0b00011100,0b00000000, 0b00000000, 0b00000000},  // D
    {0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b01000001,0b00000000, 0b00000000, 0b00000000},  // E
    {0b01111111, 0b00001001, 0b00001001, 0b00001001, 0b00000001,0b00000000, 0b00000000, 0b00000000},  // F
    {0b00111110, 0b01000001, 0b01001001, 0b01001001, 0b01111010,0b00000000, 0b00000000, 0b00000000},  // G
    {0b01111111, 0b00001000, 0b00001000, 0b00001000, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // H
    {0b01000000, 0b01000001, 0b01111111, 0b01000001, 0b01000000,0b00000000, 0b00000000, 0b00000000},  // I
    {0b00100000, 0b01000000, 0b01000001, 0b00111111, 0b00000001,0b00000000, 0b00000000, 0b00000000},  // J
    {0b01111111, 0b00001000, 0b00010100, 0b00100010, 0b01000001,0b00000000, 0b00000000, 0b00000000},  // K
    {0b01111111, 0b01000000, 0b01000000, 0b01000000, 0b01000000,0b00000000, 0b00000000, 0b00000000},  // L
    {0b01111111, 0b00000010, 0b00001100, 0b00000010, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // M
    {0b01111111, 0b00000100, 0b00001000, 0b00010000, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // N
    {0b00111110, 0b01000001, 0b01000001, 0b01000001, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // O
    {0b01111111, 0b00001001, 0b00001001, 0b00001001, 0b00000110,0b00000000, 0b00000000, 0b00000000},  // P
    {0b00111110, 0b01000001, 0b01010001, 0b00100001, 0b01011110,0b00000000, 0b00000000, 0b00000000},  // Q
    {0b01111111, 0b00001001, 0b00011001, 0b00101001, 0b01000110,0b00000000, 0b00000000, 0b00000000},  // R
    {0b01000110, 0b01001001, 0b01001001, 0b01001001, 0b00110001,0b00000000, 0b00000000, 0b00000000},  // S
    {0b00000001, 0b00000001, 0b01111111, 0b00000001, 0b00000001,0b00000000, 0b00000000, 0b00000000},  // T
    {0b00111111, 0b01000000, 0b01000000, 0b01000000, 0b00111111,0b00000000, 0b00000000, 0b00000000},  // U
    {0b00011111, 0b00100000, 0b01000000, 0b00100000, 0b00011111,0b00000000, 0b00000000, 0b00000000},  // V
    {0b00111111, 0b01000000, 0b00111000, 0b01000000, 0b00111111,0b00000000, 0b00000000, 0b00000000},  // W
    {0b01100011, 0b00010100, 0b00001000, 0b00010100, 0b01100011,0b00000000, 0b00000000, 0b00000000},  // X
    {0b00000111, 0b00001000, 0b01110000, 0b00001000, 0b00000111,0b00000000, 0b00000000, 0b00000000},  // Y
    {0b01100001, 0b01010001, 0b01001001, 0b01000101, 0b01000011,0b00000000, 0b00000000, 0b00000000},  // Z
    {0b01111111, 0b01000001, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // [
    {0b00010101, 0b00010110, 0b01111100, 0b00010110, 0b00010101,0b00000000, 0b00000000, 0b00000000},  // back slash
    {0b00000000, 0b00000000, 0b00000000, 0b01000001, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // ]
    {0b00000100, 0b00000010, 0b00000001, 0b00000010, 0b00000100,0b00000000, 0b00000000, 0b00000000},  // ^
    {0b01000000, 0b01000000, 0b01000000, 0b01000000, 0b01000000,0b00000000, 0b00000000, 0b00000000},  // _
    {0b00000000, 0b00000001, 0b00000010, 0b00000100, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // `
    {0b00100000, 0b01010100, 0b01010100, 0b01010100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // a
    {0b01111111, 0b01001000, 0b01000100, 0b01000100, 0b00111000,0b00000000, 0b00000000, 0b00000000},  // b
    {0b00111000, 0b01000100, 0b01000100, 0b01000100, 0b00100000,0b00000000, 0b00000000, 0b00000000},  // c
    {0b00111000, 0b01000100, 0b01000100, 0b01001000, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // d
    {0b00111000, 0b01010100, 0b01010100, 0b01010100, 0b00011000,0b00000000, 0b00000000, 0b00000000},  // e
    {0b00001000, 0b01111110, 0b00001001, 0b00000001, 0b00000010,0b00000000, 0b00000000, 0b00000000},  // f
    {0b00001100, 0b01010010, 0b01010010, 0b01010010, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // g
    {0b01111111, 0b00001000, 0b00000100, 0b00000100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // h
    {0b00000000, 0b01000100, 0b01111101, 0b01000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // i
    {0b00100000, 0b01000000, 0b01000100, 0b00111101, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // j
    {0b01111111, 0b00010000, 0b00101000, 0b01000100, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // k
    {0b00000000, 0b01000001, 0b01111111, 0b01000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // l
    {0b01111100, 0b00000100, 0b00011000, 0b00000100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // m
    {0b01111100, 0b00001000, 0b00000100, 0b00000100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // n
    {0b00111000, 0b01000100, 0b01000100, 0b01000100, 0b00111000,0b00000000, 0b00000000, 0b00000000},  // o
    {0b01111100, 0b00010100, 0b00010100, 0b00010100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // p
    {0b00001000, 0b00010100, 0b00010100, 0b00011000, 0b01111100,0b00000000, 0b00000000, 0b00000000},  // q
    {0b01111100, 0b00001000, 0b00000100, 0b00000100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // r
    {0b01001000, 0b01010100, 0b01010100, 0b01010100, 0b00100000,0b00000000, 0b00000000, 0b00000000},  // s
    {0b00000100, 0b00111111, 0b01000100, 0b01000000, 0b00100000,0b00000000, 0b00000000, 0b00000000},  // t
    {0b00111100, 0b01000000, 0b01000000, 0b00100000, 0b01111100,0b00000000, 0b00000000, 0b00000000},  // u
    {0b00011100, 0b00100000, 0b01000000, 0b00100000, 0b00011100,0b00000000, 0b00000000, 0b00000000},  // v
    {0b00111100, 0b01000000, 0b00111000, 0b01000000, 0b00111100,0b00000000, 0b00000000, 0b00000000},  // w
    {0b01000100, 0b00101000, 0b00010000, 0b00101000, 0b01000100,0b00000000, 0b00000000, 0b00000000},  // x
    {0b00001100, 0b01010000, 0b01010000, 0b01010000, 0b00111100,0b00000000, 0b00000000, 0b00000000},  // y
    {0b01000100, 0b01100100, 0b01010100, 0b01001100, 0b01000100,0b00000000, 0b00000000, 0b00000000},  // z
    {0b00000000, 0b00001000, 0b00110110, 0b01000001, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // {
    {0b00000000, 0b00000000, 0b01111111, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // |
    {0b00000000, 0b01000001, 0b00110110, 0b00001000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // }
    {0b00001000, 0b00001000, 0b00101010, 0b00011100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // ~
    {0b00001000, 0b00011100, 0b00101010, 0b00001000, 0b00001000,0b00000000, 0b00000000, 0b00000000}   // <-
};

int display_555 = 1;
int firstEdge = 1;
unsigned int Freq = 0;  // Example: measured frequency value (global variable)
unsigned int Res = 0;   // Example: measured resistance value (global variable)

/*** Call this function to boost the STM32F0xx clock to 48 MHz ***/
void SystemClock48MHz(void) {
//
// Disable the PLL
//
    RCC->CR &= ~(RCC_CR_PLLON);
//
// Wait for the PLL to unlock
//
    while (( RCC->CR & RCC_CR_PLLRDY ) != 0 );
//
// Configure the PLL for 48-MHz system clock
//
    RCC->CFGR = 0x00280000;
//
// Enable the PLL
//
    RCC->CR |= RCC_CR_PLLON;
//
// Wait for the PLL to lock
//
    while (( RCC->CR & RCC_CR_PLLRDY ) != RCC_CR_PLLRDY );
//
// Switch the processor to the PLL clock source
//
    RCC->CFGR = ( RCC->CFGR & (~RCC_CFGR_SW_Msk)) | RCC_CFGR_SW_PLL;
//
// Update the system with the new clock frequency
//
    SystemCoreClockUpdate();
}


int main(int argc, char* argv[]) {
	SystemClock48MHz();

	trace_printf("System clock: %u Hz\n", SystemCoreClock);

    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;  /* Enable SYSCFG clock */

    myGPIOA_Init();		/* Initialize I/O port PA (USER button interrupts) */
	myGPIOB_Init();		/* Initialize I/O port PB (555 timer and Function Generator interrupts)*/
	myTIM3_Init();		/* Initialize timer TIM3 */
	myEXTI_Init();		/* Initialize EXTI */

	oled_config();

	while (1) {

		refresh_OLED();
	}

	return 0;

}

//
// LED Display Functions
//

void refresh_OLED( void )
{ // SOURCE: page 30-31 of SSD1780 Data Sheet, Relevant TIM register bit definitions in stm32f051x8.h

	// Buffer size = at most 16 characters per PAGE + terminating '\0'
	unsigned char Buffer[17];

	snprintf( Buffer, sizeof( Buffer ), "Resistance: %5u Ohms", Res );
	/* Buffer now contains your character ASCII codes for LED Display
		- select PAGE (LED Display line) and set starting SEG (column)
		- for each c = ASCII code = Buffer[0], Buffer[1], ...,
			send 8 bytes in Characters[c][0-7] to LED Display
	*/
	oled_Write_Cmd(0xB0);  // select PAGE0
	oled_Write_Cmd(0x00); // lower nibble = 0
	oled_Write_Cmd(0x10); // upper nibble = 0
	for (int i = 0; Buffer[i] != '\0'; i++) {
		unsigned char c = Buffer[i];
		for (int j = 0; j <= 7; j++) {
			oled_Write_Data(Characters[c][j]);
		}
	}
	snprintf( Buffer, sizeof( Buffer ), "Frequency: %5u Hz", Freq );
	/* Buffer now contains your character ASCII codes for LED Display
		- select PAGE (LED Display line) and set starting SEG (column)
		- for each c = ASCII code = Buffer[0], Buffer[1], ...,
			send 8 bytes in Characters[c][0-7] to LED Display
	*/
	oled_Write_Cmd(0xB1);  // select PAGE1
	oled_Write_Cmd(0x00); // lower nibble = 0
	oled_Write_Cmd(0x10); // upper nibble = 0
	for (int i = 0; Buffer[i] != '\0'; i++) {
		unsigned char c = Buffer[i];
		for (int j = 0; j <= 7; j++) {
			oled_Write_Data(Characters[c][j]);
		}
	}

	/* Wait for ~100 ms (for example) to get ~10 frames/sec refresh rate
		- You should use TIM3 to implement this delay (e.g., via polling)
	*/
	TIM3->CNT = 0; //clear count
	TIM3->CR1 |= TIM_CR1_CEN; // start timer
	while (!(TIM3->SR & TIM_SR_UIF)); // wait for timer overflow
	TIM3->SR &= ~TIM_SR_UIF; // clear overflow flag
//	trace_printf("refresh_OLED\n");
}

void oled_Write_Cmd( unsigned char cmd )
{
    // make PB8 = CS# = 1
	GPIOB->BSRR = GPIO_BRR_BR_8; // (1 << 8)
    // make PB9 = D/C# = 0
	GPIOB->BRR = GPIO_BRR_BR_9; // (1 << 9)
    // make PB8 = CS# = 0
	GPIOB->BRR = GPIO_BRR_BR_8;
    oled_Write( cmd );
    // make PB8 = CS# = 1
	GPIOB->BSRR = GPIO_BRR_BR_8;
}

void oled_Write_Data( unsigned char data )
{
    // make PB8 = CS# = 1
	GPIOB->BSRR = GPIO_BRR_BR_8; // (1 << 8)
    // make PB9 = D/C# = 1
	GPIOB->BSRR = GPIO_BRR_BR_9; // (1 << 9)
    // make PB8 = CS# = 0
	GPIOB->BRR = GPIO_BRR_BR_8;
    oled_Write( data );
    // make PB8 = CS# = 1
	GPIOB->BSRR = GPIO_BRR_BR_8;
}

void oled_Write( unsigned char Value )
{ // SOURCE: Interfacing Examples slideset

    /* Wait until SPI2 is ready for writing (TXE = 1 in SPI2_SR) */
	while (!(SPI2->SR & SPI_SR_TXE));

    /* Send one 8-bit character:
       - This function also sets BIDIOE = 1 in SPI2_CR1
    */
    HAL_SPI_Transmit( &SPI_Handle, &Value, 1, HAL_MAX_DELAY );


    /* Wait until transmission is complete (TXE = 1 in SPI2_SR) */
	while (!(SPI2->SR & SPI_SR_TXE));

}

void oled_config( void ) // important
{

// Don't forget to enable GPIOB clock in RCC
// Don't forget to configure PB13/PB15 as AF0
// Don't forget to enable SPI2 clock in RCC

	/* Enable clock for SPI2 peripheral */
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

    SPI_Handle.Instance = SPI2;

    SPI_Handle.Init.Direction = SPI_DIRECTION_1LINE;
    SPI_Handle.Init.Mode = SPI_MODE_MASTER;
    SPI_Handle.Init.DataSize = SPI_DATASIZE_8BIT;
    SPI_Handle.Init.CLKPolarity = SPI_POLARITY_LOW;
    SPI_Handle.Init.CLKPhase = SPI_PHASE_1EDGE;
    SPI_Handle.Init.NSS = SPI_NSS_SOFT;
    SPI_Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    SPI_Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SPI_Handle.Init.CRCPolynomial = 7;

//
// Initialize the SPI interface
//
    HAL_SPI_Init( &SPI_Handle );

//
// Enable the SPI
//
    __HAL_SPI_ENABLE( &SPI_Handle );


    /* Reset LED Display (RES# = PB11):
       - make pin PB11 = 0, wait for a few ms -> use TIM3
       - make pin PB11 = 1, wait for a few ms -> use TIM3
    */
    GPIOB->BRR = (1 << 11);
	/*** "wait for a few ms" ??? ***/
    uint32_t start = TIM3->CNT; // track current count
	while ((TIM3->CNT - start) < myTIM3_PERIOD * 0.05); // wait for timer overflow

	GPIOB->BSRR = (1 << 11);
	/*** "wait for a few ms" ??? ***/
	start = TIM3->CNT; // track current count
	while ((TIM3->CNT - start) < myTIM3_PERIOD * 0.05); // wait for timer overflow


//
// Send initialization commands to LED Display
//
    for ( unsigned int i = 0; i < sizeof( oled_init_cmds ); i++ )
    {
        oled_Write_Cmd( oled_init_cmds[i] );
    }


    /* Fill LED Display data memory (GDDRAM) with zeros:
       - for each PAGE = 0, 1, ..., 7
           set starting SEG = 0
           call oled_Write_Data( 0x00 ) 128 times
    */
	for (int i = 0; i < 7; i++) {
		oled_Write_Cmd(0xB1 | i);  // select PAGE i
		oled_Write_Cmd(0x04); // lower nibble = 0
		oled_Write_Cmd(0x10); // upper nibble = 0
		for (int j = 0; j < 128; j++) {
			oled_Write_Data( 0x00 );
		}
	}
	trace_printf("OLED configured\n");



}

//
// GPIO and EXTI Functions
//
void myGPIOA_Init() {
	/* Enable clock for GPIOA peripheral */
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	/* Configure PA0 (USER button) as input, Ensure no pull-up/pull-down */
	GPIOA->MODER &= ~(GPIO_MODER_MODER0);
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR0);
}

void myGPIOB_Init() {
	/* Enable clock for GPIOB peripheral */
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	/* Configure PB2/3 as input, Ensure no pull-up/pull-down */
	GPIOB->MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3);
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR2 | GPIO_PUPDR_PUPDR3);

	/* Configure PB8/9/11 (Control signals CS#, D/C#, and RES# respectively) as output, Ensure no pull-up/pull-down */
	GPIOB->MODER &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER9 | GPIO_MODER_MODER11);
	GPIOB->MODER |= (GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0 | GPIO_MODER_MODER11_0);
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR8 | GPIO_PUPDR_PUPDR9 | GPIO_PUPDR_PUPDR11);

	/* Configure PB15 (Serial data signal, SDIN = SPI MOSI) as output (AF0) */
	GPIOB->MODER &= ~(GPIO_MODER_MODER15_0);
	GPIOB->MODER |= GPIO_MODER_MODER15_1;
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR15);

	/* Configure PB13 (Clock signal, SCLK = SPI SCK) as output (AF0) */
	GPIOB->MODER &= ~(GPIO_MODER_MODER13_0);
	GPIOB->MODER |= GPIO_MODER_MODER13_1;
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR13);
}

void myTIM2_Init() {
	/* Enable clock for TIM2 peripheral */
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	/* Configure TIM2: buffer auto-reload, count up, stop on overflow,
	 * enable update events, interrupt on overflow only */
	TIM2->CR1 = ((uint16_t)0x008C);

	/* Set clock prescaler value */
	TIM2->PSC = myTIM2_PRESCALER;
	/* Set auto-reloaded delay */
	TIM2->ARR = myTIM2_PERIOD;

	/* Update timer registers */
	TIM2->EGR = ((uint16_t)0x0001);

	/* Assign TIM2 interrupt priority = 0 in NVIC */
	NVIC_SetPriority(TIM2_IRQn, 0);

	/* Enable TIM2 interrupts in NVIC */
	NVIC_EnableIRQ(TIM2_IRQn);

	/* Enable update interrupt generation */
	TIM2->DIER |= TIM_DIER_UIE;
}

void myTIM3_Init() {
	/* Enable clock for TIM3 peripheral */
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	/* Configure TIM3: buffer auto-reload, count up, stop on overflow,
	 * enable update events, interrupt on overflow only */
	TIM3->CR1 = ((uint16_t)0x008C);

	/* Set clock prescaler value */
	TIM3->PSC = myTIM3_PRESCALER;
	/* Set auto-reloaded delay */
	TIM3->ARR = myTIM3_PERIOD;

	/* Update timer registers */
	TIM3->EGR = ((uint16_t)0x0001);
	// TIM3->CNT = 0;
}

void myEXTI_Init() {
	/* Map EXTI0 line to PA0 */
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA;
	/* Map EXTI2 line to PB2 */
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI2;
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PB;
	/* Map EXTI3 line to PB3 */
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI3;
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI3_PB;

	/* EXTI0/2/3 line interrupts: set rising-edge trigger */
	EXTI->RTSR = EXTI_RTSR_TR0 | EXTI_RTSR_TR2 | EXTI_RTSR_TR3;

	/* Unmask interrupts from EXTI0/2/3 line */
	EXTI->IMR = EXTI_IMR_MR0 | EXTI_IMR_MR2 | EXTI_IMR_MR3;

	/*** Assign EXTI0 interrupt priority = 0 in NVIC, Enable EXTI0 interrupts in NVIC ***/
	NVIC_SetPriority(EXTI0_1_IRQn, 0);
	NVIC_EnableIRQ(EXTI0_1_IRQn);
	/* Assign EXTI2/3 interrupt priority = 0 in NVIC, Enable EXTI2/3 interrupts in NVIC */
	NVIC_SetPriority(EXTI2_3_IRQn, 64);
	NVIC_EnableIRQ(EXTI2_3_IRQn);
}

/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void TIM2_IRQHandler() {
	/* Check if update interrupt flag is indeed set */
	if ((TIM2->SR & TIM_SR_UIF) != 0)
	{
		trace_printf("\n*** Overflow! ***\n");

		/* Clear update interrupt flag */
		TIM2->SR &= ~(TIM_SR_UIF);

		/* Restart stopped timer */
		TIM2->CR1 |= TIM_CR1_CEN;
	}
}

void EXTI0_1_IRQHandler() {
	/* Handle user button interrupts */
		if ((GPIOA->IDR & GPIO_IDR_0)) { // USER button pressed (active high)
			return;
		} else { // USER button released
			trace_printf("\nUSER button press detected\n");
			display_555 = !display_555;
			// reset TIM3
			TIM3->CR1 |= TIM_CR1_CEN;
			TIM3->CNT = 0;
		}
		// Clear EXTI2 interrupt pending flag (EXTI->PR).
		EXTI->PR |= EXTI_PR_PR0;
}

/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void EXTI2_3_IRQHandler() {
	/*
	Calculate frequency
	*/
	float period = 1;
	float frequency = 0;

	/* Measure Function Generator signal frequency */
	if (!display_555 && (EXTI->PR & EXTI_PR_PR2)) {
		if (firstEdge) {
			// Clear count register (TIM2->CNT) and start timer (TIM2->CR1).
			TIM2->CNT = 0x00;
			TIM2->CR1 |= TIM_CR1_CEN;
			firstEdge = 0;
		}
		else {
			//	- Stop timer (TIM2->CR1) and read out count register (TIM2->CNT).
			TIM2->CR1 &= ~(TIM_CR1_CEN);
			period = TIM2->CNT;
			period /= 48172691.6; // Average of 10 runs on 1 second

			//	- Calculate signal period and frequency
			frequency = 1.0/period;
			trace_printf("fg period: %.10fs\n", period);
			trace_printf("fg frequency: %fHz\n", frequency);

			firstEdge = 1;
			Freq = frequency;
		}
		// Clear EXTI2 interrupt pending flag (EXTI->PR)
		EXTI->PR |= EXTI_PR_PR2;

	/* Measure 555 timer signal frequency */
	} else if (display_555 && (EXTI->PR & EXTI_PR_PR3)) {
		if (firstEdge) {
			// Clear count register (TIM2->CNT) and start timer (TIM2->CR1).
			TIM2->CNT = 0x00;
			TIM2->CR1 |= TIM_CR1_CEN;
			firstEdge = 0;
		}
		else {
			//	- Stop timer (TIM2->CR1) and read out count register (TIM2->CNT).
			TIM2->CR1 &= ~(TIM_CR1_CEN);
			period = TIM2->CNT;
			period /= 48172691.6; // Average of 10 runs on 1 second

			//	- Calculate signal period and frequency
			frequency = 1.0/period;
			trace_printf("555 period: %.10fs\n", period);
			trace_printf("555 frequency: %fHz\n", frequency);

			firstEdge = 1;
			Freq = frequency;
		}
		// Clear EXTI3 interrupt pending flag (EXTI->PR)
		EXTI->PR |= EXTI_PR_PR3;
	}
}

#pragma GCC diagnostic pop
