#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

// for string
char *toString(int *intArray, int size);

int main(int argc, char *argv[])
{
    int rank, nproc;
    MPI_Init(&argc, &argv);

    // the ids
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // getting the size of process
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    // randomize the seed
    srand(time(NULL) + rank);

    int *arraySizes = (int *)malloc(sizeof(int) * nproc);

    // different size of array
    for (int i = 0; i < nproc; i++)
    {
        arraySizes[i] = i + 1;
    }

    // equal to size of process
    int *procArray = (int *)malloc(sizeof(int) * arraySizes[rank]);

    // initializing the array
    for (int i = 0; i < arraySizes[rank]; i++)
    {
        procArray[i] = rand() % 100;
    }

    // printing the array
    printf("Process %d has array: %s\n", rank, toString(procArray, arraySizes[rank]));

    // receiving array
    int *recArray = (int *)malloc(sizeof(int) * nproc);

    // iterating to all processes
    for (int i = 0; i < nproc; i++)
    {
        // if rank is different from i
        if (rank != i)
        {
            if (i > arraySizes[i])
            {
                // send to the i process
                MPI_Send(&procArray[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD);

                // print the message
                printf("\nProcess %d: sent %d to process %d\n", rank, procArray[i], i);
            }
            else
            {
                // send to the i process
                MPI_Send(&procArray[0], 1, MPI_INT, i, 0, MPI_COMM_WORLD);

                // print the message
                printf("\nProcess %d: sent %d to process %d\n", rank, procArray[0], i);
            }

            // receive from the i process
            MPI_Recv(&recArray[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // print the message
            printf("\nProcess %d: received %d from process %d\n", rank, recArray[i], i);
        }
        else
        {
            // if rank is equal to i
            // then copy the value
            if (i <= arraySizes[i])
            {
                recArray[i] = procArray[i];
            }
            else
            {
                recArray[i] = procArray[0];
            }
        }
    }

    printf("\n");

    // print the array
    printf("\nFinal: Process %d: has array {%s}\n", rank, toString(recArray, nproc));

    // free the memory
    free(recArray);
    free(procArray);
    free(arraySizes);

    MPI_Finalize();

    printf("\n");

    return 0;
}

// for printing
char *toString(int *intArray, int size)
{
    char *string = (char *)malloc(sizeof(char) * (size * 4) + 1);

    // Copy the values to the string
    int offset = 0;
    for (int i = 0; i < size; i++)
    {
        offset += snprintf(string + offset, size * 4 - offset + 1, "%d ", intArray[i]);
    }

    // Add the null terminator
    string[size * 4] = '\0';

    return string;
}
