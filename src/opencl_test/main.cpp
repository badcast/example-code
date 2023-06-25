#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <cmath>
#include <CL/cl.h>

using namespace std;

struct cl_info_data {
    char name[64];
    char vendor[64];
    int version;
};

int main()
{
    // OpenCL::OpenCL
    constexpr int MAX_PLATFORM_ID = 16;
    constexpr int MAX_DEVICE_ID = 16;
    cl_platform_id platforms[MAX_PLATFORM_ID] {};
    std::vector<cl_info_data> platform_info;
    std::vector<cl_info_data> device_info;
    cl_info_data __temp_info;
    cl_uint x;
    cl_int err;

    // get cl platrforms
    std::uint32_t platforms_n;
    clGetPlatformIDs(MAX_PLATFORM_ID, platforms, &platforms_n);

    for (x = 0; x < platforms_n; ++x) {
        clGetPlatformInfo(platforms[x], CL_PLATFORM_NAME, sizeof(__temp_info.name), __temp_info.name, nullptr);
        clGetPlatformInfo(platforms[x], CL_PLATFORM_VENDOR, sizeof(__temp_info.vendor), __temp_info.vendor, nullptr);
        clGetPlatformInfo(platforms[x], CL_PLATFORM_VERSION, sizeof(__temp_info.version), &__temp_info.version, nullptr);
        platform_info.emplace_back(__temp_info);
    }

    // get cl devices
    cl_device_id devices[MAX_DEVICE_ID];
    cl_uint devices_n;
    clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, MAX_DEVICE_ID, devices, &devices_n);
    for (x = 0; x < devices_n; ++x) {
        clGetDeviceInfo(devices[x], CL_DEVICE_NAME, sizeof(__temp_info.name), __temp_info.name, nullptr);
        clGetDeviceInfo(devices[x], CL_DEVICE_VENDOR, sizeof(__temp_info.vendor), __temp_info.vendor, nullptr);
        clGetDeviceInfo(devices[x], CL_DEVICE_VERSION, sizeof(__temp_info.version), &__temp_info.version, nullptr);
        device_info.emplace_back(__temp_info);
    }
    // create cl context
    cl_context context = clCreateContext(nullptr, 1, devices, nullptr, nullptr, &err);
    if (err) {
        cout << "Failed create CL Context" << endl;
        return -1;
    }

    // create command queue
    cl_command_queue command = clCreateCommandQueueWithProperties(context, devices[0], nullptr, &err);
    if (err) {
        cout << "Failed create CL command queue" << endl;
        return -1;
    }

    // create cl buffer (GPU Memory)
    constexpr auto mem_size = 1000000000; // 1 GiB
    cl_mem pgpu_mem = clCreateBuffer(context, CL_MEM_READ_WRITE, mem_size, nullptr, &err);
    if (err) {
        cout << "Failed create cl mem object" << endl;
        return -1;
    }

    // make cl mem
    constexpr char text_to_gpu_mem[] = "Hello. I'm text for GPU, and my site is GPU Cell";

    cout << "Write data to GPU (pattern): (" << text_to_gpu_mem << ") ... ";
    err = clEnqueueWriteBuffer(command, pgpu_mem, CL_TRUE, 0, sizeof(text_to_gpu_mem), text_to_gpu_mem, 0, nullptr, nullptr);
    if (err) {
        cout << endl << "Failed cl write" << endl;
        return -1;
    }
    cout << "complete" << endl;

    cout << "Starting read from GPU: ";
    char text_from_gpu_mem[64];
    err = clEnqueueReadBuffer(command, pgpu_mem, CL_TRUE, 0, sizeof text_from_gpu_mem, text_from_gpu_mem, 0, 0, 0);
    if (err) {
        cout << endl << "Failed cl read" << endl;
        return -1;
    }
    cout << text_from_gpu_mem << endl;

    clReleaseMemObject(pgpu_mem);
    clFlush(command);

    // Out information

    cout << "OpenCL platforms: " << platforms_n << endl;
    for (x = 0; x < platforms_n; ++x) {
        cout << x << ':' << endl;
        cout << "\tName: " << platform_info[x].name << endl;
        cout << "\tVendor: " << platform_info[x].vendor << endl;
    }

    for (x = 0; x < devices_n; ++x)
        clReleaseDevice(devices[x]);

    clReleaseContext(context);
    return 0;
}
