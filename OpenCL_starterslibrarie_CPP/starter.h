#pragma once

#include "CL/cl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Error handeling
void checkOpenClErrors(cl_int error, const char*);
cl_platform_id slectOpenClPlatforms();
cl_device_id selectOpenClDevice(cl_platform_id platform);
cl_context createContext(cl_device_id device);
cl_command_queue createCommandQueue(cl_context context, cl_device_id device);
cl_program createProgram(cl_context context, cl_device_id device, const char* source_code);
cl_kernel createKernel(cl_program program, const char* kernel_name);
void cleanupOpenCLResources(cl_kernel kernel, cl_program program, cl_command_queue command_queue, cl_context context);
