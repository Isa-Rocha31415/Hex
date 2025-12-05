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
#include "gfx.h"
#include "gamegui.h"

#define MAX_PROC 32
#define BUFLEN 128
#define R 0
#define W 1

typedef struct {
    int fda[2];
    int fdb[2];
} pipe_t;

void draw_piece(int cx, int cy, char piece){
    if(piece == '+') return;

    rgb player_color   = {50, 100, 255};   // X
    rgb oponent_color  = {255, 80, 80};    // O
    rgb fill;

    if(piece == 'X') fill = player_color;
    else if(piece == 'O') fill = oponent_color;
    else return;

    draw_hex(cx, cy, fill);
}
void draw_pieces(int x0, int y0, int size, char *board){
    int cx, cy;
    for(int j=0;j<size;j++){
        for(int i=0;i<size;i++){
            int pos = j*size + i;
            char p = board[pos];
            if(p!='+'){
                if(get_hex_centers(size, i, j, &cx, &cy)){
                    draw_piece(x0 + cx, y0 + cy, p);
                }
            }
        }
    }
}
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
        gfx_open(WIN_W, WIN_H, "HEX Game");
        gfx_clear_color(255, 255, 255);
        gfx_color(0, 0, 0);
        gfx_clear();
        int x0=1+HEX_R;
        int y0=(WIN_H/2)+50;
        rgb blue_sky = {100, 220, 255};
        rgb dark_blue = {0, 0, 100};
        rgb highlight = {255, 200, 100};
        int px,py;
        int cx,cy;
        int hex_index;
        char c;
        draw_board(x0, y0, BRD_SZ);
        while(1){
            draw_hex_banner(50, 15, WIN_W - 100, 80, blue_sky, dark_blue);
            
            c=gfx_wait();
            if(c=='q') break;
            if(c == 1){
                py = gfx_ypos();
                px = gfx_xpos();
            
                hex_index = get_board_hex(BRD_SZ, px - x0, py - y0, &cx, &cy);
                if(hex_index != -1 && board[hex_index]=='+'){
                    board[hex_index] = player;
                    cx += x0;
                    cy += y0;
                    draw_hex(cx, cy, highlight);
                    printf("Hexagon selected at index: %d\n", hex_index);
                }

                draw_board(x0, y0, BRD_SZ);
                draw_pieces(x0, y0, BRD_SZ, board);
            }
            
            //if() continue;

            // Check player win
            if(board_test(board,size)!= '+'){
                draw_board(x0,y0,size);
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
            hex_index = game_move(stats,size);
            if(board[hex_index] == '+') {
                board[hex_index] = oponent;
            }
            gfx_clear();
            draw_board(x0, y0, BRD_SZ);
            draw_pieces(x0, y0, BRD_SZ, board);
            printf("Hexagon selected at index: %d\n", hex_index);

            if(board_test(board,size)!='+'){
                draw_board(x0,y0,size);
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
