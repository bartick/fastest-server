#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 104857600

const char *get_mime_type(const char *mime) {
    if (strcasecmp(mime, "html") == 0 || strcasecmp(mime, "htm") == 0) {
        return "text/html";
    } else if (strcasecmp(mime, "txt") == 0) {
        return "text/plain";
    } else if (strcasecmp(mime, "jpg") == 0 || strcasecmp(mime, "jpeg") == 0) {
        return "image/jpeg";
    } else if (strcasecmp(mime, "png") == 0) {
        return "image/png";
    } else if (strcasecmp(mime, "json") == 0) {
        return "application/json";
    } else {
        return "application/octet-stream";
    }
}

char *url_decode(const char *src) {
    size_t src_len = strlen(src);
    char *decoded = malloc(src_len + 1);
    size_t decoded_len = 0;

    // decode %2x to hex
    for (size_t i = 0; i < src_len; i++) {
        if (src[i] == '%' && i + 2 < src_len) {
            int hex_val;
            sscanf(src + i + 1, "%2x", &hex_val);
            decoded[decoded_len++] = hex_val;
            i += 2;
        } else {
            decoded[decoded_len++] = src[i];
        }
    }

    // add null terminator
    decoded[decoded_len] = '\0';
    return decoded;
}

void build_http_response(const char *data,
                        const char *mime,
                        char *response, 
                        size_t *response_len) {
    // build HTTP header
    const char *mime_type = get_mime_type(mime);
    char *header = (char *)malloc(BUFFER_SIZE * sizeof(char));
    snprintf(header, BUFFER_SIZE,
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: %s\r\n"
             "\r\n",
             mime_type);

    // copy header to response buffer
    *response_len = 0;
    memcpy(response, header, strlen(header));
    *response_len += strlen(header);

    // copy data to response buffer
    memcpy(response + *response_len, data, strlen(data));
    *response_len += strlen(data);

    free(header);
}

void *handle_client(void *arg) {
    int client_fd = *((int *)arg);
    char *buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));

    // receive request data from client and store into buffer
    ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received > 0) {
        // check if request is GET
        regex_t regex;
        regcomp(&regex, "^GET /([^ ]*) HTTP/1.1", REG_EXTENDED);
        regmatch_t matches[2];

        if (regexec(&regex, buffer, 2, matches, 0) == 0) {

            // get the path from request
            char *path = (char *)malloc(BUFFER_SIZE * sizeof(char));
            size_t path_len = matches[1].rm_eo - matches[1].rm_so;
            memcpy(path, buffer + matches[1].rm_so, path_len);
            path[path_len] = '\0';

            // decode url
            char *decoded_path = url_decode(path);

            printf("Request: /%s\n", decoded_path);

            // build HTTP response
            char *response = (char *)malloc(BUFFER_SIZE * 2 * sizeof(char));
            size_t response_len;
            char *response_data = (char *)malloc(BUFFER_SIZE * sizeof(char));
            snprintf(response_data,
                     BUFFER_SIZE,
                     "Hello World!\n"
                     "Request: /%s\n",
                     decoded_path);
            build_http_response(response_data, "txt", response, &response_len);

            // send HTTP response to client
            send(client_fd, response, response_len, 0);

            free(response);
        }
        regfree(&regex);
    }
    close(client_fd);
    free(arg);
    free(buffer);
    return NULL;
}

// handle SIGINT and exit
void stop_connection() {
    printf("\nStopping server...\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    int server_fd;
    struct sockaddr_in server_addr;

    // create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // config socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // bind socket to port
    if (bind(server_fd, 
            (struct sockaddr *)&server_addr, 
            sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listen for connections
    if (listen(server_fd, 10) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);
    while (1) {
        // client info
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int *client_fd = malloc(sizeof(int));

        // accept client connection
        if ((*client_fd = accept(server_fd, 
                                (struct sockaddr *)&client_addr, 
                                &client_addr_len)) < 0) {
            perror("accept failed");
            continue;
        }

        // create a new thread to handle client request
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, (void *)client_fd);
        pthread_detach(thread_id);

        printf("Client connected: %s:%d\n", 
                inet_ntoa(client_addr.sin_addr), 
                ntohs(client_addr.sin_port));
        
        // handle SIGINT
        signal(SIGINT, stop_connection);

    }

    close(server_fd);
    return 0;
}