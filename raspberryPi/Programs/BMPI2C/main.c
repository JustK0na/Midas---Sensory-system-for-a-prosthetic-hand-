//i2cdetect -l 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/time.h>

uint8_t readBuffor;

#include "BMP581i2c.h"

void measureLoop(uint8_t fd, uint8_t fd2, int i, int j){
 int  n=0;
 double pressure = 0;
 double pressure2 = 0;
 long measureIndex = 0;
 struct timeval stop, start;
 gettimeofday(&start, NULL);
 while(1){
   int timeout = 500; //500ms
   int waited = 0;

   while(1){
     readBuffor = readRegister(0x27, fd);
#ifdef DEBUG     
     printf("check data pressure 1\n");
#endif
     if((readBuffor & 0x01))
       break;
     
     usleep(1000);
     waited++;
     if(waited>=timeout){
       printf("\t%d:measurement timeout\n", i);
       break;
     }      
   }
   waited = 0;
   while(1){
     readBuffor = readRegister(0x27, fd2);
#ifdef DEBUG     
       printf("check data pressure 2\n");
#endif
     if((readBuffor & 0x01))
       break;
     
     usleep(1000);
     waited++;
     if(waited>=timeout){
       printf("\t%d:measurement timeout\n", j);
       break;
     }      
   }   

#ifdef DEBUG
     printf("pressure 1\n");
#endif
   pressure = readAndConvertPressure(fd);
   //   pressure = 100000;
#ifdef DEBUG
     printf("pressure 2\n");
#endif
   pressure2 = readAndConvertPressure(fd2);


   measureIndex++;
   char message[256];
   gettimeofday(&stop, NULL);
   snprintf(message, sizeof(message), "%6.0lu;%.2f;%.2f\n",((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec-start.tv_usec)/1000,  pressure,  pressure2);
   //send(sock, message, strlen(message), 0);
   printf("%s", message);
   usleep(500000);
   //    printf("%f\n", pressure); //100hz = 0.01s = 10ms = 10000us
   //n++;
 } 
}

int main() {
    char *filename = "/dev/i2c-1";
    int fd=0, fd2=0;
    
    fd = initI2C(filename, 0x46);
    fd2 = initI2C(filename, 0x47);

    printf("Starting sensor 1 \n");
    startI2C(fd);
    printf("Starting sensor 2 \n");
    startI2C(fd2);


    #ifdef DEBUG
    printf("Sensor 1:\n");
    statusCheck(fd);
    printf("Sensor 2:\n");
    statusCheck(fd2);
    #endif


    printf("setting sensor 1 up\n");
    startUPProcedure(fd);
    printf("setting sensor 2 up\n");
    startUPProcedure(fd2);

    measureLoop(fd, fd2, 1, 2);
    
    close(fd);
    close(fd2);
    return 0;
}
