// Andrew McClelland (14amm5) - 10150229

#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#define ARRAY_SIZE 4

// Function to verify MPI solution with a simple sequential solution
long verify(long *array) {
    long sequential_sum = 0;

    for(int i = 0; i < ARRAY_SIZE; i++) {
        sequential_sum += *(array + i) * *(array + i);
    }
    return sequential_sum;
}

void main(int argc, char *argv []) {
	int myid, numprocs, i;
	long local_sum, result;
	long *array1 = (long*)malloc(ARRAY_SIZE * sizeof(long)), *array2 = (long*)malloc(ARRAY_SIZE * sizeof(long));
	double starttime, endtime;

	for (i = 0; i < ARRAY_SIZE; i++) {
		array1[i] = i + 1;
		array2[i] = i + 1;
	}

	// Init MPI, size, and ranks
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	starttime = MPI_Wtime();
	for (i = myid, local_sum = 0; i < ARRAY_SIZE; i += numprocs)
	{
		local_sum += array1[i] * array2[i];
	}
	MPI_Reduce(&local_sum, &result, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
	endtime = MPI_Wtime();

	if (myid == 0)
	{
		printf("Using %d processes, the dot product of arrays 1 to %i is %lu\n", numprocs, ARRAY_SIZE, result);
		printf("It took %0.8f seconds for thread id[%d] to complete\n", endtime - starttime, myid);
	}
	MPI_Finalize();

	free(array1);
	free(array2);
	return;
}