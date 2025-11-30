//main.c

#include <stdio.h>
#include <stdint.h>
#include "pcg_basic.h"
#include <string.h>
#include <stdlib.h>
#include "hex.h"
#include <time.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAX_PROC 32
#define BUFLEN 128

#define R 0
#define W 1
//se v aa hacer un arreglo por esos se usa typedef 
//el que tiene el indece 0 es el canal de 
#//canala 1 escritura
typedef struct{
    int fda[2];
    int fdb[2];
}pipe_t;

void board_clear(char* board, int size){
    for (int i = 0; i < size * size; i++) {
    board[i] = '+';
    }
}
void print_help(){
    printf("usage: hex <nproc> <nsym> <player>\n");
    printf("nproc: Number of process to run\n");
    printf("nsym: NUmber of simulations per box\n");
    printf("player: Can be \'X\' or \'O\' \n");

}
void error_msg(const char* msg){
    fprintf(stderr, "ERROR: %s\n", msg);
    exit(1);
}
int main(int argc, char** argv){
    if(argc != 5) print_help();

    int nproc = atoi(argv[1]);
    if(nproc < 1 || nproc > MAX_PROC){
        error_msg("Wrong number of prosses");
    }

    int64_t nsim = atoll(argv[2]);
    if(nsim < 1) error_msg("Wrong number of simulations");

    char player = argv[3][0];
    if(player != '0' && player != 'X') error_msg("Weong player");

    int size = atoi(argv[4]);
    if(size < 3 || size > MAX_BOARD_SIDE) error_msg("Wrong board size");

    char oponent = (player == 'X') ? 'O' : 'X';

    if(size<3 || size > MAX_BOARD_SIDE)
            error_msg("Wrong board size");

    //pipes
    pipe_t pipels[MAX_PROC];
    for(int i=0; i<nproc; i++){
        pipe(pipels[i].fda);
        pipe(pipels[i].fdb);
    }

    //Process
    pid_t procls[MAX_PROC];
    //rank nos dice de que proceso estamos hablando
    int rank =0;
    for(int i=0; i < nproc; i++){
        procls[i]=fork();
        //child
        if(procls[i] == 0){
            rank=i+1;
            break;
        }
    }

    // PRNG INICIAIZATION
    pcg32_srandom(time(0), getpid());
    int64_t stats[MAX_BOARD_SIZE];
    int64_t stmp[MAX_BOARD_SIZE];
    char board[MAX_BOARD_SIZE];

    //PROCESO PADRE
    if(rank == 0){
        //configuramos los pipes
        for(int i =0; i<nproc;i++){
            close(pipels[i].fda[W]); //si vamos a read cerramos el q escribe
            close(pipels[i].fdb[R]); //si write cerramos el q lee
        }
        
        board_clear(board,  size);
        board_print(board, size);
        char buffer[BUFLEN];
        int pos;
        while(1){

        printf("%c: ", player);
        fflush(stdout);
        fgets(buffer,BUFLEN, stdin);
        trim(buffer); //se formateo la cadena de comandos
        
        
        if(strcmp(buffer,"quit") == 0) exit(0);
        //printf("> %i,(%i,%i)\n",pos, x,y,x+'A',y);
        //pos=traduce a una posicion de tablero 
        pos = read_move( buffer,size);
        //si el movimiento es correcto
        if(pos <0) continue;
        //se juega
        board[pos] =player;
        board_print(board,size);
        if(board_test(board,size) == player){
            printf("Gana X!\n");
            exit(0);
        }

        //write --> child
        for(int i =0; i< nproc; i++){
            write(pipels[i].fdb[W],board,sizeof(board));
        }
        //luego read <-- child
        for(int i =0; i< MAX_BOARD_SIZE;i++) stats[i] =0;
        for(int i=0; i< nproc; i++){
            read(pipels[i].fda[R],stmp,sizeof(stmp));
            for(int k=0; k< size*size; k++) stats[k] += stmp[k];
        }
        //combina las estadisticaspipels
        //game_stats(board,size,player,nsim,stats); //child process

        pos =game_move(stats,size);
        board[pos] = oponent;
        board_print(board,size);
        if(board_test(board,size) == oponent){
            printf("\nGana 0!\n");
            break;
        }
        board[0] =0;
        for(int i =0; i< nproc; i++){
            write(pipels[i].fdb[W], board, sizeof(board));
        }
        for(int i =0; i< nproc; i++){
            waitpid(procls[i], NULL,0);
        }
        // FInalizacon
        board[0] =0;
        for(int i =0; i<nproc;i++){
            close(pipels[i].fda[R]);
            close(pipels[i].fda[W]);
        }
    }
    }
    //procesos HIJOS
    if(rank >0){
        //configuramos los pipes
        for(int i =0; i<nproc;i++){
            if (i == rank -1) continue;
            close(pipels[i].fda[R]);
            close(pipels[i].fda[W]);
            close(pipels[i].fdb[R]);
            close(pipels[i].fdb[W]);
        }
        close(pipels[rank -1].fda[R]); //write
        close(pipels[rank -1].fdb[W]); //read

        while(1){
            read(pipels[rank-1].fdb[R],board, sizeof(board));

            if(board[0] == 0){
                close(pipels[rank-1].fda[W]);
                close(pipels[rank-1].fdb[W]);
            }

            game_stats(board, size, player, nsim, stats);
            write(pipels[rank-1].fda[W], stats, sizeof(stats));

        }
        
    }
    
    
    return 0;


}
