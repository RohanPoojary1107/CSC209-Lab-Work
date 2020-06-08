#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "socket.h"

struct server_sock
{
    int sock_fd;
    char buf[BUF_SIZE];
    int inbuf;
};

int main(void)
{
    struct server_sock s;
    s.inbuf = 0;
    int exit_status = 0;

    // Create the socket FD.
    s.sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (s.sock_fd < 0)
    {
        perror("client: socket");
        exit(1);
    }

    // Set the IP and port of the server to connect to.
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) < 1)
    {
        perror("client: inet_pton");
        close(s.sock_fd);
        exit(1);
    }

    // Connect to the server.
    if (connect(s.sock_fd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("client: connect");
        close(s.sock_fd);
        exit(1);
    }

    char *buf = NULL; // Buffer to read name from stdin
    int name_valid = 0;
    while (!name_valid)
    {
        printf("Please enter a username: ");
        fflush(stdout);
        size_t buf_len = 0;
        size_t name_len = getline(&buf, &buf_len, stdin);
        if (name_len < 0)
        {
            perror("getline");
            fprintf(stderr, "Error reading username.\n");
            free(buf);
            exit(1);
        }

        if (name_len - 1 > MAX_NAME)
        { // name_len includes '\n'
            printf("Username can be at most %d characters.\n", MAX_NAME);
            free(buf);

        }else if(strchr(buf, '\r') != NULL || strchr(buf, '\t') != NULL || strchr(buf, ' ') != NULL ||
         strchr(buf, '\v') != NULL || strchr(buf, '\f') != NULL){
            printf("Username cannot contain spaces.\n");
            free(buf);
        }
        else
        {
            // Replace LF+NULL with CR+LF
            buf[name_len - 1] = '\r';
            buf[name_len] = '\n';
            if (write_to_socket(s.sock_fd, buf, name_len + 1))
            {
                fprintf(stderr, "Error sending username.\n");
                free(buf);
                exit(1);
            }
            name_valid = 1;
            free(buf);
        }
    }

    fd_set read_fds;
    int numfd;

    while (1)
    {

        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds);
        FD_SET(s.sock_fd, &read_fds);
        int flag = 0;
        numfd = s.sock_fd + 1;

        if (select(numfd, &read_fds, NULL, NULL, NULL) != 1)
        {
            perror("select");
            exit(1);
        }

        
        if (FD_ISSET(0, &read_fds))
        {
            char *buf2 = malloc(sizeof(char) * (MAX_USER_MSG + 2));
            memset(buf2, 0, (MAX_USER_MSG + 2));

            while ((fgets(buf2, (MAX_USER_MSG + 1), stdin)) != NULL)
            {
                if(buf2[0] == '\n' && flag == 1){
                    memset(buf2, 0, 1);
                    break;
                }
                else if (buf2[strlen(buf2) - 1] == '\n') //situation where \n is encountered before
                { 
                    buf2[strlen(buf2) - 1] = '\r';
                    buf2[strlen(buf2)] = '\n';
                    if (write_to_socket(s.sock_fd, buf2, strlen(buf2)))
                    {
                        fprintf(stderr, "Error sending your message.\n");
                        free(buf2);
                        exit(1);
                    }
                    memset(buf2, 0,strlen(buf2));
                    break;
                }
                else
                {
                    buf2[MAX_USER_MSG] = '\r';
                    buf2[MAX_USER_MSG + 1] = '\n';
                    flag = 1;
                    if (write_to_socket(s.sock_fd, buf2, MAX_USER_MSG + 2))
                    {
                        fprintf(stderr, "Error sending your message.\n");
                        free(buf2);
                        exit(1);
                    }
                    memset(buf2, 0,(MAX_USER_MSG + 2));
                }
            }
            free(buf2);
        }


        if (FD_ISSET(s.sock_fd, &read_fds))
        {
            int client_closed = read_from_socket(s.sock_fd, s.buf, &(s.inbuf));
            if (client_closed == 0)
            {
                char *msg;
                while (!get_message(&msg, s.buf, &(s.inbuf)))
                {
                    char *ptr_msg = strchr(msg, ' ');
                    char *usr = strtok(msg, " ");
                    if(strlen((ptr_msg+1)) > MAX_USER_MSG){
                        printf("Message can be at most %d characters.\n", MAX_USER_MSG);
                        free(msg);
                        exit(1);
                    }
                    if(strlen(usr) > MAX_NAME){
                        printf("Username can be at most %d characters.\n", MAX_NAME);
                        free(msg);
                        exit(1);
                    }                    
                    printf("%s: %s\n", usr, (ptr_msg + 1));
                    free(msg);
                }
            }
            else if (client_closed == -1)
            {
                perror("read");
                exit(1);
            }else if(client_closed == 1){
                fprintf(stderr,"Oops! Server shut down\n");
                exit(1);
            }
        }
    }

    close(s.sock_fd);
    exit(exit_status);
}
