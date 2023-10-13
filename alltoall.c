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

    // equal to size of process
    int *procArray = (int *)malloc(sizeof(int) * nproc);

    // generate random numbers --- initialize
    for (int i = 0; i < nproc; i++)
    {
        procArray[i] = rand() % 100;
    }

    // print the array
    printf("\nProcess %d: has array {%s}\n", rank, toString(procArray, nproc));

    // will send to the each other process
    // in such a way that if i process is sending to j process the i position in j process would change

    // recieving array
    int *recArray = (int *)malloc(sizeof(int) * nproc);

    // iterating to all processes
    for (int i = 0; i < nproc; i++)
    {
        // if rank is differnt from i
        if (rank != i)
        {
            // send to the i process
            MPI_Send(&procArray[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD);

            // print the message
            printf("\nProcess %d: sent %d to process %d\n", rank, procArray[i], i);

            // receive from the i process
            MPI_Recv(&recArray[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // print the message
            printf("\nProcess %d: received %d from process %d\n", rank, recArray[i], i);
        }
        else
        {
            // if rank is equal to i
            // then copy the value
            recArray[i] = procArray[i];
        }
    }

    printf("\n");

    // print the array
    printf("\nFinal: Process %d: has array {%s}\n", rank, toString(recArray, nproc));

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
