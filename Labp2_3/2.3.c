
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "CL/cl.h"
#include "../OpenCL_StartersLibrary/functions.c"
#include "../OpenCL_StartersLibrary/starter.h"
#include "../ImageFunctionsLib/bmpfuncs.h"
#include "../ImageFunctionsLib/bmpfuncs.c"
#include <stdint.h>

#define MATRIX_SIZE 4

void printMatrix(float* matrix, int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            printf("%f ", matrix[i * N + j]);
        }
        printf("\n");
    }
}
int main(void)
{
    printf("Exercice 2_3 - Matrix Avarages \n");
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

    // Read OpenCL Source
    char* sourceCode;
    uint32_t fileLength = 0;
    FILE* fp = fopen("kernel.cl", "rb");
    fseek(fp, 0, SEEK_END);

    fileLength = ftell(fp);
    sourceCode = (char*)malloc(sizeof(char) * fileLength + 1);

    rewind(fp);
    fread(sourceCode, sizeof(char), fileLength, fp);
    sourceCode[fileLength] = '\0';

    printf("Kernel Source:\n%s\n", sourceCode);

    // build program and create kernel
    program = createProgram(context, device, sourceCode);
    kernel = createKernel(program, "matrix_avarages");

    // set up matrix
    int N = MATRIX_SIZE;
    size_t matrixSize = N * N; // 4 by 4

    // Create the matrixes (A, B, C)
    float* A = (float*)malloc(matrixSize * sizeof(float));
    float* B = (float*)malloc(matrixSize * sizeof(float));
    float* C = (float*)malloc(matrixSize * sizeof(float));

    // fill matrix
    for (int i = 0; i < matrixSize; i++) {
        A[i] = rand() % 10;
        B[i] = rand() % 10;
    }

    // Buffers to hold data
    cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY, matrixSize * sizeof(float), NULL, NULL);
    cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY, matrixSize * sizeof(float), NULL, NULL);
    cl_mem bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, matrixSize * sizeof(float), NULL, NULL);

    clEnqueueWriteBuffer(command_queue, bufferA, CL_TRUE, 0, matrixSize * sizeof(float), A, 0, NULL, NULL);
    clEnqueueWriteBuffer(command_queue, bufferB, CL_TRUE, 0, matrixSize * sizeof(float), B, 0, NULL, NULL);

    // Print matrices A and B
    printf("Matrix A:\n");
    printMatrix(A, N);
    printf("\nMatrix B:\n");
    printMatrix(B, N);


    // Calculate the results of multiplication
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferA);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferB);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferC);
    clSetKernelArg(kernel, 3, sizeof(int), &N);

    // Execute the kernel
    size_t globalWorkSize[2] = { N, N };
    clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, globalWorkSize, NULL, 0, NULL, NULL);

    // Retrieve Matrix C From the device
    clEnqueueReadBuffer(command_queue, bufferC, CL_TRUE, 0, matrixSize * sizeof(float), C, 0, NULL, NULL);

    // Print the result matrix C
    printf("\nResult Matrix C:\n");
    printMatrix(C, N);


    // Cleanup
    free(A);
    free(B);
    free(C);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);


    return 0;
}