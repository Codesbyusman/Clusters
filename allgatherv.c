#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define MAX_ARRAY_SIZE 10

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

    // sending and receiving data between all processes
    // making dynamic arrays
    // int array_size = rand() % MAX_ARRAY_SIZE + 1;

    // for storing sizes
    int ArraySizes[nproc];
    int totalsize = 0;

    for (int i = 0; i < nproc; i++)
    {
        ArraySizes[i] = i + 1;
        totalsize += ArraySizes[i];
    }

    int *send = (int *)malloc(sizeof(int) * ArraySizes[rank]);
    int *recvf = (int *)malloc(sizeof(int) * totalsize);

    // generate random numbers that will be acting as data
    for (int i = 0; i < ArraySizes[rank]; i++)
    {
        send[i] = rand() % 100;
    }

    // print the data for each process
    printf("::::::: the data for process %d : { %s } ::::::: \n\n", rank, toString(send, ArraySizes[rank]));

    // send and receive data between all processes
    // loop through all processes
    for (int i = 0; i < nproc; i++)
    {
        // if the process is not the current process
        // mean what to do when not the rank
        // don't send to itself
        if (i != rank)
        {
            // send the data to the process
            MPI_Send(send, ArraySizes[rank], MPI_INT, i, 0, MPI_COMM_WORLD);
            printf("\nOn process %d, sending data to process %d: {%s} \n", rank, i, toString(send, ArraySizes[rank]));
        }
    }

    // loop through all processes
    for (int i = 0; i < nproc; i++)
    {

        // if the process is not the current process
        // recieve the data from the process
        // not receiving from itself
        if (i != rank)
        {
            // allocating the memory accordingly
            int *recv = (int *)malloc(sizeof(int) * ArraySizes[i]);

            // // receive the data from the process
            MPI_Recv(recv, ArraySizes[i], MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // print the received data
            printf("\nOn process %d, received data from process %d: {%s} \n", rank, i, toString(recv, ArraySizes[i]));

            // calculating offsets so that can use in concatenation
            int offset = 0;
            for (int j = 0; j < i; j++)
            {
                // mapping with array sizes that what would be offset
                offset += ArraySizes[j];
            }

            // add up in that offset
            for (int j = 0; j < ArraySizes[i]; j++)
            {
                recvf[offset + j] = recv[j];
            }
            // free the memory
            free(recv);
        }
        else
        {
            // if the process is the current process
            // copy the data to the recv array the process own data
            // calculating offsets so that can use in concatenation
            int offset = 0;
            for (int j = 0; j < i; j++)
            {
                offset += ArraySizes[j];
            }

            // add up in that offset
            for (int j = 0; j < ArraySizes[i]; j++)
            {
                recvf[offset + j] = send[j];
            }
        }
    }

    printf("\n\n :::::::::::::::::::::::::::::::::::: \n\n", totalsize);

    // print the received data for each process
    printf("\nFinal: On process %d, received data: {%s} \n", rank, toString(recvf, totalsize));

    printf("\n\n ::::::::::::::::::::::::::::::::::::: \n\n");

    // free the memory
    free(send);
    free(recvf);
    
    MPI_Finalize();

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
