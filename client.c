#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

#define arraylength 5
#define PORT 8766

//Bold high intensity text
#define BHBLK "\e[1;90m"
#define BHRED "\e[1;91m"
#define BHGRN "\e[1;92m"
#define BHYEL "\e[1;93m"
#define BHBLU "\e[1;94m"
#define BHMAG "\e[1;95m"
#define BHCYN "\e[1;96m"
#define BHWHT "\e[1;97m"

//Reset
#define reset "\e[0m"

int main(int argc, char const *argv[]) {

    // We create a sock variable
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    
    // This call results in a stream socket with the TCP
    // protocol providing the underlying communication
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    // socket between server/client
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Establishes a connection to a peer
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    char message[100];
    printf(BHYEL);
    printf("\nHey!! This is Bamby, your personal assistant.\nFor any help, refer to the documentation by typing `doc`\n\n");
    
    // It keeps taking input from the user
    while (1) {
    	printf(BHCYN);
    	printf("You > ");
        fgets(message, sizeof(message), stdin);
        send(sock, message, strlen(message), 0);
        char buffer[99999] = {0};
        recv(sock, buffer, sizeof(buffer), 0);
        if (strcmp(buffer, "exit") == 0) {
            close(sock);
            exit(0);
        }
        printf(BHRED); //Set the text to the color red
        printf("Bamby > %s\n\n", buffer);
    }
    close(sock);
}