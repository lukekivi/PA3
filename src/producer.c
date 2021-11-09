#include "producer.h"

/**
 *
 * Producer thread will read from the file and write data to
 * the end of the shared queue
 */
void *producer(void *arg){

    printf("producer\n");

    //TODO: open the file and read its content line by line

    FILE * fd_in = (FILE*)arg; //  file we are reading from; will be passed in
    ssize_t nread;
    char buffer[chunkSize]; // chunk size for now
    size_t len = chunkSize;

    // read until EOF
    while (nread = getLineFromFile(fd_in, buffer, len) != -1) {
      //Send data to the shared queue
      struct Packet* p = (struct Packet*) malloc(sizeof(struct Packet));
      p->line = buffer;

      struct Node* n = (struct Node*)malloc(sizeof(struct Node));
      n->next = NULL;
      n->packet = p;

      //enqueue(q, n);  q is not recognized at the moment for some reason.
      freeNode(n);
    }
    //When reaching the end of the file, send EOF message
    printf("Reached EOF.\n");

    // cleanup and exit
    return NULL;
}
