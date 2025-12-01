#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include "pcg_basic.h"
#include "hex.h"

#define MAX_PROC 32
#define BUFLEN 128
#define R 0
#define W 1

typedef struct {
    int fda[2];
    int fdb[2];
} pipe_t;

// --- Board helpers ---
void board_clear(char* board, int size){
    for(int i=0;i<size*size;i++) board[i]='+';
}
int main(int argc,char** argv){
    if(argc!=5){
        printf("usage: hex <nproc> <nsim> <player> <size>\n");
        return 1;
    }

    int nproc = atoi(argv[1]);
    if(nproc<1||nproc>MAX_PROC) { printf("Wrong number of processes\n"); return 1; }

    int64_t nsim = atoll(argv[2]);
    if(nsim<1){ printf("Wrong number of simulations\n"); return 1; }

    char player = argv[3][0];
    if(player != 'X' && player != 'O'){ printf("Wrong player\n"); return 1; }

    int size = atoi(argv[4]);
    if(size<3||size>MAX_BOARD_SIDE){ printf("Wrong board size\n"); return 1; }

    char oponent = (player=='X') ? 'O':'X';

    pipe_t pipels[MAX_PROC];
    pid_t procls[MAX_PROC];
    for(int i=0;i<nproc;i++){
        pipe(pipels[i].fda);
        pipe(pipels[i].fdb);
    }

    int rank=0;
    for(int i=0;i<nproc;i++){
        procls[i]=fork();
        if(procls[i]==0){ rank=i+1; break; }
    }

    pcg32_srandom(time(0), getpid());
    char board[MAX_BOARD_SIZE];
    int64_t stats[MAX_BOARD_SIZE];
    int64_t stmp[MAX_BOARD_SIZE];

    // --- Padre ---
    if(rank==0){
        for(int i=0;i<nproc;i++){
            close(pipels[i].fda[W]);
            close(pipels[i].fdb[R]);
        }

        board_clear(board,size);

        while(1){
            board_print(board,size);
            printf("%c: ", player);
            fflush(stdout);

            char buffer[BUFLEN];
            if(fgets(buffer,BUFLEN,stdin)==NULL) continue;
            trim(buffer);
            if(strcmp(buffer,"quit")==0) break;

            int pos = read_move(buffer,size);
            if(pos<0) continue;
            board[pos] = player;

            // Check player win
            if(board_test(board,size)!= '+'){
                board_print(board,size);
                printf("%c gana!\n", player);
                // Notify children to exit
                board[0]=0;
                for(int i=0;i<nproc;i++)
                    write(pipels[i].fdb[W],board,sizeof(board));
                break;
            }

            // Send board to children
            for(int i=0;i<nproc;i++)
                write(pipels[i].fdb[W],board,sizeof(board));

            // Collect stats
            for(int i=0;i<MAX_BOARD_SIZE;i++) stats[i]=0;
            for(int i=0;i<nproc;i++){
                read(pipels[i].fda[R],stmp,sizeof(stmp));
                for(int k=0;k<size*size;k++) stats[k]+=stmp[k];
            }

            // IA move
            pos = game_move(stats,size);
            board[pos] = oponent;

            if(board_test(board,size)!='+'){
                board_print(board,size);
                printf("%c gana!\n", oponent);
                board[0]=0;
                for(int i=0;i<nproc;i++)
                    write(pipels[i].fdb[W],board,sizeof(board));
                break;
            }
        }

        for(int i=0;i<nproc;i++){
            close(pipels[i].fda[R]);
            close(pipels[i].fdb[W]);
            waitpid(procls[i],NULL,0);
        }
    }

    // --- Hijos ---
    if(rank>0){
        for(int i=0;i<nproc;i++){
            if(i==rank-1) continue;
            close(pipels[i].fda[R]); close(pipels[i].fda[W]);
            close(pipels[i].fdb[R]); close(pipels[i].fdb[W]);
        }
        close(pipels[rank-1].fda[R]);
        close(pipels[rank-1].fdb[W]);

        while(1){
            int n = read(pipels[rank-1].fdb[R],board,sizeof(board));
            if(n<=0 || board[0]==0){
                close(pipels[rank-1].fda[W]);
                close(pipels[rank-1].fdb[R]);
                break;
            }
            game_stats(board,size,player,nsim,stats);
            write(pipels[rank-1].fda[W],stats,sizeof(stats));
        }
    }

    return 0;
}
