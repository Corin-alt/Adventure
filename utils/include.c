#include <string.h>
#include <stdio.h>
#include "other_utils.h"
#include "include.h"

query_t *create_query(char *message, int client_src, int data_type, void *data){
    query_t *q = (query_t*) malloc_check(sizeof(query_t));
    q->message = (char*) malloc_check(sizeof(char) * strlen(message));
    strcpy(q->message, message);
    q->client_src = client_src;
    q->data_type = data_type;
    q->data = data;
    return q;
}


query_t *create_empty_query(){
    query_t *q = (query_t*) malloc_check(sizeof(query_t));
    q->message = NULL;
    q->client_src = -1;
    q->data_type = -1;
    q->data = NULL;

    return q;
}


int is_correct_query(query_t *q){
    return q->message != NULL && q->client_src != -1 && q->data_type != -1 && q->data != NULL; 
}

response_t *create_response(char *message, int client_target, int data_type, void *data){
    response_t *r = (response_t*) malloc_check(sizeof(response_t));
    r->message = (char*) malloc_check(sizeof(char) * strlen(message));
    strcpy(r->message, message);
    r->client_target = client_target;
    r->data_type = data_type;
    r->data = data;

    return r;
}