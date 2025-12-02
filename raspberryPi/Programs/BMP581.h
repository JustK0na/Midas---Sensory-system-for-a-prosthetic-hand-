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

uint8_t writeRegister(uint8_t readReg, int fd, uint8_t data){
  uint8_t tx[2];
  uint8_t rx[2];


  tx[0] = readReg & 0x7F;
  tx[1] = data;
  
  struct spi_ioc_transfer tr = {
    .tx_buf = (unsigned long) tx,
    .rx_buf = (unsigned long) rx,
    .len = 2,
    .speed_hz = speed,
    .bits_per_word = bitsPerWord,    
  };

  if(ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 1){
    perror("SPI write failed\n");
    exit(-1);
  }
  return 0;

}
double readAndConvertPressure(int fd){

  uint8_t tx[4];
  uint8_t rx[4];
  uint32_t rawPressure = 0;
  double pressure = 0;
  
  struct spi_ioc_transfer tr = {
    
    .tx_buf = (unsigned long) tx,
    .rx_buf = (unsigned long) rx,
    .len = 4,
    .speed_hz = speed,
    .bits_per_word = bitsPerWord,    
  };

  tx[0] = 0x80 | (0x20 & 0x7F);
  tx[2] = tx[3] = tx[1] = 0x00;

  memset(rx, 0, sizeof(rx));
  if(ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 1){
    perror("SPI transfer failed\n");
    exit(-1);
  }

  rawPressure = (((uint32_t)rx[3] << 16) | ((uint32_t)rx[2] << 8) | ((uint32_t)rx[1]));


  pressure = (double)rawPressure / 64;
  
  return pressure;

}
void startUPProcedure(int fd){
  
  //trzeeba ustawić register 0x36
  //Sprawdzenie OSR - 

  readBuffor = readRegister(0x36, fd);
  if(DEBUG)
    printf("osr_mode  =  0x%08b\n", readBuffor);

  //Zmiana rejestru osr Over Sampling Rate -> w tym przypadku 16x oversampling
  // dla ciśnienia
  if(DEBUG)
    printf("Zmiana rejestru osr...\n");
  uint8_t osr_data = 0b01011000;
  writeRegister(0x36, fd, osr_data);
  //odczekanie chwili, aby zmiana na pewno zaszła przed kolejnym odczytem
  if(DEBUG)
    printf("waiting a bit ->\n");
  usleep(1000);
  //Sprawdzenie czy zmiana zaszła
  readBuffor = readRegister(0x36, fd);
  if(DEBUG)
    printf("osr_mode  =  0x%08b\n\n\n", readBuffor);

  //0x37 -> Power mode
  //Sprawdzenie ODR  
  readBuffor = readRegister(0x37, fd);
  if(DEBUG)
    printf("odr_mode  =  0x%08b\n", readBuffor);
  
  //Zmiana rejestru ODR, 100Hz, zmiana Powermode - normal mode
  if(DEBUG)
    printf("Zmiana rejestru odr...\n");
  uint8_t odr_data = 0b00101001;
  writeRegister(0x37, fd, odr_data);
  
  readBuffor = readRegister(0x37, fd);
  if(DEBUG)
    printf("odr_mode  =  0x%08b\n", readBuffor);
  //odczekanie chwili, aby zmiana na pewno zaszła przed kolejnym odczytem
  if(DEBUG)
    printf("waiting a bit ->\n");
  usleep(1000);
  
  readBuffor = readRegister(0x37, fd);
  if(DEBUG)
    printf("odr_mode  =  0x%08b\n\n\n", readBuffor);


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
void startSPI(int fd){
  //DUMMY READ TO START SPI
  readBuffor = readRegister(0x01, fd);
  readBuffor = readRegister(0x01, fd);
}
