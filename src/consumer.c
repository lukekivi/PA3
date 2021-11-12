#include "consumer.h"
#include <ctype.h>

/**
 * parse lines from the queue, calculate balance change
 * and update to global array
 */
void parse(char *line){
    // get customer id
    int id ;
    sscanf(line, "%d", &id);
    printf("ID: %d\n", id);

    // sum up transactions
    double totalChange = 0; // number to add to global balance array at end of function.
    double temp; // placeholder for doubles we are reading in.
    char* subLine = NULL;
    char* pattern = ",";

    // Parses the line for the next double. If there is a comma, there should be another double remaining.
    while ((subLine = strstr(line, pattern)) != NULL) {
      printf("Subline: %s\n", subLine); // Testing, comma should be first character.

      size_t len = strlen(subLine); // Find length of subLine for strncpy.
      char tempLine[(len-1)]; // String for strncpy to make version of subLine without comma.
      strncpy(tempLine, &subLine[1], (len-1)); // copies subLine to tempLine without the comma.
      printf("Templine: %s\n", tempLine); // Testing, should be same as subLine without comma.


      sscanf(tempLine, "%lf", &temp); // Scan the next double into temp.
      printf("Temp double: %lf\n", temp); // Testing, print the double.
      totalChange += temp; // Add to total change that will be applied to global array.

      strcpy(subLine, tempLine); // subLine now is updated, ready to look for next comma.
    }

    // update the global array
    printf("Total change: %lf\n", totalChange); // Testing
    balance[id] += totalChange;


}


// consumer function
void *consumer(void *arg){

    // keep reading from queue and process the data
    while(1){
        sem_wait(&staged);
        sem_wait(&mutex);
        struct Node* n = dequeue(q);
        sem_post(&mutex);

        if (n->packet->lineCount == -1) {
            return NULL;
        }

        sem_wait(&mutex); // can remove this later. this is just for testing,
                          // because it was annoying having all the consumers
                          // printing from parse at once. can be removed.
        parse(n->packet->line);
        sem_post(&mutex); // can be removed later.


        free(n);
    }

    return NULL;
}
