#include <stdio.h>
#include <stdint.h>
#include "pcg_basic.h"
#include <stdlib.h>
# include "hex.h"
#include <string.h>
#define BUFLEN 128

// Convierte (x,y) a índice en el arreglo 1D
int to_ind(int size, int x, int y){
    if(x < 0 || x >= size) return -1;
    if(y < 0 || y >= size) return -1;
    return size * y + x;
}

void board_print(const char* board, int size){
    printf("  ");
    for(int n=0; n<size; n++){
        printf("%c   ", 'A'+n);
    }
    printf("\n");
    for(int j=0; j<size; j++){
        for(int n=0; n<j; n++) printf("  ");
        printf("%i", j+1);
        for(int i=0; i<size; i++){
            char c = board[size*j + i];
            if(c == '+') printf("   +");
            else if(c == 'X') printf("   X");
            else if(c == 'O') printf("   O");
            else printf("   ?");
        }
        printf("\n");
    }
    printf("\n");
}

int to_xy(int size, int idn, int* x, int* y){
    *y = idn / size;
    *x = idn % size;
    return 0;
}

int board_test_o(const char* board, int size){
    char bcopy[MAX_BOARD_SIZE * MAX_BOARD_SIZE];
    int s = (size * size) >> 1;
    int* stack = malloc(s * sizeof(int));
    int k=0; // índice stack

    for(int i=0; i<size*size; i++) bcopy[i] = board[i];

    // Buscamos tokens 'O' en borde vertical izquierdo
    for(int i=0; i<size*size; i+=size){
        if(bcopy[i] == 'O'){
            stack[k++] = i;
            bcopy[i] = '+';
        }
    }
    if(k == 0){
        free(stack);
        return 0; // no hay camino
    }
    int x, y;

    while(k > 0){
        int pos = stack[--k];
        to_xy(size, pos, &x, &y);
        if(x == size-1){
            free(stack);
            return 1; // ganó O
        }

        int nbor[6];
        nbor[0] = to_ind(size, x, y-1);    // TL
        nbor[1] = to_ind(size, x+1, y-1);  // TR
        nbor[2] = to_ind(size, x-1, y);    // BL
        nbor[3] = to_ind(size, x+1, y);    // BR
        nbor[4] = to_ind(size, x-1, y+1);  // BL vecino
        nbor[5] = to_ind(size, x, y+1);    // BR vecino

        for(int i=0; i<6; i++){
            if(nbor[i] < 0) continue;
            if(bcopy[nbor[i]] == 'O'){
                stack[k++] = nbor[i];
                bcopy[nbor[i]] = '+';
    }
}
    }
    return 0;
}

int board_test_x(const char* board, int size){
    char bcopy[MAX_BOARD_SIZE * MAX_BOARD_SIZE];
    int s = (size * size) >> 1;
    int* stack = malloc(s * sizeof(int));
    int k=0;

    for(int i=0; i<size*size; i++) bcopy[i] = board[i];

    // Buscamos tokens 'X' en borde horizontal superior
    for(int i=0; i<size; i++){
        if(bcopy[i] == 'X'){
            stack[k++] = i;
            bcopy[i] = '+';
        }
    }
    if(k == 0){
        free(stack);
        return 0;
    }
    int x, y;
    while(k > 0){
        int pos = stack[--k];
        to_xy(size, pos, &x, &y);
        if(y == size-1){
            free(stack);
            return 1; // ganó X
        }
        int nbor[6];
        nbor[0] = to_ind(size, x, y-1);
        nbor[1] = to_ind(size, x+1, y-1);
        nbor[2] = to_ind(size, x-1, y);
        nbor[3] = to_ind(size, x+1, y);
        nbor[4] = to_ind(size, x-1, y+1);
        nbor[5] = to_ind(size, x, y+1);

        for(int i=0; i<6; i++){
            if(nbor[i] < 0) continue;
            if(bcopy[nbor[i]] == 'X'){
                stack[k++] = nbor[i];
                bcopy[nbor[i]] = '+';
            }
        }
    }

    return 0;
}

//Evalua el estado del tablero
//Regresa:
//"X": Si el jugador "X" conecto ambos lados del tablero
//"O": Si el jugador "O" conecto ambos lados del tablero
//
char board_test(const char* board, int size){
    if(board_test_o(board, size)) return 'O';
    if(board_test_x(board, size)) return 'X';
    return '+';
}


//Simula un juego a partir de la posicion actual del tablero
int game_sim(const char* board, int size, char player){
    char bcopy[MAX_BOARD_SIZE];
    //Copia local del tablero
    
    for(int i =0; i < size* size; i++) bcopy[i] = board[i];
    char turn =player; // O o X
    char out;
    while(1){
        int move;
        move= pcg32_boundedrand(size*size);
        if(bcopy[move] != '+') continue;

        bcopy[move] = turn;
        out = board_test(bcopy,size);
        if(out != '+') return out;

        turn = (turn == 'X') ? 'O':'X';
    }
    return '+';
}

//Simula muchos juegos y obtiene estadisticas, luego decide donde jugar
// board
//size
//nsym numero de simulaciones a ejecutar
//Stat
void game_stats(const char* board, int size, char player, int64_t nsym, int64_t* stat){
    char bcopy[MAX_BOARD_SIZE];

    //copia local del tablero
    for(int i =0; i< size*size; i++)bcopy[i] = board[i] ;

    //inicializacionmove
    for(int i =0; i<size*size;i++) stat[i] =0;

    //elegimos donde jugar
    char other = (player == 'X') ? 'O' : 'X';
    char out;
    for(int64_t n =0; n < nsym; n++){
        for(int k =0; k < size*size; k++){
            if(bcopy[k] != '+') continue;
            bcopy[k] =player;
            out = game_sim(bcopy,size,other);
            bcopy[k] ='+';

            if(out == player) stat[k]++;
            else stat[k]--;

            }
        }
        
}

int game_move(int64_t* stats,int size ){
    int move =0;
    for(int i =1; i<size*size; i++){
        if(stats[i] > stats[move]) move=i;
    }
    return move;
}
int is_space(char c){
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}
//remueve los caracteres en blanco deel inicio y del final de la cadena 
void trim (char* str){

 int start = 0;
    int end = strlen(str) - 1;
    while(str[start] && is_space(str[start])) start++;
    while(end >= start && is_space(str[end])) end--;
    int j = 0;
    for(int i = start; i <= end; i++)
        str[j++] = str[i];

    str[j] = '\0'; // terminar la cadena
}
int read_move(const char* buffer, int size){
    char* buf_copy = malloc(strlen(buffer) + 1);
    strcpy(buf_copy, buffer);
    trim(buf_copy);
    
    int r, c;
    if (buf_copy[0] >= 'A' && buf_copy[0] <= 'Z'){
        c = buf_copy[0] - 'A';
    }
    else if (buf_copy[0] >= 'a' && buf_copy[0] <= 'z'){
        c = buf_copy[0] - 'a';
    }
    else {
        free(buf_copy);
        return -1;
    }

    r = atoi(buf_copy + 1) - 1;  // convertir a índice 0-based
    free(buf_copy);
    
    return to_ind(size, c, r);
}
