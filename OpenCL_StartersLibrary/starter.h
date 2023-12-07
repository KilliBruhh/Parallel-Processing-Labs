#pragma once

#include "CL/cl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Error handeling
void checkOpenClErrors(cl_int error, const char*);
// Creating start OpenCl
cl_platform_id slectOpenClPlatforms();
cl_device_id selectOpenClDevice(cl_platform_id platform);
cl_context createContext(cl_device_id device);
cl_command_queue createCommandQueue(cl_context context, cl_device_id device);
cl_program createProgram(cl_context context, cl_device_id divice, const char* source_code);
cl_kernel createKernel(cl_program program, const char* kernel_name);
char* readKernelSource(const char* filename);
cl_int enqueueNDRangeKernel(cl_command_queue command_queue, cl_kernel kernel,
    cl_uint work_dim, const size_t* global_work_size,
    const size_t* local_work_size, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list, cl_event* event);
cl_program buildProgram(cl_context context, cl_device_id device, const char* kernelSource);

// Cleanup 
void cleanupOpenCLResources(cl_kernel kernel, cl_program program, cl_command_queue command_queue, cl_context context);

