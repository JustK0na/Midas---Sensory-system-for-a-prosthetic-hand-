#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(){

  int fd;
  char * pathToFIFO = "/tmp/fifo";
  char *labels[3] = {"Sensor1: ", "Sensor2: ", "Sensor3: "};
  char message[1024];
  int data[3] = {0,0,0};
  
  srand(time(NULL));

  mkfifo(pathToFIFO, 0666);

  fd = open(pathToFIFO, O_WRONLY);

  while(1){
    for(int i = 0; i<3; i++ ){
      data[i] = rand()%1023;
      snprintf(message, sizeof(message),"%s%d\n",labels[i], data[i]);
      write(fd, message, strlen(message));
      usleep(500000);
    }
  }


  
  close(fd);
  unlink(pathToFIFO);

  return 0;
}
