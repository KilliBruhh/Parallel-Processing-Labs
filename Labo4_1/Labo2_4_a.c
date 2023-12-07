// Lab 4 Oef 1 - Image rotation 

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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


    const char* source_code =
        "__kernel void rotateImage(__global uchar* inputImage, __global uchar* outputImage, int width, int height, float angle, int xPivot, int yPivot) {\n"
        "    int x = get_global_id(0);\n"
        "    int y = get_global_id(1);\n"
        "    int inputIndex = y * width + x;\n"
        "    int outputX = (int)((x - xPivot) * cos(angle) - (y - yPivot) * sin(angle) + xPivot);\n"
        "    int outputY = (int)((x - xPivot) * sin(angle) + (y - yPivot) * cos(angle) + yPivot);\n"
        "    if (outputX >= 0 && outputX < width && outputY >= 0 && outputY < height) {\n"
        "        int outputIndex = outputY * width + outputX;\n"
        "        outputImage[outputIndex] = inputImage[inputIndex];\n"
        "    }\n"
        "}\n";

    // Initialize OpenCL
    platform = slectOpenClPlatforms();
    device = selectOpenClDevice(platform);
    context = createContext(device);
    command_queue = createCommandQueue(context, device);
    program = createProgram(context, device, source_code);

    // Image properties (width, height, pivot point, rotation angle)                                                                            
    int width = 4;
    int height = 4;
    int xPivot = 2;
    int yPivot = 2;
    float angle = CL_M_PI / 4.0; // Rotate by 45 degrees (pi/4 radians)

    // Create input and output images (dynamically allocated arrays)
    unsigned char* inputImage = (unsigned char*)malloc(sizeof(unsigned char) * width * height);
    unsigned char* outputImage = (unsigned char*)malloc(sizeof(unsigned char) * width * height);

    // Check for memory allocation errors
    if (inputImage == NULL || outputImage == NULL) {
        printf("Memory allocation error.\n");
        return 1;
    }

     // Populate the input image (use your own image data)
    printf("Original Image:\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            inputImage[i * width + j] = i * width + j + 1;
            printf("%d ", inputImage[i * width + j]);
        }
        printf("\n");
    }


    // Create buffers for images on the device
    cl_mem bufferInputImage = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(unsigned char) * width * height, inputImage, NULL);
    cl_mem bufferOutputImage = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
        sizeof(unsigned char) * width * height, NULL, NULL);

    // Check for buffer creation errors
    if (bufferInputImage == NULL || bufferOutputImage == NULL) {
        printf("Buffer creation error.\n");
        free(inputImage);
        free(outputImage);
        return 1;
    }

    // Create the rotation kernel
    kernel = clCreateKernel(program, "rotateImage", NULL);

    // Check for kernel creation errors
    if (kernel == NULL) {
        printf("Kernel creation error.\n");
        free(inputImage);
        free(outputImage);
        clReleaseMemObject(bufferInputImage);
        clReleaseMemObject(bufferOutputImage);
        return 1;
    }

    // Set kernel arguments
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferInputImage);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferOutputImage);
    clSetKernelArg(kernel, 2, sizeof(int), &width);
    clSetKernelArg(kernel, 3, sizeof(int), &height);
    clSetKernelArg(kernel, 4, sizeof(float), &angle);
    clSetKernelArg(kernel, 5, sizeof(int), &xPivot);
    clSetKernelArg(kernel, 6, sizeof(int), &yPivot);

    // Define the global work size for image size
    size_t globalWorkSize[2] = { width, height };

    // Enqueue the rotation kernel for execution
    clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, globalWorkSize, NULL, 0, NULL, NULL);
    clFinish(command_queue);

    // Read the result back from the buffer
    clEnqueueReadBuffer(command_queue, bufferOutputImage, CL_TRUE, 0, sizeof(unsigned char) * width * height, outputImage, 0, NULL, NULL);

    // Print the rotated image (outputImage)
    printf("Rotated Image:\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%d ", outputImage[i * width + j]);
        }
        printf("\n");
    }



    // Cleanup
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);


    return 0;
}