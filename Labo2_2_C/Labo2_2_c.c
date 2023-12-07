// Lab 3 Oef 3 - Matrix Averages

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "CL/cl.h"
#include "../OpenCL_StartersLibrary/functions.c"

int main(void)
{
    printf("Exercice 2_2_C \n");
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

    // Cleanup
    // clReleaseKernel(kernel);
    // clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);


    return 0;
}