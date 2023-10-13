#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define ARRAY_SIZE 5

int main(int argc, char *argv[])
{

    printf("\n\n");
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
    int send[ARRAY_SIZE];
    int recv[ARRAY_SIZE * nproc];

    // generate random numbers that will be acting as data
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        send[i] = rand() % 100;
    }

    // print the data for each process
    printf("::::::: the data for process %d :::::::\n", rank);
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        printf("%d ", send[i]);
    }
    printf("\n");
    printf("\n\n ::::::::::::::::::::::::::::::::::::: \n\n");
    
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
            MPI_Send(send, ARRAY_SIZE, MPI_INT, i, 0, MPI_COMM_WORLD);
            printf("\nOn process %d, sending data to process %d: ", rank, i);
            for (int j = 0; j < ARRAY_SIZE; j++)
            {
                printf("%d ", send[j]);
            }
            printf("\n");
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
            // receive the data from the process
            MPI_Recv(recv + i * ARRAY_SIZE, ARRAY_SIZE, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // print the received data
            printf("\nOn process %d, received data from process %d: ", rank, i);
            for (int j = 0; j < ARRAY_SIZE; j++)
            {
                printf("%d ", recv[i * ARRAY_SIZE + j]);
            }
            printf("\n");
        }
        else
        {
            // if the process is the current process
            // copy the data to the recv array the process own data
            for (int j = 0; j < ARRAY_SIZE; j++)
            {
                recv[i * ARRAY_SIZE + j] = send[j];
            }
        }
    }

    printf("\n\n ::::::::::::::::::::::::::::::::::::: \n\n");

    // print the received data for each process
    printf("\nOn process %d, received data: ", rank);
    for (int i = 0; i < ARRAY_SIZE * nproc; i++)
    {
        printf("%d ", recv[i]);
    }
    printf("\n");

    printf("\n\n ::::::::::::::::::::::::::::::::::::: \n\n");

    MPI_Finalize();

    return 0;
}
