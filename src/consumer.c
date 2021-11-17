#include "consumer.h"
#include <ctype.h>
char *logDir;
int mode;
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
    int ACCOUNT_INFO_MAX_LENGTH = 32;

    // Log
    if (mode == 1 || mode == 3) {
      char * consumerLog = (char*)malloc(ACCOUNT_INFO_MAX_LENGTH*sizeof(char));
      sprintf(consumerLog, "consumer %d\n", *consumerId);
      writeLineToFile(logDir, consumerLog);
      free(consumerLog);
    }

    // keep reading from queue and process the data
    while(1){
        sem_wait(&staged);
        sem_wait(&mutex);
        struct Node* n = dequeue(q);
        sem_post(&mutex);
        // Log consumer id: line number

        if (mode == 1 || mode == 3) {
          char* pattern = (char*)malloc(ACCOUNT_INFO_MAX_LENGTH*sizeof(char));
          sprintf(pattern, "consumer %d: line %d\n", *consumerId, n->packet->lineCount);
          writeLineToFile(logDir, pattern);
          free(pattern);
        }


        if (n->packet->lineCount == -1) {
            return NULL;
        }

        parse(n->packet->line);


        free(n);
    }

    return NULL;
}
