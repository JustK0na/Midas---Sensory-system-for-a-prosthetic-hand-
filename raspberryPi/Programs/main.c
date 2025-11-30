#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/spi/spidev.h>
#include <string.h>
#include <errno.h>

uint8_t mode = SPI_MODE_0, bitsPerWord = 8;
uint32_t speed = 1000000;

int initSPI(){
  int fd = open("/dev/spidev0.0", O_RDWR);
  if(fd < 0){
    perror("Cannot open SPI device >w<\n");
    close(fd);
    exit(-1);
  }

  if(ioctl(fd, SPI_IOC_WR_MODE, &mode) == -1){
    perror("Cannot set SPI device MODE to 0\n");
    close(fd);
    exit(-1);
  }

  if(ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bitsPerWord) == -1){
    perror("Cannot set SPI device Bits Per Word\n");
    close(fd);
    exit(-1);
  }
  if(ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1){
    perror("Cannot set SPI device MODE to 0\n");
    close(fd);
    exit(-1);
  }

  return fd;
}
uint8_t readRegister(uint8_t readReg, int fd){
  uint8_t tx[2];
  uint8_t rx[2];

  struct spi_ioc_transfer tr = {
    
    .tx_buf = (unsigned long) tx,
    .rx_buf = (unsigned long) rx,
    .len = 2,
    .speed_hz = speed,
    .bits_per_word = bitsPerWord,    
  };

  tx[0] = 0x80 | (readReg & 0x7F);
  tx[1] = 0x00;

  memset(rx, 0, sizeof(rx));
  if(ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 1){
    perror("SPI transfer failed\n");
    exit(-1);
  }
  return rx[1];

}

int main(){
  int fd=0;
  uint8_t readBuffor;
  
  printf("Hello World!:= 0x%08b\n", 0x1A);
  fd = initSPI();

  //DUMMY READ TO START SPI
  readBuffor = readRegister(0x01, fd);
  
  printf("Checking if everything is alright....\n");
  readBuffor = readRegister(0x01, fd);
  printf("CHIP_ID =  0x%02X\n", readBuffor);
  printf("\tShould just not be all zeros, preferably 0x50\n");
  
  readBuffor = readRegister(0x28, fd);
  printf("STATUS =  0x%08b\n", readBuffor);
  printf("\tShould look like that: 0xXXXXX01X\n");
  
  readBuffor = readRegister(0x27, fd);
  printf("INTERRUPT_STATUS  =  0x%08b\n", readBuffor);
  printf("\tShould look like that: 0xXXX1XXXX\n");

  
    
  
  close(fd);
  return(0);
}
