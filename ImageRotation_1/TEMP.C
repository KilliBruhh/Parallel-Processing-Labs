#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "CL/cl.h"
#include "../OpenCL_StartersLibrary/functions.c"
#include "../OpenCL_StartersLibrary/starter.h"
#include "../ImageFunctionsLib/bmpfuncs.h"
#include "../ImageFunctionsLib/bmpfuncs.c"
#include <stdint.h>

int main(void)
{
    printf("Exercice Image Rotation \n");
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue command_queue;
    cl_program program;
    cl_kernel kernel;

    int W, H;
    float* inputImage = readImage("C:/Users/Killi/OneDrive/Documenten/School/Fase 4/Parralel Processing/Lab/!Images/input.bmp", &W, &H);

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

    // build the program
    program = buildProgram(context, device, sourceCode);

    // Create the kernel
    kernel = createKernel(program, "img_rotate");
    // Allocate memory for the output image
    float* outputImage = (float*)malloc(sizeof(float) * W * H);

    // Declare Buggers for input and output data (W = width, H = height)
    cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, W * H * sizeof(float), inputImage, NULL);
    cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, W * H * sizeof(float), NULL, NULL);
    size_t globar_work_size[] = { W, H };

    // Enqueu and run the kernel
    clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, globar_work_size, NULL, 0, NULL, NULL);

    // Read result back to the host
    clEnqueueReadBuffer(command_queue, outputBuffer, CL_TRUE, 0, sizeof(float) * W * H, outputBuffer, 0, NULL, NULL);

    // Store the image
    storeImage(outputImage, "C:/Users/Killi/OneDrive/Documenten/School/Fase 4/Parralel Processing/Lab/!Images/output.bmp", W, H, "C:/Users/Killi/OneDrive/Documenten/School/Fase 4/Parralel Processing/Lab/!Images/input.bmp");

    // Image Variables
    // Cleanup
    // clReleaseKernel(kernel);
    // clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);


    return 0;
}