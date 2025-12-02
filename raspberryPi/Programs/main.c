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

#define DEBUG 0

#define SERVER_IP "10.231.141.172" 
#define PORT 5000
int sock = 0;
struct sockaddr_in serv_addr;

uint8_t mode = SPI_MODE_0, bitsPerWord = 8;
uint8_t readBuffor;
uint32_t speed = 1000000;

#include "BMP581.h"
#include "client.h"


void measureLoop(uint8_t fd){
 int  n=0;
 double pressure = 0;
  while(n<100){
    int timeout = 500; //500ms
    int waited = 0;
    
    while(1){
      readBuffor = readRegister(0x27, fd);
      if((readBuffor & 0x01))
	break;
      
      usleep(1000);
      waited++;
      if(waited>=timeout){
	printf("\tmeasurement timeout\n");
	break;
      }
    }
    pressure = readAndConvertPressure(fd);
    printf("%f\n", pressure); //100hz = 0.01s = 10ms = 10000us
    char message[100];
    snprintf(message, sizeof(message), "%f", pressure);
    send(sock, message, strlen(message), 0);
    printf("Sent: %s\n", message);
    n++;
  } 
}

int main(){
  int fd=0;
  
  fd = initSPI();
  
  startSPI(fd);
  if(DEBUG)
    statusCheck(fd);
  startUPProcedure(fd);

  //TCP Client setup and connect
  TCPClient();
  
  //printf("\n\nMierzenie ciśnienia:\n\n");
  measureLoop(fd);
  
  close(sock);
  close(fd);
  return(0);
}
