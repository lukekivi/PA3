#include "utils.h"

/** Define globals **/
/* shared array definition */
double balance[acctsNum];

/* shared queue of nodes containing data packets */
struct Queue* q;

/* mode of the program - specified by command line arguments  */
int mode;

/* number of consumers - specified by command line arguments */
int nConsumers;

// shared queue size
int queueBufferSize;

/* sempahores */
sem_t mutexBalances[acctsNum];
sem_t mutexQueue;             
sem_t staged;            
sem_t queueNodes;      

// pthread.h included in header.h

/* File operations */
void writeLineToFile(char *filepath, char *line) {
    int fd = open(filepath, O_CREAT | O_WRONLY | O_APPEND, 0777);
    if (fd < 0){
        printf("ERROR: Cannot open the file %s\n", filepath);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
    int ret = write(fd, line, strlen(line));
    if(ret < 0){
        printf("ERROR: Cannot write to file %s\n", filepath);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
    close(fd);
}

FILE * getFilePointer(char *inputFileName) {
    return fopen(inputFileName, "r");
}

ssize_t getLineFromFile(FILE *fp, char *line, size_t len) {
    memset(line, '\0', len);
    return getline(&line, &len, fp);
}

void _removeOutputDir(){
    pid_t pid = fork();
    if(pid == 0){
        char *argv[] = {"rm", "-rf", "output", NULL};
        if (execvp(*argv, argv) < 0) {
            printf("ERROR: exec failed\n");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    } else{
        wait(NULL);
    }
}

void _createOutputDir(){
    mkdir("output", ACCESSPERMS);
}

void bookeepingCode(){
    _removeOutputDir();
    sleep(1);
    _createOutputDir();
}

/*** other functions ***/


/** Queue Function **/

/* Return a pointer to an initialized queue */
struct Queue* initQueue() {
    struct Queue* q = (struct Queue*) malloc(sizeof(struct Queue));
    q->head = (struct Node*) malloc(sizeof(struct Node));
    q->head->next = NULL;
    q->head->packet = NULL;

    q->tail = NULL;

    return q;
}

/* Add node to the bottom of a queue */
void enqueue(struct Queue* q, struct Node* node) {

    if (q->tail == NULL) {
        // case where q is empty
        q->tail = node;
        q->head->next = node;
    } else {
        // case where q has been started
        struct Node *temp = q->tail;
        temp->next = node;
        q->tail = node;
    }
}

/* Pop node from queue */
struct Node* dequeue(struct Queue* q) {
    if (q->head->next == NULL) {
        // q is empty
        return NULL;
    }

    struct Node *temp = q->head->next;
    if (q->head->next == q->tail) {
        // last element removed
        q->tail = NULL;
    }

    q->head->next = q->head->next->next;
    temp->next = NULL;

    return temp;
}

/* Deallocate a node */
void freeNode(struct Node* node) {
    if (node == NULL) {
        return;
    } else {
        if (node->packet != NULL) {
            free(node->packet->line);
            node->packet->line = NULL;
            free(node->packet); 
            node->packet = NULL;   
        }
        node->next = NULL;        
        free(node);
    }
}

/* free entire queue */
void freeQueue(struct Queue* q) {
    struct Node* curNode = q->head;
    while(curNode != NULL) {
        struct Node* temp = curNode->next;
        freeNode(curNode);
        curNode = temp;
    }
    q->head = NULL;
    q->tail = NULL;

    free(q);
}

/** End Queue Functions **/

/* Parse the command line argument for modes */
int parseModeArg(char* str) {
    int len = strlen(str);

    if (len > 3 || str[0] != '-') {
        return -1;
    }

    int mode = 0;

    for (int i = 1; i < len; i++) {
        if (str[i] == 'p') {
            mode += 1;
        } else if (str[i] == 'b') {
            mode += 2;
        } else {
            return -1;
        }
    }

    return mode;
}

/* Check a string to see if it is strictly composed of digits */
int isDigits(char* str) {
    int len = strlen(str);

    for (int i = 0; i < len; i++) {
        if (str[i] < '0' || str[i] > '9') {
            return -1;
        }
    }

    return 1;
}



/*** Debugging Functions ***/

void printQueue(struct Queue* q) {
    if(q->head->next == NULL) {
        printf("queue is empty\n");
    } 

    struct Node* curNode = q->head->next;
    while (curNode != NULL) {
        printNode(curNode);
        curNode = curNode->next;
    }
}

void printNode(struct Node* node) {
    if (node == NULL) {
        printf("NULL NODE\n");
    } else if (node->packet == NULL) {
        printf("packet is NULL\n");
    }
    if(node->packet->line == NULL) {
        printf("EOF Node\n");   
    } else {
        printf("%s", node->packet->line); 
    }
}

