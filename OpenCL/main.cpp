
#include "DES.h"
#include <string>
#include <iostream>
#include <fstream>

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl.h>

using namespace std;


const int IV[64] = { 1 };


int main(int argc, char *argv[]) {
	
	string type(argv[1]);
	string inputFile(argv[2]);

	//string type = "d";
	//string inputFile = "encryption_result.txt";
	
	char *input, *result;
	int length;
	FILE* fp;
	fopen_s(&fp, inputFile.c_str(), "r");
	fseek(fp, 0, SEEK_END);
	int n = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	int rem = n % 8;
	if (rem) {
		length = n + 8 - rem;
		input = new char[length];
		for (int i = n; i < length; i++) {
			input[i] = 0;
		}
	}
	else {
		length = n;
		input = new char[length];
	}
	fread(input, n, 1, fp);
	fclose(fp);
	result = new char[length];


	int subKeys[17][48];
	if (type == "e") {
		generateSubKeys(subKeys, true);
	}
	else {
		generateSubKeys(subKeys, false);
	}



	std::fstream kernelFile("des.cl");
	std::string content(
		(std::istreambuf_iterator<char>(kernelFile)),
		std::istreambuf_iterator<char>()
	);

	size_t kernelSize = content.size();
	const char* source_str = new char[kernelSize];
	source_str = content.c_str();

	// Get platform and device information
	cl_platform_id platform_id = NULL;
	cl_device_id device_id = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;

	cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id,
		&ret_num_devices);


	// Create an OpenCL context
	cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL,
		&ret);

	cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);


	cl_mem inputMemObject = clCreateBuffer(context,
		CL_MEM_READ_ONLY, length * sizeof(char), NULL, &ret);

	cl_mem subKeysMemObject = clCreateBuffer(context,
		CL_MEM_READ_ONLY, 17*48 * sizeof(int), NULL, &ret);

	cl_mem outputMemObject = clCreateBuffer(context,
		CL_MEM_WRITE_ONLY, length * sizeof(char), NULL, &ret);
	


	ret = clEnqueueWriteBuffer(command_queue, inputMemObject, CL_TRUE, 0, length * sizeof(char), input, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, subKeysMemObject, CL_TRUE,0, 17*48 * sizeof(int), subKeys, 0, NULL, NULL);


	// Create a program from the kernel source
	cl_program program = clCreateProgramWithSource(context, 1, (const
		char**)&source_str, (const size_t *)&kernelSize, &ret);


	// Build the program
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	if (ret == CL_BUILD_PROGRAM_FAILURE) {
		// Determine the size of the log
		size_t log_size;
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

		// Allocate memory for the log
		char *log = (char *)malloc(log_size);

		// Get the log
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

		// Print the log
		cout << log;
	}


	// Create the OpenCL kernel object
	cl_kernel kernel = clCreateKernel(program, "execute", &ret);

	// Set the arguments of the kernel
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&inputMemObject);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&subKeysMemObject);
	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&outputMemObject);

	// Execute the OpenCL kernel on the array
	size_t global_item_size = length / 8;
	size_t local_item_size = 1;

	//Execute the kernel on the device
	cl_event event;
	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1,
		NULL, &global_item_size, &local_item_size, 0, NULL, &event);

	ret = clFinish(command_queue);


	char *output = new char[length];

	ret = clEnqueueReadBuffer(command_queue, outputMemObject, CL_TRUE, 0, length * sizeof(char), output, 0, NULL, NULL);

	if (type != "e") {
		while (output[length - 1] == 0) {
			length--;
		}
	}

	string filename;
	string extension = inputFile.substr(inputFile.find_last_of('.'));
	if (type == "e") {
		filename = "files/encryption_result" + extension;
	}
	else {
		filename = "files/decryption_result" + extension;
	}
	fp;
	fopen_s(&fp, filename.c_str(), "w");
	fwrite(output, length, 1, fp);
	fclose(fp);


	ret = clFlush(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);

	ret = clReleaseMemObject(inputMemObject);
	ret = clReleaseMemObject(subKeysMemObject);
	ret = clReleaseMemObject(outputMemObject);

	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);
	delete input;
	delete output;


	return 0;
}

