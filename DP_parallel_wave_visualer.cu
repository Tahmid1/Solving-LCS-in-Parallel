/* DP_parallel_wave_visualer.cu
 * implemented for CS87 final project
 * by Brandon Chow and Tahmid Rahman
 * implements an animated LCS solver on the GPU
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "myopengllib.h"
#include <curand_kernel.h>
#include <curand.h>
#include <math.h>
#include <sys/types.h>
#include <signal.h>

static int N       = 256;

typedef struct my_cuda_data {
  int* read_grid;
  curandState *dev_random;
  int N;
  int num_iters;
  int cur_iters;
  cudaEvent_t start;
  cudaEvent_t stop;
  int* string_a;
  int* string_b;
  int* currMax;
} my_cuda_data;
my_cuda_data cudaData;

static void clean_up(void);
static void compute_LCS(uchar4 *devPtr, void *my_data) ;
__global__ void  LCS_kernel(my_cuda_data data);
__global__ void  retrace_kernel(my_cuda_data data);
__global__ void color_in_kernel(uchar4 *optr, my_cuda_data data);
__device__ int getFireNeighbor(int* data, int x, int y, int N);
__device__ void colorPix(uchar4 *optr, int offset, int type);

void usage();

/**************************************************************/
//MAIN
/**************************************************************/
int main(int argc, char *argv[])  {
  int i, j;
  //process_args(argc, argv);
  //int world[N][N];

  //parse input
  if (argc != 1){
    if(argc != 2) {
      printf("usage: ./dp_rand (size) \n");
      exit(0);
    } else {
      N = atoi(argv[1]);
    }
  }
  N = N+1;
  int* world;
  int* string_a = new int[N-1];
  int* string_b = new int[N-1];


  for (int i = 0; i < N-1; i++) {
    string_a[i] = rand() % 26;
  }


  for (int i = 0; i < N-1; i++) {
    string_b[i] = rand()%26;
  }

  //set up world
  world = new int[N*N]();

  cudaEventCreate(&(cudaData.start));
  cudaEventCreate(&(cudaData.stop));

  GPUDisplayData my_display(N, N, &cudaData, "Simple openGL-Cuda");

  //make world
  //fill in trees
  for(i=0; i < N; i++){
    for(j=0; j < N; j++){
      world[i*N + j] = -1*i + -1*j;
    }
  }

  cudaData.num_iters = 2*N;
  cudaData.cur_iters = 0;
  cudaData.N = N;

  int x = -1;
  int* currMax;
  currMax = &x;

  /*
     printf("%c\n", cudaData.string_a[40]);
     printf("%s\n", cudaData.string_b);
   */

  //allocate memory for currMax
  HANDLE_ERROR(cudaMalloc((void**)&cudaData.currMax,
        sizeof(int)), "malloc read_grid") ;

  // copy the initial data to the GPU
  HANDLE_ERROR (cudaMemcpy(cudaData.currMax, currMax,
        sizeof(int), cudaMemcpyHostToDevice), "copy dev_grid to GPU") ; 

  //allocate memory for grid
  HANDLE_ERROR(cudaMalloc((void**)&cudaData.read_grid,
        sizeof(int)*N*N), "malloc read_grid") ; 

  // copy the initial data to the GPU
  HANDLE_ERROR (cudaMemcpy(cudaData.read_grid, world,
        sizeof(int)*N*N, cudaMemcpyHostToDevice), "copy read_grid to GPU") ;

  //allocate memory for string_a
  HANDLE_ERROR(cudaMalloc((void**)&cudaData.string_a,
        sizeof(int)*(N-1)), "malloc read_grid") ; 

  // copy the initial data to the GPU
  HANDLE_ERROR (cudaMemcpy(cudaData.string_a, string_a,
        sizeof(int)*(N-1), cudaMemcpyHostToDevice), "copy string_a to GPU") ;


  //allocate memory for string_b
  HANDLE_ERROR(cudaMalloc((void**)&cudaData.string_b,
        sizeof(int)*(N-1)), "malloc read_grid")

    // copy the initial data to the GPU
    HANDLE_ERROR (cudaMemcpy(cudaData.string_b, string_b,
          sizeof(int)*(N-1), cudaMemcpyHostToDevice), "copy string_b to GPU") ; 


  // register a clean-up function on exit that will call cudaFree
  // on any cudaMalloc'ed space
  my_display.RegisterExitFunction(clean_up);

  // have the library run our Cuda animation
  my_display.AnimateComputation(compute_LCS);

  return 0;
}//end main

/**************************************************************/
//FUNCTION: clean_up
// passed to AnimateComputation method.
// it is called when the program exits and should clean up
// all cudaMalloc'ed state.  Your clean-up function's prototype
// must match this, which means you need a global(s) that point
// to anything cudaMalloc'ed so you can free them here
/**************************************************************/
static void clean_up(void) {
  cudaFree(cudaData.read_grid);
  cudaFree(cudaData.dev_random);
}


/* computeLCS
 * @ inputs: devPtr
 * @ inputs: my_data - a struct consisting of important data for LCS
 * 
 * This function computes the LCS of two strings.
 */
static void compute_LCS(uchar4 *devPtr, void *my_data) {
  //printf("INSIDE ANIMATE FIRE\n");
  char place_holder;

  //divvy up blocks and threads
  my_cuda_data * cudaData = (my_cuda_data *)my_data;

  //count iterations
  int c_iters = cudaData->cur_iters;
  cudaData->cur_iters = c_iters + 1;

  int N = cudaData->N;

  int thread_count;
  //printf("HERE HERE HERE\n");
  //char* A = cudaData->string_a;
  //printf("%c\n", A[2]);
  //printf("HERE HERE HERE\n");
  thread_count = 32;

  //set up blocks
  dim3 blocks(N/thread_count,N/thread_count,1);
  dim3 threads_block(thread_count,thread_count,1);

  //if we've done the max number of iterations, handle timing info and quit
  if (c_iters < 2* N + 1){
    LCS_kernel<<<blocks, threads_block>>>(*cudaData);
    color_in_kernel<<<blocks,threads_block>>>(devPtr, *cudaData);
  }
  else if (c_iters < 4 * N + 2){
    retrace_kernel<<<blocks, threads_block>>>(*cudaData);
    color_in_kernel<<<blocks,threads_block>>>(devPtr, *cudaData);
  }
  else{
    //find the colors of cells
    float tim;
    cudaEventRecord(cudaData->stop, 0);
    cudaEventSynchronize(cudaData->stop);
    cudaEventElapsedTime(&tim, cudaData->start, cudaData->stop);
    //clean_up();

    scanf("%c", &place_holder);

    kill(getpid(), SIGKILL);
  }
}//end animate_fire


/* color_in_kernel
 * @ inputs: optr
 * @ inputs: data - cuda data struct for holding useful info
 * 
 * kernel dedicated to coloring cells in
 */
__global__ void color_in_kernel(uchar4 *optr, my_cuda_data data) {
  //get the offset so we can map from 2d array into 1d
  const int* fire_state = data.read_grid;
  int N = data.N;

  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;
  int offset = x + y*N;

  //change pixel colors based off world values
  int state;
  state = fire_state[offset];

  if(state < 0){ //cell not yet filled in
    colorPix(optr, offset, 0);
  }

  else if(state > N){//cell filled in 
    colorPix(optr, offset, 2);
  }

  else{
    colorPix(optr, offset, 1);
  }
}// end fire_to_color


/* LCS_kernel
 * @ inputs: data - a struct containing useful data for solving LCS 
 * 
 * this function either:
 *    increments a cell's age by 1
 *    computes the LCS value of a cell based on the LCS algorithm if age = 0
 */
__global__ void  LCS_kernel(my_cuda_data data){

  int N = data.N;
  int* read_data = data.read_grid;
  int* string_a = data.string_a;
  int* string_b = data.string_b;

  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;
  int offset = x + y*N;

  int state = read_data[offset];

  if (state < 0){
    read_data[offset] = state+1;
  }

  else if (state > 0){
  }

  else{
    if (x == 0){
      //printf("basecasing x\n");
      read_data[offset] = 0;
    }
    else if (y == 0){
      //printf("basecasing y\n");
      read_data[offset] = 0;
    }

    else if (string_a[x] == string_b[y]){
      //printf("Found matching letters\n");
      int old_offset = (x-1) + (y-1)*N; 
      read_data[offset] = read_data[old_offset] + 1;
    }

    else{
      //printf("maximizing from old\n");
      int old_offset1 = (x-1) + y*N;
      int old_offset2 = x + (y-1)*N;
      int max_sublen1 = read_data[old_offset1];
      int max_sublen2 = read_data[old_offset2];

      if (max_sublen1 > max_sublen2){
        read_data[offset] = max_sublen1;
      }
      else{
        read_data[offset] = max_sublen2;
      }
    }
  }
}//end fire_sim_kernel


/* retrace_kernel
 * @ inputs: data - a struct containing useful data for solving LCS 
 * 
 * retraces the final LCS value backwards
 */
__global__ void  retrace_kernel(my_cuda_data data){
  int N = data.N;
  int* read_data = data.read_grid;
  int* string_a = data.string_a;
  int* string_b = data.string_b;

  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;
  int offset = x + y*N;

  int state = read_data[offset];

  if ((x == (N-2)) && (y == (N-2))){
    read_data[offset] += N + 1;
  }
  if (state > N){
    if (string_a[x] == string_b[y]){
      int new_offset = (x-1) + (y-1)*N;
      read_data[new_offset] += N + 1;
    }
    else{
      int old_offset1 = (x-1) + y*N;
      int old_offset2 = x + (y-1)*N;
      int max_sublen1 = read_data[old_offset1];
      int max_sublen2 = read_data[old_offset2];

      if (max_sublen1 > max_sublen2){
        read_data[old_offset1] += N + 1;
      }
      else{
        read_data[old_offset2] += N + 1; 
      }     
    }
  }
} 

/*colorPix
 * @ inputs: optr
 * @ inputs: offset - offset to denote cell being colored
 * @ inputs: type - denotes the color to be set based on type
 * 
 *colors a pixel
 *type determines if it is an unfilled cell (0), filled cell(1),
 */
__device__ void colorPix(uchar4 *optr, int offset, int type){
  int x = 0, y = 0, z = 0;
  if (type == 0){
    z = 255;
  }
  else if (type == 1){
    y = 255;
  }
  else if (type == 2){
    x = 255;
  }

  optr[offset].x = x;   //R
  optr[offset].y = y;   //G
  optr[offset].z = z; //B
  optr[offset].w = 255; //opacity
}
