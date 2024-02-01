#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024

void send_response(int clientfd, const char* response) {
    if (send(clientfd, response, strlen(response), 0) <= 0) {
        perror("send");
    }
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(9080),
        .sin_addr.s_addr = INADDR_ANY,
        .sin_zero = {0}
    };

    if (bind(sockfd, (const struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(sockfd);
        return 1;
    }

    if (listen(sockfd, 10) == -1) {
        perror("listen");
        close(sockfd);
        return 1;
    }

    printf("Listening on port 9080...\nSite online on http://localhost:9080/\n");

    while (1) {
        int clientfd = accept(sockfd, NULL, NULL);
        if (clientfd == -1) {
            perror("accept");
            continue;
        }

        printf(" - Connection accepted\n");

        char buffer[MAX_BUFFER_SIZE] = {0};
        if (recv(clientfd, buffer, MAX_BUFFER_SIZE - 1, 0) <= 0) {
            perror("recv");
            close(clientfd);
            continue;
        }

        // Parse the HTTP request to extract the requested file
        char file_request[MAX_BUFFER_SIZE];
        sscanf(buffer, "GET %s HTTP/1.1", file_request);

        // If no specific file is requested, default to "index.html"
        if (strcmp(file_request, "/") == 0 || strcmp(file_request, "/index.html") == 0) {
            FILE* f = fopen("index.html", "r");
            if (f == NULL) {
                perror("fopen");
                close(clientfd);
                continue;
            }

            // Read file content
            char response[MAX_BUFFER_SIZE * 2];
            size_t len = fread(response, 1, sizeof(response), f);
            fclose(f);

            // Send HTTP response
            char http_response[MAX_BUFFER_SIZE * 2];
            snprintf(http_response, sizeof(http_response), "HTTP/1.1 200 OK\r\nContent-Length: %zu\r\n\r\n%s", len, response);

            send_response(clientfd, http_response);
        } else {
            // If a specific file is requested, handle it here
            char not_found_response[] = "HTTP/1.1 404 Not Found\r\nContent-Length: 13\r\n\r\n404 Not Found";
            send_response(clientfd, not_found_response);
        }

        close(clientfd);
    }

    close(sockfd);
    return 0;
}

// lel
