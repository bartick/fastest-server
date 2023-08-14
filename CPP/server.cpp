// Server side C program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define PORT 8080

char* parse(char line[], const char symbol[]) {
    char *copy = (char *)malloc(strlen(line) + 1);
    strcpy(copy, line);
    
    char *message;
    char *token = strtok(copy, symbol);
    int current = 0;

    while( token != NULL ) {
      
      token = strtok(NULL, " ");
      if(current == 0){
          message = token;
          if(message == NULL){
              strcpy(message, "");
          }
          return message;
      }
      current = current + 1;
   }
   free(token);
   free(copy);
   return message;
}

char* parse_method(char line[], const char symbol[]) {
    char *copy = (char *)malloc(strlen(line) + 1);
    strcpy(copy, line);
        
    char *message;
    char *token = strtok(copy, symbol);
    int current = 0;

    while( token != NULL ) {
      
      //token = strtok(NULL, " ");
      if(current == 0){
          message = token;
          if(message == NULL){
              strcpy(message, "");
          }
          return message;
      }
      current = current + 1;
   }
   free(copy);
   free(token);
   return message;
}

void send_text_message(int fd, char message[], char head[]) {
    write(fd, head, strlen(head));
    write(fd, message, strlen(message));
    printf("send message: %s \n" , message);
}

char http_header[25] = "HTTP/1.1 200 Ok\r\n";


int main(int argc, char const *argv[]) {
    int server_fd, new_socket, pid; 
    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("In sockets");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("In bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0) {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    
    while(1) {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        //Create child process to handle request from different client
        pid = fork();
        if(pid < 0) {
            perror("Error on fork");
            exit(EXIT_FAILURE);
        }
        
        if(pid == 0){
            char buffer[30000] = {0};
            valread = read( new_socket , buffer, 30000);

            printf("\n buffer message: %s \n", buffer);
            char *parse_string_method = parse_method(buffer, " ");  //Try to get the path which the client ask for
            printf("Client method: %s\n", parse_string_method);

            char *parse_string = parse(buffer, " ");  //Try to get the path which the client ask for
            printf("Client ask for path: %s\n", parse_string);

            char *copy_head = (char *)malloc(strlen(http_header) +200);
            strcpy(copy_head, http_header);

            if(parse_string_method[0] == 'G' && parse_string_method[1] == 'E' && parse_string_method[2] == 'T') {
                //https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types
                printf("Inside GET method\n Parse String: %lu\n", strlen(parse_string));

                if(strlen(parse_string) >= 1) {
                    char path_head[50000] = "";
                    strcat(path_head, "Hello World");
                    strcat(path_head, "\nRequest: ");
                    strcat(path_head, parse_string);
                    strcat(copy_head, "Content-Type: text/plain\r\n\r\n");
                    send_text_message(new_socket, path_head, copy_head);
                }
                printf("\n------------------Server sent----------------------------------------------------\n");
            }
            close(new_socket);
            free(copy_head);  
        } else{
            printf(">>>>>>>>>>Parent create child with pid: %d <<<<<<<<<", pid);
            close(new_socket);
        }
    }
    close(server_fd);
    return 0;
}