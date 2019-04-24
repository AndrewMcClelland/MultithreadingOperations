// Andrew McClelland (14amm5) - 10150229

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define NUM_ROWS 5 // M dimension
#define NUM_COLS 2 // N dimension

// Function to verify MPI solution with a simple sequential solution
int verify(long* array, long* vector, long* result) {
	int i, j;
	long *sequential_result = (long*)malloc(NUM_ROWS * 1 * sizeof(long));

	for (i = 0; i < NUM_ROWS; i++)
	{
		sequential_result[i] = 0;
		for (j = 0; j < NUM_COLS; j++)
		{
			sequential_result[i] += *(array + i * NUM_COLS + j) * *(vector + j);
		}
		// Check that sequential result equals MPI result (0 is error code here)
		if (sequential_result[i] != *(result + i)) return 0;
	}
	// If we get here, then MPI was correct
	return 1;
}

void main(int argc, char *argv[])
{
	int myid, numprocs, i, j;
	long local_row_sum;
	double starttime, endtime;
	long *matrix = (long*)malloc(NUM_ROWS * NUM_COLS * sizeof(long));
	long *vector = (long*)malloc(NUM_COLS * sizeof(long));
	long *local_result = (long*)malloc(NUM_ROWS * sizeof(long));
	long *final_result = (long*)malloc(NUM_ROWS * sizeof(long));

	// Initialize array and vector
	for (i = 0; i < NUM_ROWS; i++) {
		for(j = 0; j < NUM_COLS; j++) {
			matrix[i * NUM_COLS + j] = i + j;
		}
	}

	for (j = 0; j < NUM_COLS; j++) {
		vector[j] = j;
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	starttime = MPI_Wtime();
	for (i = myid; i < NUM_ROWS; i += numprocs) {
		for (j = 0, local_row_sum = 0; j < NUM_COLS; j++) {
			local_row_sum += matrix[i * NUM_COLS + j] * vector[j];
		}
		local_result[i] = local_row_sum;
	}
	MPI_Reduce(local_result, final_result, NUM_ROWS, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
	endtime = MPI_Wtime();
	if (myid == 0) {
		if(verify(matrix, vector, final_result)) printf("Result verified!\n");
		printf("With %d processes, Matrix-Vector multiplication (M = %d, N = %d) took %0.8f seconds for thread id[%d]\n", numprocs, NUM_ROWS, NUM_COLS, endtime - starttime, myid);
		// Prints final result elements
		for (i = 0; i < NUM_ROWS; i++) {
			printf("Final_result[%d] = %lu\n", i, final_result[i]);
		}
	}

	MPI_Finalize();

	free(matrix);
	free(vector);
	free(local_result);
	free(final_result);

	return;
}