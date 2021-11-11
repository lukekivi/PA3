#include "consumer.h"
#include <ctype.h>

/**
 * parse lines from the queue, calculate balance change
 * and update to global array
 */
void parse(char *line){
    
    // get customer id

    // sum up transactions

    // update the global array


}


// consumer function
void *consumer(void *arg){
    
    // keep reading from queue and process the data
    while(1){
        sem_wait(&staged);
        sem_wait(&mutex);
        struct Node* n = dequeue(q);
        sem_post(&mutex);

        if (n->packet->lineCount == -1) {
            return NULL;
        }

        printf("consumer %d\n", n->packet->lineCount);

        free(n);
    }
    
    return NULL; 
}


