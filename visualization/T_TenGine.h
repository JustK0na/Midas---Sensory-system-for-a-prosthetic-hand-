#ifndef T_TENGINE
#define T_TENGINE
#include <termios.h>
#include <time.h>

#define MARGIN 2

typedef struct map_t{
    int col;
    int row;
    char** tiles;

}map_t;


//disable two flags from terminal
//ICANON and ECHO
//returns old Configuration
//also sets read to non-block so it wouldnt wait for user enter
struct termios enableRAWmode();
//Sets old config back, clears screen
struct termios escapeRAWmode(struct termios oldConf);

void clearScreen();
//Prints game name, last clicked button, ticks per second, frames per second
void infoPrint(char* gameName, char key, int tick, clock_t deltaTmicro);
void loadMap(char *filename, map_t *map);
void drawMap(map_t *map);

int rounding(float number);


#endif 
