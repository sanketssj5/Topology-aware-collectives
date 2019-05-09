#include <stdio.h>
#include "ping-pong.c"

void ringAlgo(int myRank, int size, int D)
{
    int  **arr;
    double startTime, endTime;

    arr = (int **)malloc(sizeof(int *) * size);

    for (int i = 0; i < size; i++)
    {
        arr[i] = (int *)(malloc(sizeof(int) * D));
    }
    for (int i = 0; i < D; i++)
    {
        arr[0][i] = myRank;
    }

    int index = 0;
    if (myRank == 0)
        startTime = MPI_Wtime();
    for (int step = 1; step < size; step++)
    {
        MPI_Request req1, req2;
        MPI_Isend(arr[index], D, MPI_INT, (myRank + 1) % size, myRank, MPI_COMM_WORLD, &req1);
        MPI_Irecv(arr[++index], D, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &req2);
        MPI_Wait(&req1, MPI_STATUS_IGNORE);
        MPI_Wait(&req2, MPI_STATUS_IGNORE);
        
    }
    if (myRank == 0)
    {
        endTime = MPI_Wtime();
        printf("Total time = %f\n", endTime - startTime);
    }
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
    
    if (myRank == 0)
    {
        minarr = (int *)(malloc(sizeof(int) * size));
    }

    priority = (int *)(malloc(sizeof(int) * size));
    findMinMapping(myRank, size, minarr);


     if (myRank == 0)
     {
        printf("minimum mapping \n");
         for (int i = 0; i < size; i++)
         {
             printf("%d ", minarr[i]);
         }
         printf("\n");
     }

    //      int i1 = 1;
    //      for (int i = 0; i < size; i++)
    //      {
    //          priority[minarr[i]] = i1++;
    //      }
    //      // for (int i = 0; i < size; i++)
    //      // {
    //      //     printf("%d ", priority[i]);
    //      // }
    //      // printf("\n");
    //  }
    //  MPI_Bcast(priority, size, MPI_INT, 0, MPI_COMM_WORLD);

    //  //let's split the communicator now
    //  MPI_Comm row_comm;
    //  MPI_Comm_split(MPI_COMM_WORLD, 0, priority[myRank], &row_comm);
    //  int row_rank, row_size;
    //  MPI_Comm_rank(row_comm, &row_rank);
    //  MPI_Comm_size(row_comm, &row_size);
    //  // printf("WORLD RANK/SIZE: %d/%d \t ROW RANK/SIZE: %d/%d\n",
    //  //        myRank, size, row_rank, row_size);

    //  //Run ring Algorithm with this new setup
    //  ringAlgo(row_rank, size, D);

    //MPI_Comm_free(&row_comm);
    MPI_Finalize();
    return 0;
}
