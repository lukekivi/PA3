#include "producer.h"

struct Queue* q;
sem_t mutex;
sem_t staged;
/**
 *
 * Producer thread will read from the file and write data to
 * the end of the shared queue
 */
void *producer(void *arg){

    printf("In the producer.\n");

    //TODO: open the file and read its content line by line

    FILE * fd_in = (FILE*)arg; //  file we are reading from; will be passed in
    ssize_t nread;
    char buffer[chunkSize]; // chunk size for now
    size_t len = chunkSize;
    int lineCount = 0;

    // read until EOF
    while (nread = getLineFromFile(fd_in, buffer, len) != -1) {
      lineCount++;
      //Send data to the shared queue
      struct Packet* p = (struct Packet*) malloc(sizeof(struct Packet));
      p->line = buffer;
      p->lineCount = lineCount;

      // Testing to see that the line and packet are the same.
      printf("Line: %s", buffer);
      printf("Packet: %s", p->line);

      struct Node* n = (struct Node*)malloc(sizeof(struct Node));
      n->next = NULL;
      n->packet = p;

      // Testing to see that node succesfully copied.
      printf("Node: %s", n->packet->line);

      sem_wait(&mutex);
      enqueue(q, n);
      sem_post(&mutex);
      sem_post(&staged);
      free(n);
      free(p);
    }

    //When reaching the end of the file, send EOF message
    printf("Reached EOF.\n");
    printf("Printing queue:\n");
    // Print queue for testing.
    printQueue(q);

    // cleanup and exit
    return NULL;
}
