#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h> /* inet_ntoa */
#include <netdb.h>     /* gethostname */
#include <sys/socket.h>
#include <errno.h>

#include "socket.h"

/*
 * Initialize a server address associated with the required port.
 * Create and setup a socket for a server to listen on.
 */
void setup_server_socket(struct listen_sock *s)
{
    if (!(s->addr = malloc(sizeof(struct sockaddr_in))))
    {
        perror("malloc");
        exit(1);
    }
    // Allow sockets across machines.
    s->addr->sin_family = AF_INET;
    // The port the process will listen on.
    s->addr->sin_port = htons(SERVER_PORT);
    // Clear this field; sin_zero is used for padding for the struct.
    memset(&(s->addr->sin_zero), 0, 8);
    // Listen on all network interfaces.
    s->addr->sin_addr.s_addr = INADDR_ANY;

    s->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (s->sock_fd < 0)
    {
        perror("server socket");
        exit(1);
    }

    // Make sure we can reuse the port immediately after the
    // server terminates. Avoids the "address in use" error
    int on = 1;
    int status = setsockopt(s->sock_fd, SOL_SOCKET, SO_REUSEADDR,
                            (const char *)&on, sizeof(on));
    if (status < 0)
    {
        perror("setsockopt");
        exit(1);
    }

    // Bind the selected port to the socket.
    if (bind(s->sock_fd, (struct sockaddr *)s->addr, sizeof(*(s->addr))) < 0)
    {
        perror("server: bind");
        close(s->sock_fd);
        exit(1);
    }

    // Announce willingness to accept connections on this socket.
    if (listen(s->sock_fd, MAX_BACKLOG) < 0)
    {
        perror("server: listen");
        close(s->sock_fd);
        exit(1);
    }
}

/*
 * Search the first n characters of buf for a network newline (\r\n).
 * Return one plus the index of the '\n' of the first network newline,
 * or -1 if no network newline is found.
 * Definitely do not use strchr or other string functions to search here. (Why not?)
 */
int find_network_newline(const char *buf, int inbuf)
{

    for (int i = 0; i < inbuf - 1; i++)
    {
        if (buf[i] == '\r' && buf[i + 1] == '\n')
        {
            return (i + 2);
        }
    }
    return -1;
}

/* 
 * Reads from socket sock_fd into buffer *buf containing *inbuf bytes
 * of data. Updates *inbuf after reading from socket.
 *
 * Return -1 if read error or maximum message size is exceeded.
 * Return 0 upon receipt of CRLF-terminated message.
 * Return 1 if socket has been closed.
 * Return 2 upon receipt of partial (non-CRLF-terminated) message.
 */
int read_from_socket(int sock_fd, char *buf, int *inbuf)
{
    char *temp_buf = malloc(sizeof(char) * BUF_SIZE);
    memset(temp_buf, '0', BUF_SIZE);
    int num_read = read(sock_fd, temp_buf, BUF_SIZE - (*inbuf));

    if (num_read == 0)
    {
        free(temp_buf);
        return 1;
    }
    else if (num_read == -1)
    {
        free(temp_buf);
        return -1;
    }
    else
    {
        char *temp_buf2 = malloc(sizeof(char) * BUF_SIZE);
        memcpy(temp_buf2, buf, BUF_SIZE);
        memcpy(temp_buf2 + (*inbuf), temp_buf, num_read);
        free(temp_buf);

        if(find_network_newline(temp_buf2, num_read + (*inbuf)) == -1 && ((*inbuf) + num_read) >= BUF_SIZE){
            
            free(temp_buf2);

            return -1;
        }
        if (find_network_newline(temp_buf2, num_read + (*inbuf)) == -1)
        {
            memcpy(buf, temp_buf2, BUF_SIZE);
            free(temp_buf2);
            *inbuf += num_read;

            return 2;
        }
        else
        {
            memcpy(buf, temp_buf2, BUF_SIZE);
            free(temp_buf2);
            *inbuf += num_read;

            return 0;
        }
    }
}

/*
 * Search src for a network newline, and copy complete message
 * into a newly-allocated NULL-terminated string **dst.
 * Remove the complete message from the *src buffer by moving
 * the remaining content of the buffer to the front.
 *
 * Return 0 on success, 1 on error.
 */
int get_message(char **dst, char *src, int *inbuf)
{

    int index = find_network_newline(src, *inbuf);

    if (index != -1)
    {

        *dst = malloc(sizeof(char) * (index - 1));
        memcpy(*dst, src, index - 1);
        (*dst)[index - 2] = '\0';

        char *temp = malloc(sizeof(char) * (BUF_SIZE));
        memcpy(temp, src + index, (*inbuf) - index);
        memset(src, '0', BUF_SIZE);
        memcpy(src, temp, BUF_SIZE);

        *inbuf -= index;
        free(temp);

        return 0;
    }
    else
    {
        return 1;
    }
}
