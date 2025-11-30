
- Este programa no sabe cuantas barreras encesitamos
 gcc -std=c99 -Wall -o hex main.c hex.c pcg_basic.c

comando del gui.c
 gcc -std=c99 -Wall -o hexgui gui.c gfx.c hex.c pcg_basic.c -lX11 -lm

 ./hex 4 1000 X 5


 la libreria de graficos funciona con geometria para crear el hexagono 



 centro de los hexagonos par ala captura de eventos 


 mv gui.c gamegui.c
 mv gui.c gamegui.h
 cp gemegui.c gamegui.h
 abribr gamegui.h
 definir ahi
 #ifndef GAMEUI_H
 #define  GAMEUI_H
 en las funciones solo poner los prototipos

 lo convirtio en un modulo y luego uso el main de guien el codigo y cambio cosas