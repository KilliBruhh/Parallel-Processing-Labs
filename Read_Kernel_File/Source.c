
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "CL/cl.h"
#include "../OpenCL_StartersLibrary/functions.c"
#include "../ImageFunctionsLib/bmpfuncs.h"
#include "../ImageFunctionsLib/bmpfuncs.c"
#include <stdint.h>

#define OCLFILE = "kernel.cl"

int main(void)
{
    printf("Exercice Reading Kernel File \n");
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue command_queue;
    cl_program program;
    cl_kernel kernel;

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

    // Initialize OpenCLe
    platform = slectOpenClPlatforms();
    device = selectOpenClDevice(platform);
    context = createContext(device);
    command_queue = createCommandQueue(context, device);

    // Create memory buffers on devices for output
    cl_mem output_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 14 * sizeof(char), NULL, NULL);
    // Create program from kernel source
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&sourceCode, (const size_t*)&sourceCode, NULL);
    // create kernel
    cl_kernel kernel = clCreateKernel(program, "helloWorld", NULL);
    // set kernel arguments
    clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&output_buffer);

    // Execute the OpenCL kernel
    size_t global_item_size = 1; // Number of work items in the work group
    clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, NULL, 0, NULL, NULL);

    // Read the output buffer back to the host
    char output[14];
    clEnqueueReadBuffer(command_queue, output_buffer, CL_TRUE, 0, 14 * sizeof(char), output, 0, NULL, NULL);

    // Print the result
    printf("%s\n", output);

    // Cleanup
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);


    return 0;
}