// Lab 3 Oef 2 - Matrix Multiplication

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "CL/cl.h"
#include "../OpenCL_StartersLibrary/functions.c"

#define MATRIX_SIZE_M 3
#define MATRIX_SIZE_N 4
#define MATRIX_SIZE_P 5

void printMatrix(float* matrix, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            printf("%f ", matrix[i * cols + j]);
        }
        printf("\n");
    }
}


int main(void)
{
    printf("Exercice 2_2_B \n");
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

    // kernel Code for Matrix Multiplication
    const char* source_code =
        "__kernel void matrixMultiplication(__global float* A, __global float* B, __global float* C, int M, int N, int P) {\n"
        "    int row = get_global_id(0);\n"
        "    int col = get_global_id(1);\n"
        "    float sum = 0.0f;\n"
        "    for (int i = 0; i < N; i++) {\n"
        "        sum += A[row * N + i] * B[i * P + col];\n"
        "    }\n"
        "    C[row * P + col] = sum;\n"
        "}\n";

    // Set up Matrix Dimention
    int M = MATRIX_SIZE_M;
    int N = MATRIX_SIZE_N;
    int P = MATRIX_SIZE_P;

    size_t matrixSizeA = M * N;
    size_t matrixSizeB = N * P;
    size_t matrixSizeC = M * P;

    // Run the OpenCl Program and create Kernal
    program = createProgram(context, device, source_code);
    kernel = createKernel(program, "matrixMultiplication"); 

    // Create the matrixes (A, B, C)
    float* A = (float*)malloc(matrixSizeA * sizeof(float));
    float* B = (float*)malloc(matrixSizeB * sizeof(float));
    float* C = (float*)malloc(matrixSizeC * sizeof(float));

    // Fill the matrixes
    for (int i = 0; i < matrixSizeA; ++i) {
        A[i] = rand() % 10;
    }
    for (int i = 0; i < matrixSizeB; ++i) {
        B[i] = rand() % 10;
    }

    // Buffers to hold data
    cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY, matrixSizeA * sizeof(float), NULL, NULL);
    cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY, matrixSizeB * sizeof(float), NULL, NULL);
    cl_mem bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, matrixSizeC * sizeof(float), NULL, NULL);

    clEnqueueWriteBuffer(command_queue, bufferA, CL_TRUE, 0, matrixSizeA * sizeof(float), A, 0, NULL, NULL);
    clEnqueueWriteBuffer(command_queue, bufferB, CL_TRUE, 0, matrixSizeB * sizeof(float), B, 0, NULL, NULL);

    // Calculate the results of multiplication
    // Calculate the results of multiplication
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferA);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferB);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferC);
    clSetKernelArg(kernel, 3, sizeof(int), &M);
    clSetKernelArg(kernel, 4, sizeof(int), &N);
    clSetKernelArg(kernel, 5, sizeof(int), &P);

    // Execute the kernel
    size_t globalWorkSize[2] = { M, P };
    clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, globalWorkSize, NULL, 0, NULL, NULL);

    // Retrieve Matrix C From the device
    clEnqueueReadBuffer(command_queue, bufferC, CL_TRUE, 0, matrixSizeC * sizeof(float), C, 0, NULL, NULL);

    // Print matrices A and B
    printf("Matrix A:\n");
    printMatrix(A, M, N);
    printf("\nMatrix B:\n");
    printMatrix(B, N, P);

    // Print the result matrix C
    printf("\nResult Matrix C:\n");
    printMatrix(C, M, P);

    // Cleanup
    free(A);
    free(B);
    free(C);

    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);

    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);


    return 0;
}