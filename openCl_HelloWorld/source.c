#include "source.h"
#include "CL/cl.h"
#include "../OpenCL_StartersLibrary/functions.c"

#include <stdio.h>

// Create and compile the OpenCL program
const char* source_code = "__kernel void copyString(__global const char* input, __global char* output) {"
"int index = get_global_id(0);"
"output[index] = input[index];"
"}";

int main(int argc, char* argv[])
{

    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue command_queue;
    cl_program program;
    cl_kernel kernel;

    cl_mem inputBuffer, outputBuffer;

    size_t inputSize = 12; // "Hello World\0"
    size_t outputSize = inputSize;
    char inputString[] = "Hello World";
    char outputString[12];

    // Initialize OpenCL
    platform = slectOpenClPlatforms();
    device = selectOpenClDevice(platform);
    context = createContext(device);
    command_queue = createCommandQueue(context, device);

    // Create and compile the OpenCL program
    const char* source_code = "__kernel void copyString(__global const char* input, __global char* output) {"
        "int index = get_global_id(0);"
        "output[index] = input[index];"
        "}";
    program = createProgram(context, device, source_code);
    kernel = createKernel(program, "copyString");

    // Create input buffer
    inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, inputSize, inputString, NULL);

    // Correct output buffer size
    outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, outputSize, NULL, NULL);

    // Set kernel arguments
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputBuffer);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &outputBuffer);

    // Define global work size
    size_t global_work_size[] = { inputSize };

    // Enqueue and run the kernel
    clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
    clFinish(command_queue);

    // Read the output buffer into the output string
    clEnqueueReadBuffer(command_queue, outputBuffer, CL_TRUE, 0, outputSize, outputString, 0, NULL, NULL);

    // Print the input and output strings
    printf("Input: %s\n", inputString);
    printf("Output:\n");
    for (int i = 0; i < inputSize; i++) {
        printf("%c\n", outputString[i]);
    }
    // Cleanup OpenCL resources
    clReleaseMemObject(inputBuffer);
    clReleaseMemObject(outputBuffer);
    cleanupOpenCLResources(kernel, program, command_queue, context);

    return 0;
}

