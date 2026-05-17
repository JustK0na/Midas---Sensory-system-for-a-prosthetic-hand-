#include <linux/i2c-dev.h>


int initI2C(const char * path_to_device, int addr){
  int fd = open(path_to_device, O_RDWR);
  if(fd < 0){
    perror("Cannot open I2C bus >w<\n");
    close(fd);
    exit(-1);
  }

  if(ioctl(fd, I2C_SLAVE, addr) < 0){
    printf("cannot find device with this address: 0x%02X\n", addr);
    close(fd);
    exit(-1);
  }
  
  
  return fd;
}


uint8_t readRegister(uint8_t readReg, int fd){

  uint8_t data;

  if(write(fd, &readReg, 1) != 1){
    perror("register read: cannot access register\n");
    exit(-1);
  }

  if(read(fd, &data, 1) != 1){
    perror("register read: cannot access data from register\n");
    exit(-1);    
  }
  return data;
}

uint8_t writeRegister(uint8_t writeReg, int fd, uint8_t data){

  uint8_t buf[2] = {writeReg, data};
  
  if(write(fd, buf, 2) != 2){
    perror("register write: cannot write register\n");
    exit(-1);
  }
  
  return 0;

}


double readAndConvertPressure(int fd){

  uint8_t reg = 0x20; //Pers PRESS_DATA_M register -- first of 3
  uint8_t rx[3];
  uint32_t rawPressure = 0;
  double pressure = 0;

  if(write(fd, &reg, 1) != 1){
    printf("failed accessing 0x20 register, during pressure read for %d device\n", fd);
  }

  if(read(fd, rx, 3) != 3){
    printf("failed to read pressure from 0x20 reister, %d device", fd);
  }

    
  rawPressure = (((uint32_t)rx[2] << 16) | ((uint32_t)rx[1] << 8) | ((uint32_t)rx[0]));


  pressure = (double)rawPressure / 64;
  
  return pressure;

}

void startUPProcedure(int fd){
  
  //trzeeba ustawić register 0x36
  //Sprawdzenie OSR - 

  readBuffor = readRegister(0x36, fd);
#ifdef DEBUG
    printf("osr_mode  =  0x%08b\n", readBuffor);
#endif

  //Zmiana rejestru osr Over Sampling Rate -> w tym przypadku 16x oversampling
  // dla ciśnienia
#ifdef DEBUG
    printf("Zmiana rejestru osr...\n");
#endif
  uint8_t osr_data = 0b01011000;
  writeRegister(0x36, fd, osr_data);
  //odczekanie chwili, aby zmiana na pewno zaszła przed kolejnym odczytem
#ifdef DEBUG
    printf("waiting a bit ->\n");
#endif
  usleep(1000);
  //Sprawdzenie czy zmiana zaszła
  readBuffor = readRegister(0x36, fd);
#ifdef DEBUG
    printf("osr_mode  =  0x%08b\n\n\n", readBuffor);
#endif

  //0x37 -> Power mode
  //Sprawdzenie ODR  
  readBuffor = readRegister(0x37, fd);
#ifdef DEBUG
    printf("odr_mode  =  0x%08b\n", readBuffor);
#endif
  
  //Zmiana rejestru ODR, 100Hz, zmiana Powermode - normal mode
#ifdef DEBUG
    printf("Zmiana rejestru odr...\n");
#endif
  uint8_t odr_data = 0b00100001;
  writeRegister(0x37, fd, odr_data);
  
  readBuffor = readRegister(0x37, fd);
#ifdef DEBUG
  printf("odr_mode  =  0x%08b\n", readBuffor);
#endif
  //odczekanie chwili, aby zmiana na pewno zaszła przed kolejnym odczytem
#ifdef DEBUG
    printf("waiting a bit ->\n");
#endif
  usleep(1000);
  
  readBuffor = readRegister(0x37, fd);
#ifdef DEBUG
    printf("odr_mode  =  0x%08b\n\n\n", readBuffor);
#endif

  //Enable data ready interupt
  readBuffor = readRegister(0x15, fd);
  readBuffor = readBuffor | 0x01;
  writeRegister(0x15, fd, readBuffor);

}
void statusCheck(int fd){
  printf("Checking if everything is alright....\n");
  readBuffor = readRegister(0x01, fd);
  printf("CHIP_ID =  0x%02X\n", readBuffor);
  printf("\tShould just not be all zeros, preferably 0x50\n");
  
  readBuffor = readRegister(0x28, fd);
  printf("STATUS =  0x%08b\n", readBuffor);
  printf("\tShould look like that: 0xXXXXX01X\n");
  
  readBuffor = readRegister(0x27, fd);
  printf("INTERRUPT_STATUS  =  0x%08b\n", readBuffor);
  printf("\tShould look like that: 0xXXX1XXXX\n\n\n");
}
void startI2C(int fd){
  //DUMMY READ TO START SPI
  readBuffor = readRegister(0x01, fd);
  readBuffor = readRegister(0x01, fd);
}

