#ifndef GAMEGUI_H
#define GAMEGUI_H

#include <stdint.h>

#define WIN_W 800
#define WIN_H 600

#define PI 3.141592653589
#define RAD (PI/ 180.0)

#define HEX_R 40
#define HEX_W ((int)round(HEX_R*cos(60.0 * RAD)))
#define HEX_H ((int)round(HEX_R*sin(60.0 * RAD)))

#define BRD_SZ 7

typedef struct{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb;

void draw_hex_artistic_text(int x0, int y0, rgb color);
void draw_hex_banner(int x0, int y0, int width, int height, rgb bg_color, rgb text_color);
void draw_rectangle(int x0, int y0, int width, int height, rgb fill);
void draw_hex(int x0, int y0, rgb fill);
int get_hex_centers(int size, int i, int j, int* cx, int* cy);
void draw_board(int x0, int y0, int size);
int get_board_hex(int size, int px, int py, int* cx, int* cy);
int in_hex(int x, int y, int px, int py);
#endif