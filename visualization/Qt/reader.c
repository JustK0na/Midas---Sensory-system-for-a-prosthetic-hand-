#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>


#define MAX_BUFF 1024

int main(){

  int fd;
  char * pathToFIFO = "/tmp/fifo";
  char buf[MAX_BUFF];

  fd = open(pathToFIFO, O_RDONLY);
  while(1<3){
    read(fd, buf, MAX_BUFF);

    printf("Recived: %s\n", buf);
  }

  close(fd);
  

  return 0;
}
