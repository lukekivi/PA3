#include "header.h"
#define ACCOUNT_INFO_MAX_LENGTH 32


/**
 * Write final balance to a single file.
 * The path name should be output/result.txt
 */
void writeBalanceToFiles(void) {
    int fd = open(finalDir, O_CREAT | O_WRONLY, 0777);
    if (fd < 0){
        printf("ERROR: Cannot open the file %s\n", finalDir);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

    // write balance for each customer
    double totalChange = 0;

    for (int i = 0; i < acctsNum; i++) {
      char line[ACCOUNT_INFO_MAX_LENGTH]; 
      sprintf(line, "%d\t%lf\n", i, balance[i]);      // add account number and balance.
      totalChange += balance[i];                        // add to total change for bottom of the file.

        int ret = write(fd, line, strlen(line));
        if(ret < 0){
            printf("ERROR: Cannot write to file %s\n", finalDir);
            fflush(stdout);
            exit(EXIT_FAILURE);
        }    
    }

    // write total balance change
    char totalChangeLine[ACCOUNT_INFO_MAX_LENGTH];
    sprintf(totalChangeLine, "All: \t%lf\n", totalChange); // use totalChange to write the last line of the file.
    int ret = write(fd, totalChangeLine, strlen(totalChangeLine));
    if(ret < 0){
        printf("ERROR: Cannot write to file %s\n", finalDir);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

    close(fd);
}

int main(int argc, char *argv[]) {

    nConsumers = 0;                // Number of consumer threads to be created.
    char* path = NULL;             // Path of input file
    int mode = 0;                  // App mode that specifies use of bounded buffer or log output
    int queueBufferSize = -1;      // initialized with an invalid value

    // Argument check
    if (argc < 3) {
        fprintf(stderr, "ERROR: %d is an insufficient amount of arguments.\n", argc);
        exit(EXIT_FAILURE);
    } else {

        // First argument should be an the number of consumer threads the user desires to create
        if (isDigits(argv[1]) == -1) {
            fprintf(stderr, "ERROR: first argument \"%s\" should be an integer.\n",argv[1]);
            exit(EXIT_FAILURE);
        } else {
            nConsumers = atoi(argv[1]);
        }

        // Second argument is the path for the input file
        path = argv[2];

        // If there are more than 3 arguments there is a mode specification
        if (argc > 3) {
            if (argv[3][0] == '-') {
                // check the mode
                if ((mode = parseModeArg(argv[3])) == -1) {
                    fprintf(stderr, "ERROR: %s is an invalid argument.\n", argv[3]);
                    exit(EXIT_FAILURE);
                }

                // if bounded buffer was requested get the buffer size
                if ((mode == 2 || mode == 3)) {
                    if (argc != 5 ||isDigits(argv[4]) == -1) {
                        fprintf(stderr, "ERROR: an integer bufferSize argument must be provided immediately after \"%s\".\n",argv[3]);
                        exit(EXIT_FAILURE);
                    } else {
                        queueBufferSize = atoi(argv[4]);
                    }
                }
            } else {
                fprintf(stderr, "ERROR: arguments should be in this order \"number of consumers, input file path, mode, (if '-b') buffer size\".\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    /**
     * Modes:
     * - 0: bare minimum functionality. No mode specified.
     * - 1: generate log output.
     * - 2: use a bounded buffer.
     * - 3: generate log output and use a bounded buffer.
     */

    bookeepingCode();

    // Initialize global variables, like shared queue

    FILE* fp = fopen(path, "r");

    if (fp == NULL) {
        fprintf(stderr, "ERROR: failed to open file \"%s\"\n", path);
        exit(EXIT_FAILURE);
    }

    q = initQueue();

    for (int i = 0; i < acctsNum; i++) {
        sem_init(&mutexBalances[i], 0, 1);
    }
    sem_init(&mutexQueue, 0, 1);
    sem_init(&staged, 0, 0);

    if (mode == 2 || mode == 3) {
        sem_init(&queueNodes, 0, queueBufferSize);
    }

    // Create producer and consumer threads
    pthread_t producerThread;
    pthread_t consumerThreads[nConsumers];

    pthread_create(&producerThread, NULL, producer, fp);
    int* index = (int*) malloc(sizeof(int));
    for (int i = 0; i < nConsumers; i++) {
        *index = i;
        pthread_create(&consumerThreads[i], NULL, consumer, index);
        index = (int*) malloc(sizeof(int));
    }
    index = NULL;

    // wait for all threads to complete execution
    //printf("launching producer\n");
    pthread_join(producerThread, NULL);

    for (int i = 0; i < nConsumers; i++) {
        //printf("launching consumer %d\n", i);
        pthread_join(consumerThreads[i], NULL);
    }

    //Write the final output
    writeBalanceToFiles();

    fclose(fp);

    return 0;
}
