#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include "ping-pong.c"

void Recursive_Doubling(int myRank, int size, int D, MPI_Comm comm)
{

    double *arr;
    double t1, t2;
    arr = (double *)malloc(sizeof(double) * size * D);

    for (int i = 0; i < D; i++)
    {
        arr[i] = myRank;
    }

    MPI_Request req1, req2;
    if (myRank == 0)
        t1 = MPI_Wtime();
    for (int step = 0; step < (log(size) / log(2)); step++)
    {
        // printf("step %d myrank -> %d tosend -> %d \n", step, myRank, myRank ^ (int)pow(2, step));
        MPI_Isend(arr, (int)pow(2, step) * D, MPI_DOUBLE, myRank ^ (int)pow(2, step), step, comm, &req1);
        MPI_Irecv(arr + ((int)pow(2, step)) * D, (int)pow(2, step) * D, MPI_DOUBLE, myRank ^ (int)pow(2, step), step, comm, &req2);
        MPI_Wait(&req1, MPI_STATUS_IGNORE);
        MPI_Wait(&req2, MPI_STATUS_IGNORE);
    }

    if (myRank == 0)
        t2 = MPI_Wtime();

    //check the output
    // if(myRank== 0)
    // for(int i = 0; i < D*size; i++) {
    //     printf("%f\n", arr[i]);
    // }
    if (myRank == 0)
        printf("Time: %f\n", t2 - t1);
}

int main(int argc, char **argv)
{
    int myRank, size, *minarr, *priority, D;
    //initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (myRank == 0)
    {
        D = atoi(argv[1]);  //D is a command line argument
        D = D * 1024 * 256; //MB
    }

    MPI_Bcast(&D, 1, MPI_INT, 0, MPI_COMM_WORLD);


    minarr = (int *)(malloc(sizeof(int) * size));

    findMinMapping(myRank, size, minarr);

    MPI_Bcast(minarr, size, MPI_INT, 0, MPI_COMM_WORLD);


    //let's split the communicator now
    MPI_Comm row_comm;
    MPI_Comm_split(MPI_COMM_WORLD, 0, minarr[myRank], &row_comm);
    int row_rank, row_size;
    MPI_Comm_rank(row_comm, &row_rank);
    MPI_Comm_size(row_comm, &row_size);
    // printf("WORLD RANK/SIZE: %d/%d \t ROW RANK/SIZE: %d/%d\n",
    //        myRank, size, row_rank, row_size);

    //Run ring Algorithm with this new setup
   //printf("old rank %d, new rank %d\n", myRank, row_rank);
    //Recursive_Doubling(row_rank, size, D);
    Recursive_Doubling(row_rank, size, D, row_comm);
    MPI_Comm_free(&row_comm);

    MPI_Finalize();
}