#ifndef UTILS_H
#define UTILS_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#define chunkSize 1024
#define acctsNum 1000

/* shared array definition */
extern double balance[acctsNum];

/* shared queue of nodes containing data packets */
extern struct Queue* q;

/* mode of the program - set by flags passed as arguments to main */
extern int mode;

extern int nConsumers;
extern int queueBufferSize;

/* sempahores */
extern sem_t mutexBalances[acctsNum];        // Balance is being modified    
extern sem_t mutexQueue;        // Queue is being modified
extern sem_t staged;            // A producer signals when a package was staged in the queue
extern sem_t queueNodes;        // Used when buffer size is specified - mode 2 or 3

/* file I/O */
/**
 * Get a pointer to a opened file based on the file name
 * @param inputFileName  the file path
 * @return a file pointer pointing to the file
 */
FILE * getFilePointer(char *inputFileName);

/**
 * Read an entire line from a file
 * @param  fp    the file to be read
 * @param  line  contain the line content
 * @param  len    the size of the line
 * @return the number of character reads (including the newline \n, but not including terminator)
           -1 when reaching the end of file or error occurs
 */
ssize_t getLineFromFile(FILE *fp, char *line, size_t len);

/**
 * Open a file, and write a line to the file
 * @param filepath  the file path
 * @param line      the line content
 */
void writeLineToFile(char *filepath, char *line);

/* directory */
void bookeepingCode();

/* other function declaration */

/**
 * Customer transaction data packet.
 * @param line one line from an input file consisting of customer id and then transaction data seperated by commas
 */
struct Packet {
    char* line;
    int lineCount;
};

/**
 * Node for shared queue linked list
 * @param next   pointer to the next node in the linked list
 * @param packet a packet of customer data
 */
struct Node {
    struct Node* next;
    struct Packet* packet;
};

/** Queue Stuff **/

/**
 * Queue for nodes that carry packets
 * @param tail where nodes are dequeued
 * @param head dummy node. Head->next is what is dequeued.
 */
struct Queue {
    struct Node* head;
    struct Node* tail;
};

/**
 * Initialize a queue
 * @returns a pointer to an initialized queue
 */
struct Queue* initQueue();


/**
 * Add node to a queue. If the queue is empty to start, tail should be a node with it's fields set to NULL.
 * @param q    the queue
 * @param node node to be added. node->next should be NULL
 */
void enqueue(struct Queue* q, struct Node* node);

/**
 * @details Pop the head node off of the queue
 * @param q    the queue
 * @returns    popped node or NULL if queue is empty
 */
struct Node* dequeue(struct Queue* q);

/**
 * Deallocate a node
 * @param node the node to free
 */
void freeNode(struct Node* node);

/**
 * Free entire queue
 * @param q    the queue
 */
void freeQueue(struct Queue* q);

/** End Queue Stuff **/

/**
 * @brief Parse out command line argument that starts with '-'
 * @param str a string that starts with '-' that presumably contains some combination of p & b
 * @return -1 if invalid, 1 if p, 2 if b, and 3 if both
 */
int parseModeArg(char* str);

/**
 * @brief Check if a string is composed strictly of digits
 * @param str a string
 * @return -1 if a non-digit character was found, 1 if str is composed of digits
 */
int isDigits(char* str);

/* Debugging functions */

/**
 * Print the packets in all queues
 * @param q    the queue
 */
void printQueue(struct Queue* q);

/**
 * Print a node
 * @param node the node
 */
void printNode(struct Node* node);

#endif
