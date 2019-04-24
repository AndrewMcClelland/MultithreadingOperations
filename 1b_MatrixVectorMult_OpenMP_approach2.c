// Andrew McClelland (14amm5) - 10150229

#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define NUM_ROWS 5 // M-dimension
#define NUM_COLS 2  // N-dimension
#define NUM_THREADS 2

// Function to verify OpenMP solution with a simple sequential solution
int verify(long* array, long* vector, long* result)
{
	int i, j;
	long *sequential_result = (long*)malloc(NUM_ROWS * 1 * sizeof(long));

	for (i = 0; i < NUM_ROWS; i++)
	{
		sequential_result[i] = 0;
		for (j = 0; j < NUM_COLS; j++)
		{
			sequential_result[i] += *(array + i * NUM_COLS + j) * *(vector + j);
		}
		// Check that sequential result equals OpenMP result (0 is error code here)
		if (*(sequential_result + i) != *(result + i)) return 0;
	}
	// If we get here, then OpenMP was correct
	return 1;
}

// Main function to populate sample arrays and implement OpenMP matrix-vector multiplication solution using parallel for with a reduction
// This implements the second matrix vector multiplication approach in which the threads are divided up in a way that all the elements of the output vector P are computed simultaneously
void main()
{
	int i, j;
	long row_sum, *array = (long*)malloc(NUM_ROWS * NUM_COLS * sizeof(long)), *vector = (long*)malloc(NUM_COLS * 1 * sizeof(long)), *result = (long*)malloc(NUM_ROWS * 1 * sizeof(long));
	double start_time, total_time;

	// Initialize array and vector
	for (i = 0; i < NUM_ROWS; i++) {
		for(j = 0; j < NUM_COLS; j++) {
			array[i * NUM_COLS + j] = i + j;
		}
	}

	for (j = 0; j < NUM_COLS; j++) {
		vector[j] = j;
	}

	omp_set_num_threads(NUM_THREADS);
	start_time = omp_get_wtime();

// OpenMP matrix-vector multiplication parallel for with a reduction implementation
#pragma omp parallel for private(i, j, row_sum)
	for (i = 0; i < NUM_ROWS; i++) {
		row_sum = 0;
		// #pragma omp parallel for
		for (j = 0; j < NUM_COLS; j++) {
			row_sum += *(array + i * NUM_COLS + j) * *(vector + j);
		}
		result[i] = row_sum;
	}
	total_time = omp_get_wtime() - start_time;

	// Verify OpenMP matrix-vector multiplication is equal to sequential implementation
	if (verify(array, vector, result))
	{
		printf("The matrix of array size %d X %d and vector %d X 1 is:\n\n", NUM_ROWS, NUM_COLS, NUM_COLS);
		printf("With %d threads, the elapsed time from thread creation to thread joining is %.8f seconds\n", NUM_THREADS, total_time);
	}
	else
	{
		printf("ERROR: OpenMP matrix mult is not equal to sequential program!\n OpenMP =\n\n");
	}
	for(i = 0; i < NUM_ROWS; i++) printf("P[%d][0] = %lu\n", i, *(result + i));

	free(array);
	free(vector);
	free(result);
}