//hex.h
#ifndef HEX_H
#define HEX_H
#include <stdio.h>
#include <stdint.h>
#include "pcg_basic.h"
#define BUFLEN 128

#define MAX_BOARD_SIDE 26
#define MAX_BOARD_SIZE (MAX_BOARD_SIDE*MAX_BOARD_SIDE)
#define MAX_STACK_SIZE (MAX_BOARD_SIZE/2)

typedef enum { NONE, WHITE, BLACK } token_t; //Estas estructuras no fueron utilizadas
typedef enum { TL, TR, BL, BR, ML, MR } dir_t;
//Pipelineacompleto del juego
void board_print(const char* board, int size);
char board_test(const char* board, int size);
void trim(char* str);
int read_move(const char* buffer, int size);
int game_sim(const char* board, int size, char player);
void game_stats(const char* board, int size, char player, int64_t nsym, int64_t* stat);
int game_move(int64_t* stats, int size);
int to_xy(int size, int idn, int* x, int* y);

#endif