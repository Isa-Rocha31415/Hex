#include<stdio.h>
#include<stdlib.h>
#include <stdint.h>
#include <string.h>
#include "gamegui.h"
#include "gfx.h"
#include <math.h>

 //dibuja el texto HEX
void draw_hex_artistic_text(int x0, int y0, rgb color) {
    /*
        Dibuja el texto de HEX
        REcibe las coordenadas (x,y) en formato de entero
        Y la estructura rgb que es el color
    */

    gfx_color(color.r, color.g, color.b);
    
    int h_pattern[7][7] = {
        {1, 0, 0, 0, 1, 0, 0},
        {1, 0, 0, 0, 1, 0, 0},
        {1, 0, 0, 0, 1, 0, 0},
        {1, 1, 1, 1, 1, 0, 0},
        {1, 0, 0, 0, 1, 0, 0},
        {1, 0, 0, 0, 1, 0, 0},
        {1, 0, 0, 0, 1, 0, 0}
    };
    int e_pattern[7][7] = {
        {1, 1, 1, 1, 1, 0, 0},
        {1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 0, 0}
    };
    
    int x_pattern[7][7] = {
        {1, 0, 0, 0, 1, 0, 0},
        {1, 0, 0, 0, 1, 0, 0},
        {0, 1, 0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0},
        {0, 1, 0, 1, 0, 0, 0},
        {1, 0, 0, 0, 1, 0, 0},
        {1, 0, 0, 0, 1, 0, 0}
    };
    
    int pixel_size = 7;
    int spacing = 60;
    
    //H
    for (int row = 0; row < 7; row++) {
        for (int col = 0; col < 7; col++) {
            if (h_pattern[row][col]) {
                for (int py = 0; py < pixel_size; py++) {
                    for (int px = 0; px < pixel_size; px++) {
                        gfx_point(x0 + col * pixel_size + px, 
                                 y0 + row * pixel_size + py);
                    }
                }
            }
        }
    }
    
    //E
    for (int row = 0; row < 7; row++) {
        for (int col = 0; col < 7; col++) {
            if (e_pattern[row][col]) {
                for (int py = 0; py < pixel_size; py++) {
                    for (int px = 0; px < pixel_size; px++) {
                        gfx_point(x0 + col * pixel_size + px + spacing, 
                                 y0 + row * pixel_size + py);
                    }
                }
            }
        }
    }
    
    //X
    for (int row = 0; row < 7; row++) {
        for (int col = 0; col < 7; col++) {
            if (x_pattern[row][col]) {
                for (int py = 0; py < pixel_size; py++) {
                    for (int px = 0; px < pixel_size; px++) {
                        gfx_point(x0 + col * pixel_size + px + 2 * spacing, 
                                 y0 + row * pixel_size + py);
                    }
                }
            }
        }
    }
}

//rectangulo con el texto
void draw_hex_banner(int x0, int y0, int width, int height, rgb bg_color, rgb text_color) {
    /**
     * Recibe dos enteros x,y que son las coordenadas de inicializacion de dibujado del 
     * banner
     * Se define en entero el ancho y el alto del banner 
     * Se define el color del fondo del banner y del color de texto
     * No retorna nada
     * Dibuja el banner con el texto
     */

    // Dibujar fondo del banner
    gfx_color(bg_color.r, bg_color.g, bg_color.b);
    for(int dy = 0; dy < height; dy++){
        for(int dx = 0; dx < width; dx++){
            gfx_point(x0 + dx, y0 + dy);
        }
    }
    
    // centrado
    int text_x = x0 + (width - 150) / 2;  
    int text_y = y0 + (height - 50) / 2;
    draw_hex_artistic_text(text_x, text_y, text_color);
    
    //Bordes
    gfx_color(0, 0, 0);
    for(int dx = 0; dx < width; dx++){
        gfx_point(x0 + dx, y0);
        gfx_point(x0 + dx, y0 + height - 1);
    }
    for(int dy = 0; dy < height; dy++){
        gfx_point(x0, y0 + dy);
        gfx_point(x0 + width - 1, y0 + dy);
    }
}

void draw_rectangle(int x0, int y0, int width, int height, rgb fill){
    /*
    REcibe las coordenadas de inicio de dibujado x0,y0 en entero
    Ademas del entero del ancho y del alto 
    Y la estructura rgb para el color del relleno del rectangulo
    no retorna algo
    Dibuja solo un rectangulo
    */
    gfx_color(fill.r, fill.g, fill.b);
    for(int dy = 0; dy < height; dy++){
        for(int dx = 0; dx < width; dx++){
            gfx_point(x0 + dx, y0 + dy);
        }
    }
    
    //bordes
    gfx_color(0, 0, 100);
    for(int dx = 0; dx < width; dx++){
        gfx_point(x0 + dx, y0);
        gfx_point(x0 + dx, y0 + height - 1);
    }
    for(int dy = 0; dy < height; dy++){
        gfx_point(x0, y0 + dy);
        gfx_point(x0 + width - 1, y0 + dy);
    }
}

int sarea(int ax,int ay, int bx,int by){
    //Calculo el producto vectorial de dos vectores, por lo tanto, el area
    //Recibe en entero los componentes x,y de dos vectores
    //Devuelve el area de estos dos vectores
    return (ax*by - ay*bx);
}

int is_left(int ax,int ay, int bx,int by ){
    //Determina por medio del producto vectorial en donde se dio clic
    //Recibe los dos componentes de dos vectores en x,y
    //SI es positivo, devuelve 1 en que si esta a la izquierda
    //SI es negativo regresa 0
    if(sarea(ax,ay,bx,by) > 0) return 1;
    return 0;
}

int is_right(int ax,int ay, int bx,int by ){
    //Determina por medio del producto vectorial en donde se dio clic
    //Recibe los dos componentes de dos vectores en x,y
    //SI es negativo, devuelve 1 en que si esta a la derecha
    //SI es positivo regresa 0

    if(sarea(ax,ay,bx,by) < 0) return 1;
    return 0;
}

int in_hex(int x, int y, int px, int py){
    /**
     * Determina que si, dado a unas coordenadas, un click fue dado a la derecha o la izquierda
     * Y por lo tanto,determina si está dentro o fuera de un hexágono
     * SI está a la derecha, implica que está afuera del hexágono y regresa 0, sino, regresa
     * 1 diciendo que está dentro del hexágono
     */
    int r = HEX_R;
    int w = HEX_W;
    int h = HEX_H1;
    
    int vx[] = {x + r, x + w, x - w, x - r, x - w, x + w};
    int vy[] = {y + 0, y + h, y + h, y + 0, y - h, y - h};
    
    for(int i = 0; i < 6; i++){
        int ax = vx[(i + 1) % 6] - vx[i];
        int ay = vy[(i + 1) % 6] - vy[i];
        int bx = px - vx[i];
        int by = py - vy[i];
        
        if(is_right(ax, ay, bx, by)) return 0;
    }
    
    return 1;
}

void draw_hex(int x0, int y0, rgb fill) {
    /*
    Dibuja un hexágono dado puntos iniciales definidos como x0,y0 y el color de relleno
    No retorna nada
    Los dibuja a partir de puntos y vertices 
    */
    int r = HEX_R;
    int w = HEX_W;
    int h = HEX_H1;

    int vx[6] = {x0 + r, x0 + w, x0 - w, x0 - r, x0 - w, x0 + w};
    int vy[6] = {y0, y0 + h, y0 + h, y0, y0 - h, y0 - h};

    gfx_color(fill.r, fill.g, fill.b);
    
    for (int y = y0 - h; y <= y0; y++) {
        double t = (double)(y - (y0 - h)) / (double)h;
        int dx = (int)round((r - w) * t);
        int left = x0 - w - dx;
        int right = x0 + w + dx;
        
        for (int x = left; x <= right; x++) {
            gfx_point(x, y);
        }
    }
    
    for (int y = y0; y <= y0 + h; y++) {
        double t = (double)((y0 + h) - y) / (double)h;
        int dx = (int)round((r - w) * t);
        int left = x0 - w - dx;
        int right = x0 + w + dx;
        
        for (int x = left; x <= right; x++) {
            gfx_point(x, y);
        }
    }

    gfx_color(0, 0, 0);
    for (int i = 0; i < 6; i++) {
        int next = (i + 1) % 6;
        gfx_line(vx[i], vy[i], vx[next], vy[next]);
    }
}

int get_hex_centers(int size, int i, int j, int* cx, int* cy){
    /*
    Obtiene los centros de los hexágonos. se necesita el tamaño del tablero
    los indices i,j para determinar cual hexágono 
    Y finalmente dos valores que representan las coordenadas de los centroides
    de los hexágonos
    */
    if(i < 0 || i >= size) return 0;
    if(j < 0 || j >= size) return 0;
    
    *cx = i * (HEX_R + HEX_W) + j * (HEX_R + HEX_W);
    *cy = i * HEX_H1 - j * HEX_H1;
    
    return 1;
}

void draw_board(int x0, int y0, int size){
    /**
     * Dibuja el tablero dado al tamaño entero y dos puntos iniciales enteros 
     * No retorna nada
     * El tablero lo dibuja de fondo con un gris claro y los hexágonos de color blanco
     * 
     */
    rgb white = {255, 255, 255};
    rgb light_gray = {240, 240, 240};
    int cx, cy;
    
    for(int j = 0; j < size; j++){
        for(int i = 0; i < size; i++){
            if(get_hex_centers(size, i, j, &cx, &cy)) {
                if((i + j) % 2 == 0) {
                    draw_hex(x0 + cx, y0 + cy, white);
                } else {
                    draw_hex(x0 + cx, y0 + cy, light_gray);
                }
            }
        }
    }
}

int get_board_hex(int size, int px, int py, int* cx, int* cy){
    /**
     * SE le pasa el tamaño del tablero como entero
     * Además, se le pasa las coordenadas de los píxeles que pulsa el usuario
     * Y los valores de los centroides.
     * La función transforma las coordenadas originales de hexágono, a las coordenadas
     * que tendría el tablero, en específico el motor, 
     * Sabiendo que el motor del tablero no se maneja por un vector 2D sino 1D, se tiene que
     * transformar los indices para indicar donde fue pintado  
     * 
     * Si no está dentro del hexágono, regresa un -1, indicando que fue afuera del tablero
     * 
     */
    int x, y;
    
    for(int j = 0; j < size; j++){
        for(int i = 0; i < size; i++){
            if(get_hex_centers(size, i, j, &x, &y)) {
                if(in_hex(x, y, px, py)){
                    *cx = x;
                    *cy = y;
                    return (size * j + i);
                }
            }
        }
    }
    return -1;
}
/*
int main(){
    // Inicializar ventana gráfica
    gfx_open(WIN_W, WIN_H, "HEX Game");
    gfx_clear_color(255, 255, 255);
    gfx_color(0, 0, 0);
    gfx_clear();
    
    // Coordenadas de origen del tablero
    int x0 = 1 + HEX_R;
    int y0 = (WIN_H / 2) + 50;
    
    // Colores
    rgb blue_sky = {100, 220, 255};
    rgb dark_blue = {0, 0, 100};
    rgb highlight = {255, 200, 100};
    
    draw_hex_banner(50, 15, WIN_W - 100, 80, blue_sky, dark_blue);
    
    draw_board(x0, y0, BRD_SZ);
    
    printf("HEX Game Started!\n");
    printf("Click on any hexagon to select it\n");
    printf("Press 'q' to quit\n");
    
    //bucle principal del juego
    char c;
    int px, py, cx, cy;
    int hex_index;
    
    while(1){
        c = gfx_wait();
        
        if(c == 'q') break;
        
        if(c == 1){
            px = gfx_xpos();
            py = gfx_ypos();
            
            hex_index = get_board_hex(BRD_SZ, px - x0, py - y0, &cx, &cy);
            if(hex_index != -1){
                cx += x0;
                cy += y0;
                draw_hex(cx, cy, highlight);
                printf("Hexagon selected at index: %d\n", hex_index);
            }
        }
    }
    
    printf("Game ended.\n");
    return 0;
}
    */