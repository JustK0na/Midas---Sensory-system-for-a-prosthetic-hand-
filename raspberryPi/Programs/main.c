#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include <sys/ioctl.h>


int main(){

  int file;
  int adapter = 6;
  char filename[20];

  printf("Hello world\n");
  
  snprintf(filename, 19, "/dev/i2c-%d", adapter);
  file = open(filename, O_RDWR);

  if(file < 0){
    perror("Device doesn't exist or cannot be accessed\n");    
    exit(1);
  }


  int addr = 0x46;


  if(ioctl(file, I2C_SLAVE, addr)<0){
    perror("wrond address\n");    
    exit(1);    
  }



  
  
  

  

  return 0;
}

// gcc main.c -Wall -pedantic -o main 
//i2cdetect -l
