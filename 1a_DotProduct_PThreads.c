// Andrew McClelland (14amm5) - 10150229

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

#define ARRAY_SIZE 100000
#define NUM_THREADS 8

// Shared data
long *array1, *array2;
int global_index = 0;   // global index
long sum = 0;            // final result
pthread_mutex_t mutex1; // mutually exclusive lock variable

void *slave(void *ignored) // slave threads
{
    int local_index;
    long partial_sum = 0;
    do
    {
        // get next index into the array
        pthread_mutex_lock(&mutex1);
        local_index = global_index; // read current index & save locally
        global_index++;             // increment global index
        pthread_mutex_unlock(&mutex1);

        if (local_index < ARRAY_SIZE)
            partial_sum += *(array1 + local_index) * *(array2 + local_index);
    } while (local_index < ARRAY_SIZE);

    // Add partial sum to global sum
    pthread_mutex_lock(&mutex1);
    sum += partial_sum;
    pthread_mutex_unlock(&mutex1);
}

// Function to verify pthreads solution with a simple sequential solution
int verify() {
    long sequential_sum = 0;

    for(int i = 0; i < ARRAY_SIZE; i++) {
        sequential_sum += *(array1 + i) * *(array2 + i);
    }
    return sequential_sum == sum;
}

// Main function to populate sample array and implement pthreads dot product solution
void main() {
    int i;

    // clock_gettime() parameters
    struct timespec start, finish;

    array1 = (long*)malloc(sizeof(long) * ARRAY_SIZE);// array to dot product with itself
    array2 = (long*)malloc(sizeof(long) * ARRAY_SIZE);

    pthread_t thread[NUM_THREADS];       // threads
    pthread_mutex_init(&mutex1, NULL);  // initialize mutex

    for (i = 0; i < ARRAY_SIZE; i++) {    // initialize array
        array1[i] = i + 1;
        array2[i] = i + 1;
    }

    clock_gettime(CLOCK_REALTIME, &start);

    for (i = 0; i < NUM_THREADS; i++) {    // create threads
        if(pthread_create(&thread[i], NULL, slave, NULL) != 0) perror("Pthread_create fails!");
    }

    for(i = 0; i < NUM_THREADS; i++) {
        if(pthread_join(thread[i], NULL) != 0) perror("Pthread_join fails!");
    }

    clock_gettime(CLOCK_REALTIME, &finish);
    long seconds = finish.tv_sec - start.tv_sec;
    long ns = finish.tv_nsec - start.tv_nsec;

    if (start.tv_nsec > finish.tv_nsec) { // clock underflow
        --seconds;
        ns += 1000000000;
    }

    // Verify pthreads dot product is equal to sequential implementation
    if(verify()) {
        printf("The dot product of arrays 1 to %i is correctly verified to be %lu\n", ARRAY_SIZE, sum);
        printf("With %d threads, the elapsed time from thread creation to thread joining is %e seconds\n", NUM_THREADS, (double)seconds + (double)ns/(double)1000000000);
    } else {
        printf("ERROR: pthread dot product is not equal to sequential program!");
    }

    free(array1);
    free(array2);
}