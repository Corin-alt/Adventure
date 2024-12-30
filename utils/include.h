#ifndef H_INCLUDE
#define H_INCLUDE

#define INT_TYPE 0
#define US_TYPE 1
#define STR_TYPE 2
#define MAP_TYPE 3
#define HERO_TYPE 4
#define INPUT_TYPE 5


#

typedef struct{
    char *message;
    int client_src;
    int data_type;
    void *data;
}query_t;

typedef struct{
    char *message;
    int client_target;
    int data_type;
    void *data;
}response_t;

query_t *create_query(char *message, int client_src, int data_type, void *data);
response_t *create_response(char *message, int client_target, int data_type, void *data);



query_t *create_empty_query();
int is_correct_query(query_t *q );



void send_query(query_t *query);
void send_response(response_t *response);




#endif