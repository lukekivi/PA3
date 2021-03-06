#include "producer.h"
char *logDir;

/**
 *
 * Producer thread will read from the file and write data to
 * the end of the shared queue
 */
void *producer(void *arg){

    // open the file and read its content line by line
    FILE * fd_in = (FILE*)arg; //  file we are reading from; will be passed in
    ssize_t nread;
    char* buffer = (char *) malloc(sizeof(char) * chunkSize);
    size_t len = chunkSize;
    int lineCount = -1;
    int ACCOUNT_INFO_MAX_LENGTH = 32;

    // Log producer
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

      // Log producer : line
      if (mode == 1 || mode == 3) {
        char* producerLine = (char*)malloc(sizeof(char)*ACCOUNT_INFO_MAX_LENGTH);
        sprintf(producerLine, "producer: line %d\n", lineCount);
        writeLineToFile(logDir, producerLine);
        free(producerLine);
      }

      // Extra semaphore if in bound buffer mode
      if (mode == 2 || mode == 3) {
        sem_wait(&queueNodes);
      }
      sem_wait(&mutexQueue);
      enqueue(q, n);
      sem_post(&mutexQueue);
      sem_post(&staged);


      buffer = (char *) malloc(sizeof(char) * chunkSize); // chunk size for now
    }

    // Creation of special EOF packet to send to consumers.
    char * producerEOF = "producer: line -1\n";
    for (int i = 0; i < nConsumers; i++) {
      // Log producer: line -1
      if (mode == 1 || mode == 3) {
        writeLineToFile(logDir, producerEOF);
      }
      struct Packet* p = (struct Packet*) malloc(sizeof(struct Packet));
      struct Node* n = (struct Node*) malloc (sizeof(struct Node));

      p->lineCount = -1;
      p->line = NULL;
      n->packet = p;
      n->next = NULL;

      // Extra semaphore if in bound buffer mode
      if (mode == 2 || mode == 3) {
        sem_wait(&queueNodes);
      }
      sem_wait(&mutexQueue);
      enqueue(q, n);
      sem_post(&mutexQueue);
      sem_post(&staged);
    }

    buffer = NULL;

    // cleanup and exit
    return NULL;
}
