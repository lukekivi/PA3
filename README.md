# Project #3 – Multithreading
* test machine: csel-broccoli.cselabs.umn.edu

** Group Details
    - #45
    - Lucas Kivi      - kivix019
        - main, queue, testing, help with producer and consumer
    - Viet Nguyen     - nguy4471
        - consumer, parse line, test on lab machine, extra credit
    - Dallas Schauer  - schau364
        - producer, help consumer, testing

* Whether to complete the extra task
    - Yes we completed the extra credit.

* Any assumptions outside this document
    - No.

* How to compile and run your program
    - Compilation:
        - make
    - Run:
        - ./bank nConsumers inputfile flags bufferSize
            - **nConsumers** - the desired number of consumer threads
            - **inputfile**  - the input file in .csv format
            - **flags**      - '-p' for logs printed to output/log.txt, '-b' for a bounded buffer, '-bp'/'-pb' for both
            - **buffersize** - if 'b' was specified in flags you must include a buffersize 
