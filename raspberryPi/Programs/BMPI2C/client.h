void TCPClient(){
  // Create socket
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket creation error");
    exit(-1);
  }
  
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);
  
  // Convert IPv4/IPv6 addresses from text to binary form
  if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
    perror("Invalid address / Address not supported");
    exit(-1);
  }
  printf("Connecting to server... \n");
  // Connect to server
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Connection Failed");
    exit(-1);
  }
  
  printf("Connected to server at %s:%d\n", SERVER_IP, PORT);
}
