#include "T_TenGine.h"
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <string.h>

struct termios enableRAWmode(){
  struct termios oldConf;
  struct termios rawConf;
  
  tcgetattr(STDIN_FILENO, &oldConf);
  tcgetattr(STDIN_FILENO, &rawConf);

  rawConf.c_lflag &= ~(ICANON | ECHO);

  tcsetattr(STDIN_FILENO,TCSANOW ,&rawConf);
  printf("\033[?25l");
  printf("\033[2J\033[H");
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

  return oldConf;
}

struct termios escapeRAWmode(struct termios oldConf){
  struct termios rawConf;
  tcgetattr(STDIN_FILENO, &rawConf);

  tcsetattr(STDIN_FILENO,TCSANOW ,&oldConf);
  printf("\033[2J\033[H\033[?25h");
  
  return rawConf;
}

void clearScreen(){
  //printf("\033[2J\033[H");
  printf("\033[H");
  fflush(stdout);
}

void infoPrint(char *gameName, char key, int a, clock_t deltaTmicro){
  printf("somethinf button: %c\t%s\ttps %f\tfps: %.2f\n",key, gameName,
	 (float)a*deltaTmicro/1000000.0, 1000000.0/deltaTmicro);
  fflush(stdout);  
}


void loadMap(char *filename, map_t *map){
  FILE *fp;
  int cols=0, rows=0;
  long cursorPos = 0;
  fp = fopen(filename,"rb");

  if(fp==0){
    perror("Couldn't open map file :(\n");
    exit(-1);
  }
  fscanf(fp, "%d", &rows);
  fscanf(fp, "%d", &cols);
  cursorPos=ftell(fp);
  cursorPos++;
  map->row=rows;
  map->col=cols;


  fseek(fp, 0, SEEK_END);
  rewind(fp);
  fseek(fp, cursorPos, SEEK_SET);

  map->tiles = (char **) malloc(sizeof(char *) * rows);
  for(int i=0; i<rows; i++){
    map->tiles[i]=(char *) malloc(sizeof(char));
  }
  for(int i=0; i<rows; i++){
	fread(map->tiles[i], sizeof(char), cols, fp);
  }
  map->col = cols - 1;
  fclose(fp);
}

void drawMap(map_t *map){
  for(int i=0; i<map->row; i++){
    for(int j=0; j<map->col+1; j++){
      if(map->tiles[i][j]=='`')
        printf("\033[30m%c\033[0m", map->tiles[i][j]);
      else
        printf("%c", map->tiles[i][j]);
    }
  }
}

int rounding(float number){
  int tmp;

  if(number-(int)number >= 0.51)
    tmp = (int)number + 1;
  else
    tmp = (int)number;

  return tmp;
}
