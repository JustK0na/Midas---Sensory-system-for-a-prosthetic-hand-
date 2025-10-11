#include "T_TenGine.h"
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define MICROSECONDS 1000000
#define FRAMERATE 60

int main(){
  struct termios oldConf = enableRAWmode();
  clock_t nowClock, pastClock, deltaTMacro=0;
  char key = 'c';
  char name[32] = "Midas";
  map_t map;
  int tick = 0;

  loadMap("fingerTips.T_Tmap", &map);
  pastClock = clock();

  while(1){
    nowClock = clock();
    read(STDIN_FILENO, &key, 1);
    
    if(key == 'q')
      break;

    deltaTMacro = MICROSECONDS*(nowClock - pastClock)/CLOCKS_PER_SEC;
    tick ++;
    if(deltaTMacro > (float)MICROSECONDS/FRAMERATE){
      pastClock = clock();
      clearScreen();
      infoPrint(name, key, tick, deltaTMacro);
      drawMap(&map);

      tick = 0;
    }
    
    
  }
  escapeRAWmode(oldConf);

  return 0;
}

// compile
// gcc -Wall -pedantic -o fingerTips main.c T_TenGine.c

