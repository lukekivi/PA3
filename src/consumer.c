#include "consumer.h"
#include <ctype.h>
char *logDir;

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

    // Parse through line and read in doubles.
    while (token != NULL) {
        sscanf(token, "%lf", &transaction);
        balance[id] += transaction;
        token = strtok_r(NULL, pattern, &saveptr);
    }

    balance[id] -= id; // remove id from balance
    sem_post(&mutexBalances[id]);

}

// consumer function
void *consumer(void *arg){

    int* consumerId = (int*)arg;
    int ACCOUNT_INFO_MAX_LENGTH = 32;

    // Log consumer id
    if (mode == 1 || mode == 3) {
        char * consumerLog = (char*)malloc(ACCOUNT_INFO_MAX_LENGTH*sizeof(char));
        sprintf(consumerLog, "consumer %d\n", *consumerId);
        writeLineToFile(logDir, consumerLog);
        free(consumerLog);
    }

    // keep reading from queue and process the data
    while(1){
        // Dequeue packet from queue. Posts signal if in bound buffered mode.
        sem_wait(&staged);
        sem_wait(&mutexQueue);
        struct Node* n = dequeue(q);
        sem_post(&mutexQueue);
        if (mode == 2 || mode == 3) {
            sem_post(&queueNodes);
        }

        // Log consumer id: line number.
        if (mode == 1 || mode == 3) {
            char* pattern = (char*)malloc(ACCOUNT_INFO_MAX_LENGTH*sizeof(char));
            sprintf(pattern, "consumer %d: line %d\n", *consumerId, n->packet->lineCount);
            writeLineToFile(logDir, pattern);
            free(pattern);
        }

        // Test if the packet is an EOF packet.
          if (n->packet->lineCount == -1) {
            return NULL;
        }

        // Call parse function with our packet.
        parse(n->packet->line);

        freeNode(n);
    }

    consumerId = NULL;

    return NULL;
}
