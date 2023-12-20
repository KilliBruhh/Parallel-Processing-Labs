
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
    printf("Exercice ... \n");
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
    FILE* fp = fopen("C:/Users/Killi/OneDrive/Documenten/School/Fase 4/Parralel Processing/Lab/ReadKernelFile/kernel.cl", "rb");

    fseek(fp, 0, SEEK_END);
    fileLength = ftell(fp);

    sourceCode = (char*)malloc(sizeof(char) * fileLength + 1);
    rewind(fp);

    fread(sourceCode, sizeof(char), fileLength, fp);
    sourceCode[fileLength] = '\0';



    // Cleanup
    // clReleaseKernel(kernel);
    // clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);


    return 0;
}