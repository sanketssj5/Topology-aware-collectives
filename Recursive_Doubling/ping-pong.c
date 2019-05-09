#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>

void print(char *p, int size, int rank)
{
    printf("\nrank is %d \n", rank);
    for (int i = 0; i < size; i++)
        printf("%c ", p[i]);
}

int find_closest(int ref_rank, double **matrix, int size)
{
    int min_index, firstvalue = 1;
    double min_time;

    for (int j = 0; j < size; j++)
    {
        if (matrix[ref_rank][j] > 0)
        {
            if (firstvalue)
            {
                min_time = matrix[ref_rank][j];
                min_index = j;
                firstvalue = 0;
            }
            else
            {
                if (matrix[ref_rank][j] < min_time)
                {

                    min_time = matrix[ref_rank][j];
                    min_index = j;
                }
            }
        }
    }

    for (int k = 0; k < size; k++)
    {
        matrix[k][ref_rank] = 0;
        matrix[k][min_index] = 0;
    }
    return min_index;
}

int findMinMapping(int myRank, int size, int *minarr)
{
    int D;
    double t1, t2, time = 0, avgTime = 0, iterationTime = 0, **matrix, *mat;
    char *data;

    //broadcast D
    if (myRank == 0)
    {
        // D = atoi(argv[1]); //D is a command line argument
        // D = D * 1024 * 1024;
        D = 1024 * 32;
        matrix = (double **)(malloc(sizeof(double) * size));

        for (int i = 0; i < size; i++)
        {
            minarr[i] = 0;
        }
        for (int i = 0; i < size; i++)
        {
            matrix[i] = (double *)(malloc(sizeof(double) * size));
        }
    }
    MPI_Bcast(&D, 1, MPI_INT, 0, MPI_COMM_WORLD);

    //allocating space for producer/receiver buffers
    mat = (double *)malloc(sizeof(double) * size);
    data = (char *)malloc(sizeof(char) * D);

    //fill data a->process 0, b->process 1, c-> process 2 ...
    for (int j = 0; j < D; j++)
        data[j] = 'a' + myRank;

    //running for 10 iterations final time will be stored in iterationTime
    //change iteration later
    int iter = 1;
    for (int j = 0; j < iter; j++)
    {
        //time contains summation of every send operation time done by a single process
        time = 0;
        //avgTime contains summation of every send operation time done by all the processes
        avgTime = 0;
        for (int i = 0; i < myRank; i++)
        {
            MPI_Status status;
            MPI_Recv(data, D, MPI_CHAR, i, i, MPI_COMM_WORLD, &status);
            //printf("%d->%d\n", myRank, status.MPI_TAG);
            // print(data, D, myRank);
        }

        for (int i = myRank + 1; i < size; i++)
        {
            t1 = MPI_Wtime();
            MPI_Send(data, D, MPI_CHAR, i, myRank, MPI_COMM_WORLD);
            t2 = MPI_Wtime();
            //printf("from %d to %d time: %f\n",myRank, i, t2-t1);
            // time += (t2 - t1);
            mat[i] += (t2 - t1);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    for (int i = 0; i < size; i++)
    {
        //average of all time
        mat[i] /= iter;
        //printf("rank : %d index: %d val: %f\n", myRank, i, mat[i]);
    }
    if (myRank != 0)
    {
        MPI_Send(mat, size, MPI_DOUBLE, 0, myRank, MPI_COMM_WORLD);
    }
    if (myRank == 0)
    {
        for (int i = 1; i < size; i++)
        {
            MPI_Recv(matrix[i], size, MPI_DOUBLE, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        for (int i = 0; i < size; i++)
        {
            matrix[0][i] = mat[i];
        }
        int count = 0;
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                if (i > j)
                    matrix[i][j] = matrix[j][i];
            }
        }

        //for printing
        // for (int i = 0; i < size; i++)
        // {
        //     printf("%d ", i);
        //     for (int j = 0; j < size; j++)
        //         printf("%f ", matrix[i][j]);
        //     printf("\n");
        // }
        //debug this
        //i is my minarray index
        int x = 0;
        //size should be even
        unsigned int  ref_rank = 0, i = size/2;
        unsigned int size1 = size - 1, mappingcount = 0;
        minarr[0] = 0;

        for(int i = 1; i < size; i++) {
            minarr[i] = -1;
        }

        while(size1--){

            mappingcount++;

            while(  minarr[(ref_rank ^ i)] != -1) {
                    i = i / 2;
            }
            
            unsigned int new_rank = ref_rank ^ i;
            unsigned int target_core = find_closest(ref_rank, matrix, size);
            minarr[new_rank] = target_core;

          //  printf("%d -> %d\n", new_rank, target_core);
            
            if(mappingcount == 2) {
                ref_rank = new_rank;
                i = size / 2;
                mappingcount = 0;
            }

        }

        // printf("Optimal mapping");
        //  printf("\n");
        // for(int i = 0; i < size; i++) {
        //     printf("%d ", minarr[i]);
        //     printf("\n");
        // }



        // //this was the original code
        // for (int i = 0; i < size; i++)
        // {
        //     for (int j = 0; j < size; j++)
        //     {
        //         if (matrix[x][j] > 0)
        //         {
        //             if (minarr[i] == 0)
        //                 minarr[i] = j;
        //             else
        //                 minarr[i] = (matrix[x][j] > matrix[x][minarr[i]] ? minarr[i] : j);
        //         }
        //     }

        //     for (int k = 0; k < size; k++)
        //     {
        //         matrix[k][x] = 0;
        //         matrix[k][minarr[i]] = 0;
        //     }
        //     // printf("After updation\n");
        //     // for (int i = 0; i < size; i++)
        //     // {
        //     //     printf("%d ", i);
        //     //     for (int j = 0; j < size; j++)
        //     //         printf("%f ", matrix[i][j]);
        //     //     printf("\n");
        //     // }
        //     //the x that i am seeing is actualy the i that i want to check in.
        //     x = minarr[i];
        //     //printf("x is %d \n", x);
        // }

        //shift one digit to right
        // for (int i = size - 2; i >= 0; i--)
        // {
        //     minarr[i + 1] = minarr[i];
        // }
        // minarr[0] = 0;

        // //find  minimum
        // printf("minimum mapping \n");
        // for (int i = 0; i < size; i++)
        // {
        //     printf("%d ", minarr[i]);
        // }
        // printf("\n");
    }
    MPI_Barrier(MPI_COMM_WORLD);

    return 0;
}
