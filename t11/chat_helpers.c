#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "socket.h"
#include "chat_helpers.h"
#include <unistd.h>

int write_buf_to_client(struct client_sock *c, char *buf, int len) {
    
    if(buf[len] != '\0'){
        return 1;
    }
    buf[len] = '\r';
    buf[len+1] = '\n';
    return write_to_socket(c->sock_fd, buf, len+2);
}

int remove_client(struct client_sock **curr, struct client_sock **clients) {
    struct client_sock *temp = *clients;
    struct client_sock *prev;

    if(temp->sock_fd == (*curr)->sock_fd){
        (*clients) = temp->next;
        (*curr) = temp->next;
        close(temp->sock_fd);
        free(temp->username);
        free(temp);
        return 0;
    }

    while(temp != NULL && (temp->sock_fd != (*curr)->sock_fd)){
        prev = temp;
        temp = temp->next;
    }
    
    if(temp == NULL) {
        return 1;
    } // Couldn't find the client in the list, or empty list
    
    prev->next = temp->next;
    (*curr) = temp->next;
    close(temp->sock_fd);
    free(temp->username);
    free(temp);
    return 0;
}

int read_from_client(struct client_sock *curr) {
    return read_from_socket(curr->sock_fd, curr->buf, &(curr->inbuf));
}

int set_username(struct client_sock *curr) {
    // To be completed. Hint: Use get_message().
    char *msg;
    if(!get_message(&msg, curr->buf, &(curr->inbuf))){   //need to check for spaces and length of msg
        if (strlen(msg) > MAX_NAME)
        { 
            printf("Username can be at most %d characters.\n", MAX_NAME);
            free(msg);
            return 1;

        }else if(strchr(msg, '\r') != NULL || strchr(msg, '\t') != NULL || strchr(msg, ' ') != NULL ||
         strchr(msg, '\v') != NULL || strchr(msg, '\f') != NULL){
            printf("Username cannot contain spaces.\n");
            free(msg);
            return 1;

        }else{
        curr->username = malloc(sizeof(char) * strlen(msg) + 1 );
        strncpy(curr->username, msg, strlen(msg) + 1);
        free(msg);
        return 0;
        }
    }
    return 1;
}
