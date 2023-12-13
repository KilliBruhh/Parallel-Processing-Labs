// Lab  Oef 0 - Testing the image stuff

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "CL/cl.h"
#include "../OpenCL_StartersLibrary/functions.c"
#include "../OpenCL_StartersLibrary/starter.h"
#include "../ImageFunctionsLib/bmpfuncs.h"
#include "../ImageFunctionsLib/bmpfuncs.c"


int main(void)
{
    printf("Exercice 3_0_A \n");
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

    // image variables
    int width, height;
    float* image = readImage("C:/Users/Killi/OneDrive/Documenten/School/Fase 4/Parralel Processing/Lab/!Images/input.bmp", &width, &height);

    // initialize Kernel
    // Initialize Kernel
    char* kernelSource = readKernelSource("kernel.cl");
    if (kernelSource == NULL) {
        printf("ERROR: Reading kernel source file\n");
        return -1;
    }

    program = createProgram(context, device, &kernelSource);

    // Build the program
    if (buildProgram(program, device, kernelSource) != CL_SUCCESS) {
        printf("Error building program\n");
        free(kernelSource); // Free the memory only after attempting to build the program
        return -1;
    }

    // Continue with the rest of your code...


    // Create the kernel
    kernel = clCreateKernel(program, "invertColors", NULL);

    // Set Kernel adjustmensts
    float adjustment = 50.0f;
    cl_mem imageBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float) * width * height, image, NULL);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &imageBuffer);
    clSetKernelArg(kernel, 1, sizeof(int), &width);
    clSetKernelArg(kernel, 2, sizeof(int), &height);
    clSetKernelArg(kernel, 3, sizeof(float), &adjustment);

    // Execute the kernel
    size_t globalWorkSize[2] = { width, height };
    if (enqueueNDRangeKernel(command_queue, kernel, 2, NULL, globalWorkSize, NULL, 0, NULL, NULL) != CL_SUCCESS) {
        printf("Error enqueuing kernel\n");
        return -1;
    }

    // Read back the modified image
    clEnqueueReadBuffer(command_queue, imageBuffer, CL_TRUE, 0, sizeof(float) * width * height, image, 0, NULL, NULL);

    // Store the modified image
    storeImage(image, "C:/Users/Killi/OneDrive/Documenten/School/Fase 4/Parralel Processing/Lab/!Images/output.bmp", height, width, "C:/Users/Killi/OneDrive/Documenten/School/Fase 4/Parralel Processing/Lab/!Images/input.bmp");


    // Cleanup
    clReleaseMemObject(imageBuffer);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);

    free(image);

    printf("Image processing completed!\n");



    return 0;
}