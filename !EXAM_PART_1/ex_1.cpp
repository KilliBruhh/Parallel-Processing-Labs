#define _CRT_SECURE_NO_WARNINGS

#include <exception>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <random> 
#include <time.h>
#include <chrono>  
#include <windows.h>
#include <immintrin.h> // for vector instructions
#include <thread>
#include <omp.h>  //openmp

#include "CL/cl.h"
#include "../OpenCL_StartersLibrary/functions.c"
#include "../OpenCL_StartersLibrary/starter.h"


//#include "Versions_O2_NoIntrinsics.h"
//#include "Versions_Od_Intrinsics.h"
//#include "Versions_Od_NoIntrinsics.h"
//#include "OpenCLVersion.h"
//#include "..\OpenCLExample\DeviceQuery.h"

#pragma comment(lib, "user32.lib") // for retrieving CPU information

using namespace std;
using namespace std::chrono;

#define N 200		// Default 200
#define NBR_EXPERIMENTS 3
#define NBR_ALGORITHM_VERSIONS 100 // take big enough

// openCl variables
char* source_code;
cl_platform_id platform;
cl_device_id device;
cl_context context;
cl_command_queue command_queue;
cl_program program;
cl_kernel kernel;
cl_mem bufferA;
cl_mem bufferB;
cl_mem bufferC;
float* A[N][N];
float* B[N][N];
float* C[N][N]; // For Killian version
float* D[N][N]; // For Version 1
float* E[N][N]; // For Version 2
size_t matrixSize;
int N2 = N;
size_t globalWorkSize[2] = { N, N };
// OpenCl Functions
char* getSourceCode();
void initializeOpenCl();
void cleanOpenCl();
void setupMatrix();
void setupBuffers(float arr[N][N], float arr2[N][N]);
void printMatrix(float* matrix);
void executeKernel(float arr[N][N]);
long long runClProgram(float arr[N][N]);

// the different sequential versions
long long mxm_sequential(float first[N][N], float second[N][N], float multiply[N][N]);
long long mxm_multithreaded(float first[N][N], float second[N][N], float multiply[N][N]);


// utility functions
void checkIfResultsAreTheSame(string name, float arr3[N][N], float arr4[N][N], int n);
long long minimalValue(long long* values, int NBR);
long long meanValue(long long* values, int NBR);
long long stddev(long long* values, int NBR);

void analyzePerformance(vector<string> names, long long runtimes[NBR_ALGORITHM_VERSIONS][NBR_EXPERIMENTS], long long totalNbrOfOperations, long long totalNbrOfBytes, long long ticksPerMilliSecond, long long referenceTime);

bool PRINT_DATA = false; // set this to true to print input & output data. Use this to debug
int main(int argc, char* argv[])
{
	long long totalNbrOfOperations = N * N * N * 2;
	long long totalNbrOfBytes = N * N * 12;

	LARGE_INTEGER lpFrequency;
	bool ok = QueryPerformanceFrequency(&lpFrequency);
	long long ticksPerMilliSecond = lpFrequency.QuadPart;


	cout << "** Comparison of Matrix multiplication ** " << endl;
	cout << "Matrix size = " << N << "x" << N << " Nbr Experiments = " << NBR_EXPERIMENTS << " PRINT_DATA = " << PRINT_DATA << " => " << totalNbrOfOperations << " operations and " << totalNbrOfBytes << " bytes transferred" << endl;

	if (ok)
		cout << "CPU frequency = " << (float)ticksPerMilliSecond / 1000000 << "GHz";
	else
		cout << "CPU frequency cannot be identified.";

	// CPU information. 
	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	int nbrCores = siSysInfo.dwNumberOfProcessors;
	cout << " " << nbrCores << " cores" << endl;
	//if (N % nbrCores != 0) {
	//	cout << "N (" << N << ") should be a multiple of the number of cores "<< nbrCores<<" (for the multithreaded versions)!" << endl;
	//	return -1;
	//}

	printf("Hardware information: \n");
	printf("  OEM ID: %u\n", siSysInfo.dwOemId);
	printf("  Number of processors: %u\n", siSysInfo.dwNumberOfProcessors);
	cout << endl;

	srand(time(NULL)); // random initial seed

	// allocation and initialization
	srand(time(NULL)); // random initial seed
	float arr[N][N];
	float arr2[N][N];
	float arr3[N][N];
	float arr4[N][N];
	float arr5[N][N];
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			arr[i][j] = rand();
			arr2[i][j] = rand();
			//	if (PRINT_DATA && i < 10) cout << arr[i] << "*" << arr2[i] << ", ";
			arr3[i][j] = 0; // actually, initialization is not necessary
			arr4[i][j] = 0;
			arr5[i][j] = 0;
		}
	}
	if (PRINT_DATA) cout << endl;


	long long runtimes[NBR_ALGORITHM_VERSIONS][NBR_EXPERIMENTS];
	vector<string> names;

	initializeOpenCl();
	// setupMatrix();
	setupBuffers(arr, arr2);

	for (int t = 0; t < NBR_EXPERIMENTS; t++) { // run multiple times because of fluctuations
		int version = 0;
		// ********* THE FIRST VERSION - THE REFERENCE *********
		runtimes[version++][t] = mxm_sequential(arr, arr2, arr3); // returns time in microseconds
		if (t == 0) names.push_back("MxM with optimizations");

		// ********* THE SECOND VERSION *********	
		runtimes[version++][t] = mxm_multithreaded(arr, arr2, arr4); // returns time in microseconds
		if (t == 0) {
			names.push_back("MxM Multithreaded");
			checkIfResultsAreTheSame(names.back(), arr3, arr4, N);
		}

		// ********* Killian VERSION ********* 
		runtimes[version++][t] = runClProgram(arr5); // returns time in microseconds
		if (t == 0) {
			names.push_back("MxM Killian");
			checkIfResultsAreTheSame(names.back(), arr3, arr5, N);
		}

		// ********* YET ANOTHER VERSION *********


		// ********* ADD VERSIONS HERE *********


		// ********* END OF ALL VERSIONS *********
	}
	long long referenceTime = minimalValue(runtimes[0], NBR_EXPERIMENTS);// FIRST algorithm
	analyzePerformance(names, runtimes, totalNbrOfOperations, totalNbrOfBytes, ticksPerMilliSecond, referenceTime);

	// Deallocate memory
	//delete[] arr; delete[] arr2; delete[] arr3; delete[] arr4;

	cout << endl << "Press ENTER to close window...";
	cleanOpenCl();
	char c = cin.get();
}



//  MATRIX MULTIPLICATION
long long mxm_sequential(float first[N][N], float second[N][N], float multiply[N][N]) {
	time_point<system_clock> now = system_clock::now();

	for (int c = 0; c < N; c++) {
		for (int d = 0; d < N; d++) {
			float sum = 0;
			for (int k = 0; k < N; k++) {
				sum = sum + first[c][k] * second[k][d];
			}

			multiply[c][d] = sum;
		}
	}
	time_point<system_clock> epoch = system_clock::now();
	microseconds us = duration_cast<microseconds>(epoch - now);

	return us.count();
}

// Optimize this fucntion
// Optimize this fucntion
long long mxm_multithreaded(float first[N][N], float second[N][N], float multiply[N][N]) {
	time_point<system_clock> now = system_clock::now();

	for (int c = 0; c < N; c++) {
		for (int d = 0; d < N; d++) {
			float sum = 0;
			for (int k = 0; k < N; k++) {
				// original:
				// sum = sum + first[c][k] * second[d][k];
				// Fixed that V1 and V2 have same algorithm results	
				sum = sum + first[c][k] * second[k][d];
			}

			multiply[c][d] = sum;
		}
	}
	time_point<system_clock> epoch = system_clock::now();
	microseconds us = duration_cast<microseconds>(epoch - now);

	return us.count();
}
//  *** PERFORMANCE ANALYSIS ***
void print_table_title() {
	cout << "    ***** Version Name *****     | min(us)   | mean(us)  | stddev    | GOp/s     |  GB/s     |   CPI     | speedup   |";
	cout << endl;
}
void print_row(string name, long long time, long long time_mean, long long time_stddev, long long nbrOperations, long long nbrBytes, long long ticksPerMilliSecond, long long referenceTime) {
	const char separator = ' ';
	const int nameWidth = 32;
	const int numWidth = 9;

	if (name.length() > nameWidth)
		name = name.substr(0, nameWidth);
	cout << left << setw(nameWidth) << setfill(separator) << name << " | ";
	cout << right << setw(numWidth) << setfill(separator) << time << " | ";
	cout << right << setw(numWidth) << setfill(separator) << time_mean << " | ";
	cout << right << setw(numWidth - 4) << setfill(separator) << (time_stddev * 100.0 / time_mean) << "% | ";
	cout << right << setw(numWidth) << setfill(separator) << (float)nbrOperations / time / 1000 << " | ";
	cout << right << setw(numWidth) << setfill(separator) << (float)nbrBytes / time / 1000 << " | ";
	float total_nbr_cycles = (float)(time * (ticksPerMilliSecond / 1000));
	cout << right << setw(numWidth) << setfill(separator) << ((float)total_nbr_cycles / nbrOperations) << " | ";
	cout << right << setw(numWidth) << setfill(separator) << (float)referenceTime / time << " | ";
	cout << endl;

}
void analyzePerformance(vector<string> names, long long runtimes[NBR_ALGORITHM_VERSIONS][NBR_EXPERIMENTS], long long totalNbrOfOperations, long long totalNbrOfBytes, long long ticksPerMilliSecond, long long referenceTime) {
	print_table_title();
	for (int v = 0; v < names.size(); v++) {
		long long time = minimalValue(runtimes[v], NBR_EXPERIMENTS);
		long long time_mean = meanValue(runtimes[v], NBR_EXPERIMENTS);
		long long time_stddev = stddev(runtimes[v], NBR_EXPERIMENTS);
		print_row(names[v], time, time_mean, time_stddev, totalNbrOfOperations, totalNbrOfBytes, ticksPerMilliSecond, referenceTime);
	}
}
// *** utility functions ***
// we compare the output of the 2 versions and expect no significant differences
void checkIfResultsAreTheSame(string name, float arr3[N][N], float arr4[N][N], int n) {
	int nbr_diff = 0;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			float rel_diff = arr3[i][j] == arr4[i][j] ? 0 : (arr3[i][j] - arr4[i][j]) / (arr3[i][j] == 0 ? arr4[i][j] : arr3[i][j]); // prevent division by zero
			if (rel_diff > 0.0001) {
				nbr_diff++;
				if (PRINT_DATA && i < 10) cout << arr3[i][j] << "<>" << arr4[i][j] << " (" << rel_diff << "), ";
			}
			else {
				if (PRINT_DATA && i < 10) cout << arr3[i][j] << "==" << arr4[i][j] << " (" << rel_diff << "), ";
			}
		}
	}
	if (PRINT_DATA) cout << endl;
	if (nbr_diff > 0)
		cout << "Problem with algorithm " << name << ": " << nbr_diff << " differences with Original Algorithm." << endl;
	else
		cout << "Algorithm "<< name	<<" is OK " <<endl;
}

long long minimalValue(long long* values, int NBR) {
	long long min = values[0];
	for (int i = 1; i < NBR; i++) {
		if (values[i] < min)
			min = values[i];
	}
	return min;
}
long long meanValue(long long* values, int NBR) {
	long long sum = values[0];
	for (int i = 1; i < NBR; i++) {
		sum += values[i];
	}
	return sum / NBR;
}
long long stddev(long long* values, int NBR) {
	long long mean = meanValue(values, NBR);
	long long SSE = 0;
	for (int i = 0; i < NBR; i++) {
		SSE += (values[i] - mean) * (values[i] - mean);
	}
	return sqrt(SSE / NBR);
}


char* getSourceCode() {
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

	// cout << " \n Kernel Source: \n" << sourceCode << "\n" << endl;

	return sourceCode;
}


void initializeOpenCl() {
	// using starterslib
	platform = slectOpenClPlatforms();
	device = selectOpenClDevice(platform);
	context = createContext(device);
	command_queue = createCommandQueue(context, device);

	// Get Kernel Sorce code
	source_code = getSourceCode();

	// Run Program and create the kernel
	program = createProgram(context, device, source_code);
	kernel = createKernel(program, "matrix_multiplication");
}

void setupMatrix() {
	// Set up Matrix Dimentions --> DEFAULT 200 FOR TESTING I CHOOSE 4
	matrixSize = N * N; // 4x4
	// Create matrixes
	A[N][N] = (float*)malloc(matrixSize * sizeof(float));
	B[N][N] = (float*)malloc(matrixSize * sizeof(float));
	C[N][N] = (float*)malloc(matrixSize * sizeof(float));
	// Fill the matrixes


	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			A[i][j] = (float*)rand();
			B[i][j] = (float*)rand();
		}
	}

}

void setupBuffers(float arrA[N][N], float arrB[N][N]) {
	// Allocate mem for Buffers
	bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY, matrixSize * sizeof(float), NULL, NULL);
	bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY, matrixSize * sizeof(float), NULL, NULL);
	bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, matrixSize * sizeof(float), NULL, NULL);

	clEnqueueWriteBuffer(command_queue, bufferA, CL_TRUE, 0, matrixSize * sizeof(float), arrA, 0, NULL, NULL);
	clEnqueueWriteBuffer(command_queue, bufferB, CL_TRUE, 0, matrixSize * sizeof(float), arrB, 0, NULL, NULL);

	

	// Kernel arguments
	clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferA);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferB);
	clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferC);
	clSetKernelArg(kernel, 3, sizeof(int), &N2);
}

void executeKernel(float arrC[N][N]) {

	// Execute the kernel
	clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, globalWorkSize, NULL, 0, NULL, NULL);
	// Retrieve Matrix C form the Device
	clEnqueueReadBuffer(command_queue, bufferC, CL_TRUE, 0, matrixSize * sizeof(float), arrC, 0, NULL, NULL);

}

void printMatrix(float* matrix) {
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			cout << matrix[i * N + j] << "  |  ";
		}
		cout << "\n";
	}
}



void cleanOpenCl() {
	/* Not used anymore
	free(A);
	free(B);
	free(C);
	*/
	clReleaseMemObject(bufferA);
	clReleaseMemObject(bufferB);
	clReleaseMemObject(bufferC);

	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(command_queue);
	clReleaseContext(context);
}

long long runClProgram(float arr5[N][N]) {

	time_point<system_clock> now = system_clock::now();

	executeKernel(arr5);

	time_point<system_clock> epoch = system_clock::now();
	microseconds us = duration_cast<microseconds>(epoch - now);

	return us.count();

}