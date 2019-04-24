// Andrew McClelland (14amm5) - 10150229

#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define ARRAY_SIZE 100000
#define NUM_THREADS 8

// Function to verify OpenMP solution with a simple sequential solution
long verify(long *array) {
    long sequential_sum = 0;

    for(int i = 0; i < ARRAY_SIZE; i++) {
        sequential_sum += *(array + i) * *(array + i);
    }
    return sequential_sum;
}

// Main function to populate sample arrays and implement OpenMP dot product solution using parallel for with a reduction
void main() {
    int i;
    long *array = (long*)malloc(sizeof(long) * ARRAY_SIZE), total_sum = 0;
    double start_time, total_time;

    // Initialize array
    for (i = 0; i < ARRAY_SIZE; i++) {
        array[i] = i + 1;
    }

    omp_set_num_threads(NUM_THREADS);

    start_time = omp_get_wtime();
    // OpenMP dot-product parallel for with a reduction implementation
    #pragma omp parallel for reduction (+:total_sum)
        for(i = 0; i < ARRAY_SIZE; i++) {
            total_sum += *(array + i) * *(array + i);
        }
    total_time = omp_get_wtime() - start_time;

    // Verify OpenMP dot product is equal to sequential implementation
    if (verify(array) == total_sum) {
        printf("The dot product of arrays 1 to %i is correctly verified to be %lu\n", ARRAY_SIZE, total_sum);
        printf("With %d threads, the elapsed time from thread creation to thread joining is %.8f seconds\n", NUM_THREADS, total_time);
    }
    else
    {
        printf("ERROR: OpenMP dot product is not equal to sequential program!\n OpenMP Result = %lu", total_sum);
    }

    free(array);
}