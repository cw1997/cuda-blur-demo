#include "blur.h"
#include <cstdio>
#include <cstdlib>
#define CL_TARGET_OPENCL_VERSION 120
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

// OpenCL kernel source for box blur with shared local memory tiling.
// Architecture mirrors the CUDA kernel: cooperative tile load + local memory averaging.
static const char* kernelSource =
"__kernel void blurKernel(__global const uchar* input, __global uchar* output,\n"
"                         int width, int height, int channels, int radius,\n"
"                         __local uchar* shared) {\n"
"    int tileW = get_local_size(0) + 2 * radius;\n"
"    int tileH = get_local_size(1) + 2 * radius;\n"
"\n"
"    int totalPixels = tileW * tileH;\n"
"    int threads = get_local_size(0) * get_local_size(1);\n"
"    int tid = get_local_id(1) * get_local_size(0) + get_local_id(0);\n"
"\n"
"    for (int i = tid; i < totalPixels; i += threads) {\n"
"        int sx = i % tileW;\n"
"        int sy = i / tileW;\n"
"\n"
"        int gx = get_group_id(0) * get_local_size(0) + sx - radius;\n"
"        int gy = get_group_id(1) * get_local_size(1) + sy - radius;\n"
"\n"
"        gx = clamp(gx, 0, width - 1);\n"
"        gy = clamp(gy, 0, height - 1);\n"
"\n"
"        int srcIdx = (gy * width + gx) * channels;\n"
"        int dstIdx = (sy * tileW + sx) * channels;\n"
"\n"
"        shared[dstIdx]     = input[srcIdx];\n"
"        shared[dstIdx + 1] = input[srcIdx + 1];\n"
"        shared[dstIdx + 2] = input[srcIdx + 2];\n"
"    }\n"
"\n"
"    barrier(CLK_LOCAL_MEM_FENCE);\n"
"\n"
"    int x = get_group_id(0) * get_local_size(0) + get_local_id(0);\n"
"    int y = get_group_id(1) * get_local_size(1) + get_local_id(1);\n"
"\n"
"    if (x < width && y < height) {\n"
"        int sumR = 0, sumG = 0, sumB = 0;\n"
"        int sx = get_local_id(0) + radius;\n"
"        int sy = get_local_id(1) + radius;\n"
"\n"
"        for (int dy = -radius; dy <= radius; dy++) {\n"
"            for (int dx = -radius; dx <= radius; dx++) {\n"
"                int idx = ((sy + dy) * tileW + (sx + dx)) * channels;\n"
"                sumR += shared[idx];\n"
"                sumG += shared[idx + 1];\n"
"                sumB += shared[idx + 2];\n"
"            }\n"
"        }\n"
"\n"
"        int count = (2 * radius + 1) * (2 * radius + 1);\n"
"        int dstIdx = (y * width + x) * channels;\n"
"        output[dstIdx]     = (uchar)(sumR / count);\n"
"        output[dstIdx + 1] = (uchar)(sumG / count);\n"
"        output[dstIdx + 2] = (uchar)(sumB / count);\n"
"    }\n"
"}\n";

// Check OpenCL error code and exit on failure
static void checkError(cl_int err, const char* msg) {
    if (err != CL_SUCCESS) {
        fprintf(stderr, "OpenCL error %d: %s\n", err, msg);
        exit(1);
    }
}

// OpenCL box blur host wrapper
// Initializes OpenCL (GPU), builds the kernel, launches it with profiling
double blurImageOpenCL(const uint8_t* input, uint8_t* output, int width, int height, int kernelSize) {
    int radius = kernelSize / 2;
    int channels = 3;
    int dataSize = width * height * channels * sizeof(uint8_t);

    cl_int err;

    // Select OpenCL platform and GPU device
    cl_platform_id platform;
    cl_uint numPlatforms;
    err = clGetPlatformIDs(1, &platform, &numPlatforms);
    checkError(err, "clGetPlatformIDs");

    cl_device_id device;
    cl_uint numDevices;
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, &numDevices);
    checkError(err, "clGetDeviceIDs (GPU)");

    // Create context and command queue with profiling enabled
    cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
    checkError(err, "clCreateContext");

    cl_command_queue_properties props = CL_QUEUE_PROFILING_ENABLE;
    cl_command_queue queue = clCreateCommandQueue(context, device, props, &err);
    checkError(err, "clCreateCommandQueue");

    // Build the kernel program from embedded source
    cl_program program = clCreateProgramWithSource(context, 1, &kernelSource, nullptr, &err);
    checkError(err, "clCreateProgramWithSource");

    err = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        size_t logSize;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
        char* log = (char*)malloc(logSize);
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, log, nullptr);
        fprintf(stderr, "Kernel build error:\n%s\n", log);
        free(log);
        exit(1);
    }

    cl_kernel kernel = clCreateKernel(program, "blurKernel", &err);
    checkError(err, "clCreateKernel");

    // Allocate device buffers and upload input
    cl_mem d_input = clCreateBuffer(context, CL_MEM_READ_ONLY, dataSize, nullptr, &err);
    checkError(err, "clCreateBuffer (input)");

    cl_mem d_output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, dataSize, nullptr, &err);
    checkError(err, "clCreateBuffer (output)");

    err = clEnqueueWriteBuffer(queue, d_input, CL_TRUE, 0, dataSize, input, 0, nullptr, nullptr);
    checkError(err, "clEnqueueWriteBuffer");

    // Launch configuration: 16x16 work-group, global size rounded to work-group multiple
    size_t local[2] = { 16, 16 };
    size_t global[2] = {
        ((size_t)width + local[0] - 1) / local[0] * local[0],
        ((size_t)height + local[1] - 1) / local[1] * local[1]
    };

    size_t tileW = local[0] + 2 * radius;
    size_t tileH = local[1] + 2 * radius;
    size_t localMemSize = tileW * tileH * channels * sizeof(cl_uchar);

    // Set kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_input);
    checkError(err, "clSetKernelArg 0");
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_output);
    checkError(err, "clSetKernelArg 1");
    err = clSetKernelArg(kernel, 2, sizeof(int), &width);
    checkError(err, "clSetKernelArg 2");
    err = clSetKernelArg(kernel, 3, sizeof(int), &height);
    checkError(err, "clSetKernelArg 3");
    err = clSetKernelArg(kernel, 4, sizeof(int), &channels);
    checkError(err, "clSetKernelArg 4");
    err = clSetKernelArg(kernel, 5, sizeof(int), &radius);
    checkError(err, "clSetKernelArg 5");
    err = clSetKernelArg(kernel, 6, localMemSize, nullptr);
    checkError(err, "clSetKernelArg 6 (__local)");

    // Enqueue kernel and wait for completion
    cl_event event;
    err = clEnqueueNDRangeKernel(queue, kernel, 2, nullptr, global, local, 0, nullptr, &event);
    checkError(err, "clEnqueueNDRangeKernel");

    err = clWaitForEvents(1, &event);
    checkError(err, "clWaitForEvents");

    // Measure kernel execution time via OpenCL event profiling
    cl_ulong startTime, endTime;
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &startTime, nullptr);
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &endTime, nullptr);
    double ms = (double)(endTime - startTime) / 1000000.0;

    // Download result
    err = clEnqueueReadBuffer(queue, d_output, CL_TRUE, 0, dataSize, output, 0, nullptr, nullptr);
    checkError(err, "clEnqueueReadBuffer");

    // Cleanup
    clReleaseEvent(event);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseMemObject(d_input);
    clReleaseMemObject(d_output);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    return ms;
}
