#include "consumer.h"
#include <ctype.h>

/**
 * parse lines from the queue, calculate balance change
 * and update to global array
 */


void parse(char *line){
  int id;
  sscanf(line, "%d", &id);

  double totalChange = 0;
  double transaction;
  char *token;
  char *pattern = ",";

  token = strtok(line, pattern);

  while (token != NULL) {
    sscanf(token, "%lf", &transaction);
    totalChange += transaction;
    token = strtok(NULL, pattern);
  }
  totalChange -= id;

  balance[id] += totalChange;
}

// consumer function
void *consumer(void *arg){

    int* consumerId = (int*)arg;

    // Log
    char * consumerLog = (char*)malloc(30*sizeof(char));
    char * log = "output/log.txt";
    sprintf(consumerLog, "consumer %d\n", *consumerId);
    writeLineToFile(log, consumerLog);
    free(consumerLog);


    // keep reading from queue and process the data
    while(1){
        sem_wait(&staged);
        sem_wait(&mutex);
        struct Node* n = dequeue(q);

        // Log consumer id: line number

        char* pattern = (char*)malloc(30*sizeof(char));
        sprintf(pattern, "consumer %d: line %d\n", *consumerId, n->packet->lineCount);
        writeLineToFile(log, pattern);
        free(pattern);

        sem_post(&mutex);
        if (n->packet->lineCount == -1) {
            return NULL;
        }

        //printf("Line: %s\n", n->packet->line);
        sem_wait(&mutex); // can remove this later. this is just for testing,
                          // because it was annoying having all the consumers
                          // printing from parse at once. can be removed.
        parse(n->packet->line);
        sem_post(&mutex); // can be removed later.


        free(n);
    }

    return NULL;
}
