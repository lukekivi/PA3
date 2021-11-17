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
    char *token, *saveptr;
    char *pattern = ",";


    sem_wait(&mutexBalances[id]);
    token = strtok_r(line, pattern, &saveptr);
    
    while (token != NULL) {
        sscanf(token, "%lf", &transaction);
        balance[id] += transaction;    
        token = strtok_r(NULL, pattern, &saveptr);
    }

    balance[id] -= id;
    sem_post(&mutexBalances[id]);

}

// consumer function
void *consumer(void *arg){

    // TODO: keep reading from queue and process the data
    //  feel free to change
    while (1){
        sem_wait(&staged);
        sem_wait(&mutexQueue);
        struct Node *n = dequeue(q);
        sem_post(&mutexQueue);

        if (n->packet->lineCount == -1){
            return NULL;
        }

        parse(n->packet->line);

        free(n);
    }
    return NULL;
}
