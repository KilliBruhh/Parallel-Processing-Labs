#define _CRT_SECURE_NO_WARNINGS
#include "CL/cl.h"
#include "starter.h"
#include <iostream>
#include <cstdlib>

void checkOpenClErrors(cl_int error, const char* message)
{
    if (error != CL_SUCCESS) {
        std::cerr << "OpenCl error: (" << error << "): " << message << std::endl;
        std::exit(1);
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
    cl_device_id device;
    cl_uint num_devices;

    clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, &num_devices);
    checkOpenClErrors(num_devices == 0, "No OpenCL devices available");

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
        char buildLog[4096];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buildLog), buildLog, NULL);
        std::cerr << "Build failed: " << buildLog << std::endl;
        clReleaseProgram(program);  // Release the program if the build fails.
        std::exit(EXIT_FAILURE);  // Return EXIT_FAILURE to indicate the compilation failure.
    }

    return program;
}

cl_kernel createKernel(cl_program program, const char* kernel_name) {
    cl_kernel kernel = clCreateKernel(program, kernel_name, NULL);

    return kernel;
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

int compileOpenCl(cl_context context, cl_device_id device, const char* source_code) {
    cl_program program = clCreateProgramWithSource(context, 1, &source_code, NULL, NULL);
    checkOpenClErrors(program == NULL, "Failed to create program");

    cl_int buildStatus = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if (buildStatus != CL_SUCCESS) {
        char buildLog[4096];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buildLog), buildLog, NULL);
        std::cerr << "Build failed: " << buildLog << std::endl;
        clReleaseProgram(program);  // Release the program if the build fails.
        return EXIT_FAILURE;  // Return EXIT_FAILURE to indicate the compilation failure.
    }

    return 0;
}
