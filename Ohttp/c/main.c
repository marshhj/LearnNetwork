#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#define BUF_SIZE (1024 * 10)

int g_idx = 0;

void response(int client)
{
    char buf[BUF_SIZE];

    ++g_idx;
    printf("response %d \n", g_idx);
    sprintf(buf, "<html>Welcome to Ohttp! %d<html>", g_idx);
    send(client, buf, strlen(buf), 0);
}

int create_tcp_socket()
{
    int ret = socket(PF_INET, SOCK_STREAM, 0);
    return ret;
}

struct sockaddr_in create_server_addr(int port)
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    return addr;
}


int start_listen(int port)
{
    //1.create socket
    int server_socket = create_tcp_socket();
    if(server_socket == -1)
    {
        printf("Create socket error!\n");
        return -1;
    }

    //create addr
    struct sockaddr_in server_addr = create_server_addr(port);

    //2.bind
    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Bind error!\n");
        return -1;
    }

    //3.listen
    if(listen(server_socket, 10) < 0)
    {
        printf("Listen error!\n");
        return -1;
    }
    printf("Start listening at port : %d \n", port);

    return server_socket;
}


int main()
{
    char recv_buffer[BUF_SIZE];

    int server_socket = start_listen(8800);
    if(server_socket == -1)
    {
        return -1;
    }

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    while(1)
    {
        printf("Start Accept\n");
        //4.accept
        int client_sock = accept(server_socket,
            (struct sockaddr *)&client_addr,
            &client_addr_len);

        if(client_sock == -1)
        {
            printf("Accept error!\n");
            return -1;
        }

        int r = recv(client_sock, recv_buffer, BUF_SIZE, 0);
        if(r == 0)
        {
            close(client_sock);
            continue;
        }

        response(client_sock);
        
        /*
        printf("+++++++++++++++++++++++++++++++++++++++\n");
        printf("%s\n", recv_buffer);
        printf("---------------------------------------\n");
        */
        close(client_sock);
    }

    close(server_socket);
    return 0;
}

