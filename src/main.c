#include "header.h"

/**
 * Write final balance to a single file.
 * The path name should be output/result.txt
 */
void writeBalanceToFiles(void) {
    // TODO: write balance for each customer 
    
    // TODO: write total balance change
}

int main(int argc, char *argv[]){
    int nConsumers = 0;
    char* path = NULL;
    int mode = 0;
    int queueBufferSize = -1;

    // Argument check
    if (argc < 2) {
        fprintf(stderr, "ERROR: %d is an insufficient amount of arguments.\n", argc);
        exit(EXIT_FAILURE);
    } else {
        if (isDigits(argv[1]) == -1) {
            fprintf(stderr, "ERROR: first argument \"%s\" should be an integer.\n",argv[1]);
            exit(EXIT_FAILURE);
        } else {
            nConsumers = atoi(argv[1]);
        }

        path = argv[2];

        if (argc == 5) {
            if (argv[3][0] == '-') {
                
                if ((mode = parseModeArg(argv[3])) == -1) {
                    fprintf(stderr, "ERROR: %s is an invalid argument.\n", argv[3]);
                    exit(EXIT_FAILURE);
                }

                if (mode == 2 || mode == 3) {
                    if(isDigits(argv[4]) == -1) {
                        fprintf(stderr, "ERROR: a bufferSize argument must be provided immediately after \"%s\".\n",argv[3]);
                        exit(EXIT_FAILURE);
                    } else {
                        queueBufferSize = atoi(argv[4]);
                    }
                }
            }
        }
    }

    bookeepingCode();
    
    //TODO: Initialize global variables, like shared queue
    
    //TODO: create producer and consumer threads

    //TODO: wait for all threads to complete execution
    
    //Write the final output
    writeBalanceToFiles();
    
    return 0; 
}