
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "CL/cl.h"
#include "../OpenCL_StartersLibrary/functions.c"
#include "../OpenCL_StartersLibrary/starter.h"
#include "../ImageFunctionsLib/bmpfuncs.h"
#include "../ImageFunctionsLib/bmpfuncs.c"

#define MAX_SOURCE_SIZE (0x10000)  // 64 KB

const char* kernelSource =
"__kernel void boxBlur(__global float* inputImage, __global float* outputImage, const int width, const int height) {\n"
"    int gidX = get_global_id(0);\n"
"    int gidY = get_global_id(1);\n"
"    int blurRadius = 1;\n"
"    int index = gidY * width + gidX;\n"
"    float blurColor = 0.0f;\n"
"    int numPixels = 0;\n"
"    for (int i = -blurRadius; i <= blurRadius; ++i) {\n"
"        for (int j = -blurRadius; j <= blurRadius; ++j) {\n"
"            int currentX = gidX + j;\n"
"            int currentY = gidY + i;\n"
"            if (currentX >= 0 && currentX < width && currentY >= 0 && currentY < height) {\n"
"                int currentIndex = currentY * width + currentX;\n"
"                blurColor += inputImage[currentIndex];\n"
"                numPixels++;\n"
"            }\n"
"        }\n"
"    }\n"
"    outputImage[index] = blurColor / numPixels;\n"
"}\n";

int main(void)
{
    printf("??? - Exercice 3_0_B \n");
    // Cl variables
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue command_queue;
    cl_program program;
    cl_kernel kernel;
    // Image variables
    int width, height;
    float* inputImage = readImage("C:/Users/Killi/OneDrive/Documenten/School/Fase 4/Parralel Processing/Lab/Images/input.bmp", &width, &height);

    // Load kernel code
    // Load OpenCL kernel code from file
    /*
    FILE* kernelFile = fopen("kernel.cl", "r");
    if (!kernelFile) {
        perror("Error opening kernel file");
        exit(EXIT_FAILURE);
    }
    fseek(kernelFile, 0, SEEK_END);
    size_t kernelFileSize = ftell(kernelFile);
    rewind(kernelFile);
    char* kernelSource = (char*)malloc(kernelFileSize + 1);
    if (!kernelSource) {
        perror("Error allocating memory for kernel source");
        fclose(kernelFile);
        exit(EXIT_FAILURE);
    }
    fread(kernelSource, 1, kernelFileSize, kernelFile);
    kernelSource[kernelFileSize] = '\0';
    fclose(kernelFile);

    // Initialize OpenCL
    platform = slectOpenClPlatforms();
    device = selectOpenClDevice(platform);
    context = createContext(device);
    command_queue = createCommandQueue(context, device);
    */

    // Create Buffers for input and output
    cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * width * height, inputImage, NULL);
    cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, NULL, NULL, NULL);

    // Create program
    program = clCreateProgramWithSource(context, 1, &kernelSource, NULL, NULL);

    // print kernel source (debugging)
    printf("!!! - kernel Source: ", &kernelSource);

    // Build the program
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);

    // print Program build log
    char buildLog[MAX_SOURCE_SIZE];
    size_t logSize;
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buildLog), buildLog, &logSize);
    printf("!!! - Build Log:\n%s\n", buildLog);


    // Create Kernel
    kernel = clCreateKernel(program, "blurKernel", NULL);

    // Set the kernel arguments
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputBuffer);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &outputBuffer);
    clSetKernelArg(kernel, 2, sizeof(int), &width);
    clSetKernelArg(kernel, 3, sizeof(int), &height);

    // Execute the kernel
    size_t globalSize[] = { width,height };
    clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, globalSize, NULL, 0, NULL, NULL);
    clFinish(command_queue);

    // Read the result back to the host
    float* outputImage = (float*)malloc(sizeof(float) * width * height);
    clEnqueueReadBuffer(command_queue, outputBuffer, CL_TRUE, 0, sizeof(float) * width * height, outputImage, 0, NULL, NULL);

    // Store the result using your storeImage function
    storeImage(outputImage, "output.bmp", height, width, "input.bmp");



    // Cleanup
    free(inputImage);
    free(outputImage);
    free(kernelSource);
    clReleaseMemObject(inputBuffer);
    clReleaseMemObject(outputBuffer);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);
    printf("??? - All Cleaned Up");

    return 0;
}