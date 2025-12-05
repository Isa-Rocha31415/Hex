#include <stdio.h>
#include <stdint.h>
#include "pcg_basic.h"
#include "set.h"
#include <stdlib.h>
# include "hex.h"
#include <string.h>
#define BUFLEN 128
#include<time.h>
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
    int N = size * size;
    int LEFT  = N;
    int RIGHT = N + 1;

    set uf;
    uf_init(&uf, N + 2);

    for(int y=0; y<size; y++){
        for(int x=0; x<size; x++){
            int pos = y*size + x;

            if(board[pos] != 'O') continue;

            // unir bordes
            if(x == 0)      uf_union(&uf, pos, LEFT);
            if(x == size-1) uf_union(&uf, pos, RIGHT);

            // vecinos hexagonales
            int nx[6] = { x, x+1, x-1, x+1, x-1, x };
            int ny[6] = { y-1, y-1, y,   y,   y+1, y+1 };

            for(int k=0; k<6; k++){
                int X = nx[k], Y = ny[k];
                if(X < 0 || X >= size || Y < 0 || Y >= size) continue;
                int npos = Y*size + X;
                if(board[npos] == 'O')
                    uf_union(&uf, pos, npos);
            }
        }
    }

    return uf_find(&uf, LEFT) == uf_find(&uf, RIGHT);
}

int board_test_x(const char* board, int size){
    int N = size * size;
    int TOP    = N;
    int BOTTOM = N + 1;

    set uf;
    uf_init(&uf, N + 2);

    for(int y=0; y<size; y++){
        for(int x=0; x<size; x++){
            int pos = y*size + x;

            if(board[pos] != 'X') continue;

            if(y == 0)      uf_union(&uf, pos, TOP);
            if(y == size-1) uf_union(&uf, pos, BOTTOM);

            int nx[6] = { x, x+1, x-1, x+1, x-1, x };
            int ny[6] = { y-1, y-1, y,   y,   y+1, y+1 };
            
            for(int k=0; k<6; k++){
                int X = nx[k], Y = ny[k];
                if(X < 0 || X >= size || Y < 0 || Y >= size) continue;
                int npos = Y*size + X;
                if(board[npos] == 'X')
                    uf_union(&uf, pos, npos);
            }
        }
    }

    return uf_find(&uf, TOP) == uf_find(&uf, BOTTOM);
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

void link_X(set* uf, const char* board, int size, int pos){
    int N = size*size;
    int TOP = N;
    int BOTTOM = N+1;

    int x = pos % size;
    int y = pos / size;

    // Conectar con bordes si está en la primera o última fila
    if(y == 0)          uf_union(uf, pos, TOP);
    if(y == size - 1)   uf_union(uf, pos, BOTTOM);

    // Conectar con vecinos hexagonales
    int nx[6] = { x, x+1, x-1, x+1, x-1, x };
    int ny[6] = { y-1, y-1, y,   y,   y+1, y+1 };

    for(int k=0; k<6; k++){
        int X = nx[k], Y = ny[k];
        if(X<0 || X>=size || Y<0 || Y>=size) continue;
        int npos = Y*size + X;
        if(board[npos] == 'X') uf_union(uf, pos, npos);
    }
}
void link_O(set* uf, const char* board, int size, int pos){
    int N = size*size;
    int LEFT  = N;
    int RIGHT = N+1;

    int x = pos % size;
    int y = pos / size;

    // Conectar con bordes si está en la primera o última columna
    if(x == 0)        uf_union(uf, pos, LEFT);
    if(x == size - 1) uf_union(uf, pos, RIGHT);

    // Conectar con vecinos hexagonales
    int nx[6] = { x, x+1, x-1, x+1, x-1, x };
    int ny[6] = { y-1, y-1, y,   y,   y+1, y+1 };

    for(int k=0; k<6; k++){
        int X = nx[k], Y = ny[k];
        if(X<0 || X>=size || Y<0 || Y>=size) continue;
        int npos = Y*size + X;
        if(board[npos] == 'O') uf_union(uf, pos, npos);
    }
}

//Simula un juego a partir de la posicion actual del tablero
int game_sim(const char* board, int size, char player){
     char bcopy[MAX_BOARD_SIZE];
    memcpy(bcopy, board, size*size);

    int N = size*size;
    set uf;
    uf_init(&uf, N + 4);  // nodos extra para bordes
    int TOP = N, BOTTOM = N+1, LEFT = N+2, RIGHT = N+3;

    // Conectar fichas iniciales
    for(int pos=0; pos<N; pos++){
        if(bcopy[pos]=='X') link_X(&uf, bcopy, size, pos);
        if(bcopy[pos]=='O') link_O(&uf, bcopy, size, pos);
    }

    // Lista de casillas libres
    int free_moves[N];
    int n_free = 0;
    for(int i=0;i<N;i++) if(bcopy[i]=='+') free_moves[n_free++] = i;

    char turn = player;
    while(n_free > 0){
        int idx = pcg32_boundedrand(n_free);
        int move = free_moves[idx];
        bcopy[move] = turn;

        // actualizar union-find
        if(turn=='X') link_X(&uf,bcopy,size,move);
        else link_O(&uf,bcopy,size,move);

        // comprobar victoria
        if(turn=='X' && uf_find(&uf, TOP) == uf_find(&uf, BOTTOM)) return 'X';
        if(turn=='O' && uf_find(&uf, LEFT) == uf_find(&uf, RIGHT)) return 'O';

        // actualizar lista de libres
        free_moves[idx] = free_moves[n_free-1];
        n_free--;

        turn = (turn=='X') ? 'O' : 'X';
    }

    return '+'; // empate
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
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
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
    clock_gettime(CLOCK_MONOTONIC, &end);

double elapsed_ms =
    (end.tv_sec - start.tv_sec) * 1000.0 +
    (end.tv_nsec - start.tv_nsec) / 1e6;

printf("Tiempo total: %.3f ms\n", elapsed_ms);
        
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