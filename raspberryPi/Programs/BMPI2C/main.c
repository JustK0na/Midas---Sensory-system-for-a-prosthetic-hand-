#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/spi/spidev.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/time.h>

#define SERVER_IP "192.168.100.10"
#define PORT 5000
int sock = 0;
struct sockaddr_in serv_addr;

uint8_t mode = SPI_MODE_0, bitsPerWord = 8;
uint8_t readBuffor;
uint32_t speed = 1000000;

#include "BMP581.h"
#include "client.h"

pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;


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
   send(sock, message, strlen(message), 0);
   printf("%s", message);
   //    printf("%f\n", pressure); //100hz = 0.01s = 10ms = 10000us
   //n++;
 } 
}

int main(){
  int fd=0, fd2=0;
  
  fd = initSPI("/dev/spidev0.0");
  fd2 = initSPI("/dev/spidev0.1");
  printf("\t\tsensor 1\n");
  startSPI(fd);
  printf("\t\tsensor 2\n");
  startSPI(fd2);
  
#ifdef DEBUG  
    printf("\t\tsensor 1\n");
    statusCheck(fd);
    printf("\t\tsensor 2\n");
    statusCheck(fd2);
#endif

  printf("\t\tsensor 1\n");
  startUPProcedure(fd);
  printf("\t\tsensor 2\n");
  startUPProcedure(fd2);
  
  //TCP Client setup and connect
  TCPClient();
  
  //printf("\n\nMierzenie ciśnienia:\n\n");
  measureLoop(fd, fd2, 1, 2);

  
  close(sock);
  close(fd);
  close(fd2);
  return(0);
}
