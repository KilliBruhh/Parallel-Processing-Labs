#define _CRT_SECURE_NO_WARNINGS

#include "starter.h"



int main(void)
{
	cl_platform_id platform = slectOpenClPlatforms();
	
	cl_device_id device = selectOpenClDevice(platform);
	cl_context context = createContext(device);
	
	cl_command_queue command_queue = createCommandQueue(context, device);

	const char* source_code = "__kernel void hello() { printf(\"Hello, OpenCL!\\n\"); }";
	cl_program program = createProgram(context, device, source_code);
	cl_kernel kernel = createKernel(program, "Kernel");


	cleanupOpenCLResources(kernel, program, command_queue, context);
	return 0;
}