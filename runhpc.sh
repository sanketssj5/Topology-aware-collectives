#! /bin/bash                                                                       
#PBS -N PROJECT                                                                     
#PBS -q courses                                                                    
#PBS -l nodes=16:ppn=1                                                      
#merge output and error into a single job_name.number_of_job_in_queue.          
#PBS -j oe                                                                          
#export fabric infiniband related variables                                       
export I_MPI_FABRICS=shm:tmi                                                        
export I_MPI_DEVICE=rdma:OpenIB-cma                                               
#change directory to where the job has been submitted from                        
cd $PBS_O_WORKDIR                                                                
#source paths                                                                  
#source /opt/software/intel/initpaths intel64                                
#run the job on required number of cores

echo --ringAlgo--
echo n=16, 1MB  
mpicc -std=c99 ringAlgo.c && mpirun -np 16 ./a.out 1 
echo n=16, 16MB
mpicc -std=c99 ringAlgo.c && mpirun -np 16 ./a.out 16
echo n=16, 64MB
mpicc -std=c99 ringAlgo.c && mpirun -np 16 ./a.out 64 
#echo n=16, 128MB
#mpicc -std=c99 ringAlgo.c && mpirun -np 16 ./a.out 128


echo --topAware ringAlgo--
echo n=16, 1MB  
mpicc -std=c99 top_aware_ringAlgo.c && mpirun -np 16 ./a.out 1
echo n=16, 16MB
mpicc -std=c99 top_aware_ringAlgo.c && mpirun -np 16 ./a.out 16 
echo n=16, 64MB
mpicc -std=c99 top_aware_ringAlgo.c && mpirun -np 16 ./a.out 64 
#echo n=16, 128MB
#mpicc -std=c99 top_aware_ringAlgo.c && mpirun -np 16 ./a.out 128

