#include <CL/cl.h>

#define MAX_SOURCE_SIZE 0x1000000
#define CL_USE_DEPRECATE_OPENCL_1_2_APIS


using namesppce std;

int pie()
{
	int n;
	cout<< "enter the no of intervals";
	scanf_s("%d", &n);
	double X = 1.0 / n; 
	
	double *Z = (double*)malloc(sizeof(double)*n);


	// Load the kernel source code into the array source_str
	FILE *fp;
	char *source_str;
	size_t source_size;

	errno_t err =  fopen_s(&fp, "pie.cl", "r");
	if (err)
	{
		fprintf(stderr, "Failed to load kernel.\n");
		getchar();
		exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);
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
	// Create a command queue
	cl_command_queue command_queue = clCreateCommandQueueWithProperties(context,
		device_id,
		NULL, &ret);

	// Create memory buffers on the device for each vector A, B and C
	cl_mem a_mem_obj = clCreateBuffer(context,
		CL_MEM_WRITE_ONLY, n * sizeof(double), NULL, &ret);
	
	//cl_mem c_mem_obj = clCreateBuffer(context,
	//CL_MEM_WRITE_ONLY, LIST_SIZE * sizeof(int), NULL, &ret);
	// Copy the lists A and B to their respective memory buffers

	//ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0, sizeof(int), B, 0, NULL, NULL);
	//ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE,
	//0, LIST_SIZE * sizeof(int), B, 0, NULL, NULL);
	// Create a program from the kernel source
	cl_program program = clCreateProgramWithSource(context, 1, (const
		char**)&source_str, (const size_t *)&source_size, &ret);

	// Build the program
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	// Create the OpenCL kernel object
	cl_kernel kernel = clCreateKernel(program, "pie", &ret);
	// Set the arguments of the kernel
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&a_mem_obj);
	ret = clSetKernelArg(kernel, 1, sizeof(double), &X);

	// Execute the OpenCL kernel on the array
	size_t global_item_size = n;
	size_t local_item_size = 1;
	//Execute the kernel on the device
	cl_event event;
	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1,
		NULL, &global_item_size, &local_item_size, 0, NULL, &event);
	ret = clFinish(command_queue);

	ret = clEnqueueReadBuffer(command_queue, a_mem_obj, CL_TRUE, 0, n* sizeof(double), Z, 0, NULL, NULL);
	// Display the result to the screen
	double Sum = 0.0;
	for (int i = 0; i < n; i++)
		Sum += Z[i];
	printf("The value of pi %f", Sum);

	ret = clFlush(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);

	ret = clReleaseMemObject(a_mem_obj);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);
	free(Z);

	return 0;
}