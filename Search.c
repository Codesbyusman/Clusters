#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>

#include <mpi.h>

// some constant values
#define ArraySize 2
#define NumberToSearch 1090

char *toString(int *, int);

int main(int argc, char *argv[])
{
    int array[ArraySize];
    int *chunkArray;
    int *chunkSizes;
    int *chunkOffsets;

    MPI_Init(&argc, &argv);

    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    // calculate the chunk sizes and offsets
    chunkSizes = (int *)malloc(sizeof(int) * nprocs);
    chunkOffsets = (int *)malloc(sizeof(int) * nprocs);

    // if the master process (the process with rank 0)
    if (rank == 0)
    {
        // initialize the array
        for (int i = 0; i < ArraySize; i++)
        {
            array[i] = i + 1;
        }

        printf("The Initial Array is: { %s }\n", toString(array, ArraySize));

        int remaining = ArraySize;
        for (int i = 0; i < nprocs; i++)
        {
            if (i != 0)
            {
                chunkSizes[i] = remaining / (nprocs - i);
                chunkOffsets[i] = ArraySize - remaining;
                remaining -= chunkSizes[i];
            }
            else
            {
                chunkSizes[i] = 0;
                chunkOffsets[i] = 0;
                remaining = ArraySize;
            }
        }
    }

    // broadcast the chunk sizes and offsets to all processes
    MPI_Bcast(chunkSizes, nprocs, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(chunkOffsets, nprocs, MPI_INT, 0, MPI_COMM_WORLD);

    // allocate the chunk array
    chunkArray = (int *)malloc(sizeof(int) * chunkSizes[rank]);

    // scatter the array using MPI_Scatterv
    MPI_Scatterv(array, chunkSizes, chunkOffsets, MPI_INT, chunkArray, chunkSizes[rank], MPI_INT, 0, MPI_COMM_WORLD);
    // search for the number
    // but not rank 0
    if (rank != 0)
    {
        // print the array of each process
        printf("\nProcess %d has the array: { %s }\nProcess %d Searching for %d\n\n", rank, toString(chunkArray, chunkSizes[rank]), rank, NumberToSearch);
    }

    // if the process is the master process
    if (rank == 0)
    {

        // receive the index from the process that found the number
        int index = -1;
        MPI_Status status;
        MPI_Recv(&index, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

        if (index != -1)
        {
            printf("\nMaster: Process %d found the number at index %d\nAll other processes should abort\n\n", status.MPI_SOURCE, index);

            // send abort signal to all other processes
            // the Mpi probe will be used to check if the process has received the abort signal
            for (int i = 1; i < nprocs; i++)
            {
                MPI_Send(NULL, 0, MPI_INT, i, 1, MPI_COMM_WORLD);
            }
        }
    }

    // bool found = true;
    //  searching the number // receving abort signal from the master process
    if (rank != 0)
    {
        int found = 0;

        for (int i = 0; i < chunkSizes[rank]; i++)
        {

            int abort;
            MPI_Status status;

            // getting the message from the master
            MPI_Iprobe(0, 1, MPI_COMM_WORLD, &abort, &status);

            // if said to abort so abort
            if (abort)
            {
                printf("Process %d aborting\n", rank);
                // free the allocated memory
                free(chunkArray);
                free(chunkSizes);
                free(chunkOffsets);

                MPI_Finalize();
                return 0;
            }

            if (chunkArray[i] == NumberToSearch)
            {
                printf("Process %d found the number at index %d\n", rank, i);

                // send the index to the master process
                MPI_Send(&i, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                found = 1;
                break;
            }
        }

        if (found == 0)
        {
            int i = -1;
            printf("\nNot found by process %d\n", rank);
            MPI_Send(&i, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    // free the allocated memory
    free(chunkArray);
    free(chunkSizes);
    free(chunkOffsets);

    // all processes to do work
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

    return 0;
}

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
