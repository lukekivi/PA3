#include "consumer.h"
#include <ctype.h>

/**
 * parse lines from the queue, calculate balance change
 * and update to global array
 */
void parse(char *line){

    // get customer id
    int id;
    sscanf(line, "%d", &id);

    // sum up transactions
    double totalChange = 0;
    double transaction; // placeholder for doubles we are reading in
    char *token;
    char *pattern = ",";

    token = strtok(line, pattern);

    while (token != NULL){
        sscanf(token, "%lf", &transaction);
        totalChange += transaction;
        token = strtok(NULL, pattern);
    }
    totalChange -= id;

    // update the global array
    balance[id] += totalChange;
}

// consumer function
void *consumer(void *arg){

    // TODO: keep reading from queue and process the data
    //  feel free to change
    while (1){
        sem_wait(&staged);
        sem_wait(&mutex);
        struct Node *n = dequeue(q);
        sem_post(&mutex);

        if (n->packet->lineCount == -1){
            return NULL;
        }

        parse(n->packet->line);

        free(n);
    }
    return NULL;
}
