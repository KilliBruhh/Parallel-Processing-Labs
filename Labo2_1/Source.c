#include "CL/cl.h"
#include "../OpenCL_StartersLibrary/functions.c"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int getRandomNumber(int min, int max) {
    return rand() % (max - min + 1) + min;
}

int main(int argc, char* argv[]) {
    int vectorSize = 10;

    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue command_queue;
    cl_program program;
    cl_kernel kernel;

    // Initialize OpenCL
    platform = slectOpenClPlatforms();
    device = selectOpenClDevice(platform);
    context = createContext(device);
    command_queue = createCommandQueue(context, device);

    // Create and compile the OpenCL program
    const char* source_code = "__kernel void vectorMultiplication(__global int* A, __global int* B, __global int* C) { int gid = get_global_id(0); C[gid] = A[gid] * B[gid];}";

    program = createProgram(context, device, source_code);
    kernel = createKernel(program, "vectorMultiplication");

    // Create and initialize arrays A, B, and C with random numbers
    int* A = (int*)malloc(sizeof(int) * vectorSize);
    int* B = (int*)malloc(sizeof(int) * vectorSize);
    int* C = (int*)malloc(sizeof(int) * vectorSize);

    srand(time(NULL));

    for (int i = 0; i < vectorSize; i++) {
        A[i] = getRandomNumber(1, 10);  // Random numbers between 1 and 10
        B[i] = getRandomNumber(1, 10);  // Random numbers between 1 and 10
    }

    // Initialize Buffers A, B, and C
    cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * vectorSize, A, NULL);
    cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * vectorSize, B, NULL);
    cl_mem bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * vectorSize, NULL, NULL);

    // Set kernel arguments
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferA);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferB);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferC);
    size_t global_work_size = vectorSize;

    // Enqueue and run the kernel
    clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_work_size, NULL, 0, NULL, NULL);
    clFinish(command_queue);

    // Read the result back to C in the host code
    clEnqueueReadBuffer(command_queue, bufferC, CL_TRUE, 0, sizeof(int) * vectorSize, C, 0, NULL, NULL);

    // Display the result vector C
    printf("Vector A (Result of Multiplication): ");
    for (int i = 0; i < vectorSize; i++) {
        printf("%d ", A[i]);
    }
    printf("\n");
    printf("Vector B (Result of Multiplication): ");
    
    for (int i = 0; i < vectorSize; i++) {
        printf("%d ", B[i]);
    }
    printf("\n");
    printf("Vector C (Result of Multiplication): ");
    for (int i = 0; i < vectorSize; i++) {
        printf("%d ", C[i]);
    }
    printf("\n");

    // Clean up OpenCL resources and memory
    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
    free(A);
    free(B);
    free(C);

    return 0;
}
