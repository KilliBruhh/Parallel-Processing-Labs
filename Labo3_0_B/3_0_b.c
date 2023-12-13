
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
"__kernel void grayscale(__global float* inputImage, __global float* outputImage, const int width, const int height) {\n"
"    int gidX = get_global_id(0);\n"
"    int gidY = get_global_id(1);\n"
"    int index = gidY * width + gidX;\n"
"    float pixelValue = inputImage[index];\n"
"    float grayValue = pixelValue;\n"
"    outputImage[index] = grayValue;\n"
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
    float* inputImage = readImage("C:/Users/Killi/OneDrive/Documenten/School/Fase 4/Parralel Processing/Lab/!Images/input.bmp", &width, &height);
  
    // Initialize OpenCL
    platform = slectOpenClPlatforms();
    device = selectOpenClDevice(platform);
    context = createContext(device);
    command_queue = createCommandQueue(context, device);
    
    // build the program
    program = buildProgram(context, device, kernelSource);
    
    // Create the kernel
    kernel = createKernel(program, "grayscale");
    // Allocate memory for the output image
    float* outputImage = (float*)malloc(sizeof(float) * width * height);

    // Create OpenCL buffers for input and output images
    cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * width * height, inputImage, NULL);
    cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * width * height, NULL, NULL);

    // Define global and local work sizes
    size_t global_work_size[] = { width, height };  // Adjust based on your image size

    // Enqueue and run the kernel
    clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_work_size, NULL, 0, NULL, NULL);

    // Read the result back to the host
    clEnqueueReadBuffer(command_queue, outputBuffer, CL_TRUE, 0, sizeof(float) * width * height, outputImage, 0, NULL, NULL);

    // Store the grayscale output image
    storeImage(outputImage, "C:/Users/Killi/OneDrive/Documenten/School/Fase 4/Parralel Processing/Lab/!Images/output.bmp", width, height, "C:/Users/Killi/OneDrive/Documenten/School/Fase 4/Parralel Processing/Lab/!Images/input.bmp");

    // Cleanup
    free(inputImage);
    free(outputImage);
    cleanupOpenCLResources(kernel, program, command_queue, context);
    clReleaseMemObject(inputBuffer);
    clReleaseMemObject(outputBuffer);

    printf("Grayscale conversion completed!\n");

    return 0;
}