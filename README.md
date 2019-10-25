# Thread-Safe-Hash-Table-Library
A C program for thread-safe hash table library. 

A multi-threaded application using the library can create one or more hash tables and access them from multiple threads running concurrently. The library code is in the "hash.c" file and "hash.h" file is its header. 

A table in this program has N buckets. To reduce lock contention, there are multiple locks. For M consecutive blocks, there is one lock. There will be N/M regions, and N/M locks as well.

integer-count program takes text files as input. The program counts the number appearances of unique integers. In other words, for each integer appearing in one of those C files, it will find out how many times that integer occurs in those files.

### Execution of the Program
To execute integer-count.c, you can type the example command given in the  assignment. In that class, N is 1000 and K is 100.
example execution:  ./integer-count 7 1.txt 2.txt 3.txt 4.txt 5.txt 6.txt 7.txt out.txt

To execute test.c, you just need to type: ./test.c


