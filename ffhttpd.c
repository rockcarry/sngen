#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>

#ifdef __WIN32__
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define  closesocket close
#endif

#define FFHTTPD_SERVER_PORT     8080
#define FFHTTPD_MAX_CONNECTION  100

static char *g_myweb_head = 
"HTTP/1.1 200 OK\r\n"
"Server: ffhttpd/1.0.0\r\n"
"Content-Type: text/html\r\n"
"Content-Length: 15\r\n"
"Accept-Ranges: bytes\r\n"
"Connection: close\r\n\r\n";

static char *g_myweb_data = 
"hello world !\r\n";

int main(void)
{
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int    server_fd;
    int    conn_fd;
    int    addrlen;
    int    length ;
    char   buffer[1024];

#ifdef __WIN32__
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return;
        exit(1);
    }
#endif

    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(FFHTTPD_SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        printf("failed to open socket !\n");
        exit(1);
    }

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        printf("failed to bind !\n");
        exit(1);
    }

    if (listen(server_fd, FFHTTPD_MAX_CONNECTION) == -1) {
        printf("failed to listen !\n");
        exit(1);
    }

    while (1) {
        addrlen = sizeof(client_addr);
        conn_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen);
        if (conn_fd == -1) {
            printf("failed to accept !\n");
            exit(1);
        }

        length = recv(conn_fd, buffer, 1024, 0);
        printf("%s\n", buffer);

        send(conn_fd, g_myweb_head, strlen(g_myweb_head), 0);
        send(conn_fd, g_myweb_data, strlen(g_myweb_data), 0);
        closesocket(conn_fd);
    }

    closesocket(server_fd);

#ifdef __WIN32__
    WSACleanup();
#endif
}


