#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "192.168.100.10"  // <-- Replace with your laptop’s local IP
#define PORT 5000

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4/IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        return -1;
    }
    printf("Connecting to server... \n");
    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    printf("Connected to server at %s:%d\n", SERVER_IP, PORT);

    // Send test messages
    for (int i = 0; i < 10; i++) {
        char message[100];
        snprintf(message, sizeof(message), "Test message %d", i);
        send(sock, message, strlen(message), 0);
        printf("Sent: %s\n", message);
        sleep(1);
    }

    close(sock);
    return 0;
}
