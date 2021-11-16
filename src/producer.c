#include "producer.h"

struct Queue* q;
sem_t mutex;
sem_t staged;
int nConsumers;
char* logDir;
int mode;
/**
 *
 * Producer thread will read from the file and write data to
 * the end of the shared queue
 */
void *producer(void *arg){

    // open the file and read its content line by line

    FILE * fd_in = (FILE*)arg; //  file we are reading from; will be passed in
    ssize_t nread;
    char* buffer = (char *) malloc(sizeof(char) * chunkSize); // chunk size for now
    size_t len = chunkSize;
    int lineCount = 0;
    int ACCOUNT_INFO_MAX_LENGTH = 32;

    // Log
    if (mode == 1 || mode == 3) {
      writeLineToFile(logDir, "producer\n");
    }


    // // read until EOF
    while (nread = getLineFromFile(fd_in, buffer, len) != -1) {
      lineCount++;

      //Send data to the shared queue
      struct Packet* p = (struct Packet*) malloc(sizeof(struct Packet));
      p->line = buffer;
      p->lineCount = lineCount;

      struct Node* n = (struct Node*)malloc(sizeof(struct Node));
      n->next = NULL;
      n->packet = p;

      // Log

      // THIS IS THE ONE CAUSING PROBLEMS.
      //if (mode == 1 || mode == 3) {
        char* producerLine = (char*)malloc(sizeof(char)*ACCOUNT_INFO_MAX_LENGTH);
        sprintf(producerLine, "producer: line %d\n", n->packet->lineCount);
        writeLineToFile(logDir, producerLine);
        free(producerLine);
      //}


      sem_wait(&mutex);
      enqueue(q, n);
      sem_post(&mutex);
      sem_post(&staged);


      buffer = (char *) malloc(sizeof(char) * chunkSize); // chunk size for now
    }

    char * producerEOF = "producer: line -1\n";

    for (int i = 0; i < nConsumers; i++) {

      if (mode == 1 || mode == 3) {
        writeLineToFile(logDir, producerEOF);
      }
      struct Packet* p = (struct Packet*) malloc(sizeof(struct Packet));
      struct Node* n = (struct Node*) malloc (sizeof(struct Node));

      p->lineCount = -1;
      p->line = NULL;
      n->packet = p;
      n->next = NULL;


      sem_wait(&mutex);
      enqueue(q, n);
      sem_post(&mutex);
      sem_post(&staged);
    }

    // cleanup and exit
    return NULL;
}
