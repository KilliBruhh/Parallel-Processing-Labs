#define _CRT_SECURE_NO_WARNINGS
#include "CL/cl.h"
#include "starter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void checkOpenClErrors(cl_int error, const char* message)
{
    if (error != CL_SUCCESS) {
        printf("OpenCl error: (%d): %s:\n", error, message);
        exit(1);
    }
}

cl_platform_id slectOpenClPlatforms() {
    cl_platform_id platform;
    cl_uint num_platforms;

    clGetPlatformIDs(1, &platform, &num_platforms);
    checkOpenClErrors(num_platforms == 0, "No OpenCL platforms available");

    return platform;
}

cl_device_id selectOpenClDevice(cl_platform_id platform) {
    cl_device_id device = NULL; // Initialize to NULL

    cl_uint num_devices;
    cl_int err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
    if (err != CL_SUCCESS || num_devices == 0) {
        fprintf(stderr, "Error getting OpenCL devices.\n");
        return NULL;
    }

    cl_device_id* devices = (cl_device_id*)malloc(num_devices * sizeof(cl_device_id));
    if (!devices) {
        fprintf(stderr, "Error allocating memory for devices.\n");
        return NULL;
    }

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error getting OpenCL device IDs.\n");
        free(devices);
        return NULL;
    }

    printf("Available OpenCL devices:\n");

    for (int i = 0; i < num_devices; i++) {
        cl_char vendor[1024];
        cl_char deviceName[1024];

        clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, sizeof(vendor), vendor, NULL);
        clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(deviceName), deviceName, NULL);

        printf("%d. Vendor: %s, Device: %s\n", i + 1, vendor, deviceName);
    }

    // Get the user's choice (you might want to add error checking for the input)
    int selectedDevice;
    printf("Enter the number of the OpenCL device you want to use: ");
    scanf("%d", &selectedDevice);

    // Check if the selected device number is valid
    if (selectedDevice >= 1 && selectedDevice <= num_devices) {
        device = devices[selectedDevice - 1];
    }
    else {
        fprintf(stderr, "Invalid device number.\n");
    }

    free(devices);
    return device;
}

cl_context createContext(cl_device_id device) {
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
    checkOpenClErrors(context == NULL, "ERROR with creating Context");
    return context;
}

cl_command_queue createCommandQueue(cl_context context, cl_device_id device) {
    cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device, 0, NULL);
    checkOpenClErrors(command_queue == NULL, "ERROR with creating Command Queue");
    return command_queue;
}

cl_program createProgram(cl_context context, cl_device_id device, const char* source_code) {
    cl_program program = clCreateProgramWithSource(context, 1, &source_code, NULL, NULL);
    checkOpenClErrors(program == NULL, "Failed to create program");

    cl_int buildStatus = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if (buildStatus != CL_SUCCESS) {


    }

    return program;
}


cl_kernel createKernel(cl_program program, const char* kernel_name) {
    cl_kernel kernel = clCreateKernel(program, kernel_name, NULL);
    return kernel;
}

char* readKernelSource(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    // Get the size of the file
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for the source code
    char* sourceCode = (char*)malloc(size + 1);
    if (!sourceCode) {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    // Read the source code into the buffer
    size_t bytesRead = fread(sourceCode, 1, size, file);
    fclose(file);

    if (bytesRead != (size_t)size) {
        perror("Error reading file");
        free(sourceCode);
        return NULL;
    }

    // Null-terminate the string
    sourceCode[size] = '\0';

    return sourceCode;
}


void cleanupOpenCLResources(cl_kernel kernel, cl_program program, cl_command_queue command_queue, cl_context context) {
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);
}

int startOpenCl() {
    cl_platform_id platform = slectOpenClPlatforms();
    cl_device_id device = selectOpenClDevice(platform);
    cl_context context = createContext(device);
    cl_command_queue command_queue = createCommandQueue(context, device);

    const char* source_code = "__kernel void hello() { printf(\"kernel Sourcecode!\\n\"); }";
    cl_program program = createProgram(context, device, source_code);
    cl_kernel kernel = createKernel(program, "hello");

    // Set kernel arguments (if needed).

    // Define global and local work sizes.
    size_t global_work_size[] = { 1 }; // Adjust this based on your requirements.

    // Enqueue and run the kernel using clEnqueueNDRangeKernel.
    clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);

    // Wait for the kernel to finish.
    clFinish(command_queue);

    cleanupOpenCLResources(kernel, program, command_queue, context);

    return 0;
}


cl_program compileOpenCl(cl_context context, cl_device_id device, const char* source_code) {
    cl_program program = clCreateProgramWithSource(context, 1, &source_code, NULL, NULL);
    checkOpenClErrors(program == NULL, "Failed to create program");

    cl_int buildStatus = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if (buildStatus != CL_SUCCESS) {
        char buildLog[4096];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buildLog), buildLog, NULL);
        printf("Build failed: %s\n", buildLog);
        clReleaseProgram(program);  // Release the program if the build fails.
        return NULL;  // Return NULL to indicate the compilation failure.
    }

    return program;
}

// Wierd thing
cl_int enqueueNDRangeKernel(cl_command_queue command_queue, cl_kernel kernel, cl_uint work_dim,
    const size_t* global_work_size, const size_t* local_work_size,
    cl_uint num_events_in_wait_list, const cl_event* event_wait_list,
    cl_event* event) {
    // Perform additional checks or modifications before enqueuing the kernel
    if (global_work_size[0] % local_work_size[0] != 0 || global_work_size[1] % local_work_size[1] != 0) {
        printf("Warning: Global work size is not a multiple of local work size.\n");
    }

    // Call the original clEnqueueNDRangeKernel function
    return clEnqueueNDRangeKernel(command_queue, kernel, work_dim, NULL, global_work_size, local_work_size,
        num_events_in_wait_list, event_wait_list, event);
}

// Wierd building thing

cl_program buildProgram(cl_context context, cl_device_id device, const char* kernelSource) {
    cl_int err;

    // Create program
    cl_program program = clCreateProgramWithSource(context, 1, &kernelSource, NULL, &err);
    if (err != CL_SUCCESS) {
        printf("Error creating program: %d\n", err);
        return NULL;
    }

    // Build program
    err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        size_t log_size;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

        char* log = (char*)malloc(log_size);
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

        printf("Error building program: %d\n", err);
        printf("Build log:\n%s\n", log);

        free(log);
        clReleaseProgram(program);
        return NULL;
    }

    return program;
}