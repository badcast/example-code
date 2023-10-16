#include <algorithm>
#include <array>
#include <CL/cl.h>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

constexpr auto max_cost_vendors = 3;
constexpr auto max_cost_value = 3u;
constexpr std::tuple<const char *, std::uint8_t> cost_vendors[max_cost_vendors] {
    {"nvidia corporation", max_cost_value}, {"amd", 2u}, {"intel", 1u}};

constexpr int MAX_PLATFORM_ID = 16;
cl_platform_id platforms[MAX_PLATFORM_ID] {};

class CLObject
{
private:
    cl_device_id target_device;
    cl_kernel __kadd, __ksub, __kmul, __kdiv;
    cl_context gpu_context;
    cl_command_queue command;

    void compile_kernels();

    void koperation(cl_kernel operation, const int &lhs, const int &rhs, int *result);

public:
    CLObject() : target_device(nullptr)
    {
    }

    ~CLObject()
    {
        close();
    }

    bool open(int platformId = -1, int deviceId = -1);

    bool supportCompiller();

    void showDevices();

    void close();

    int add(int lhs, int rhs);
    int sub(int lhs, int rhs);
    int mul(int lhs, int rhs);
    int div(int lhs, int rhs);

    static cl_device_id get_best_device(const cl_platform_id platforms[], std::uint32_t num, int selplatform = -1, int seldev = -1);
};

int main()
{
    CLObject cl;

    cl.showDevices();

    cl.open();

    int result = cl.add(100, 100);
    result = cl.sub(-1, 1000);

    while(1)
        result = cl.mul(result, 3);

    return 0;
}

void check_cl_error(cl_int errcode);

bool CLObject::open(int platformId, int deviceId)
{
    std::uint32_t platforms_n;
    cl_int err, status;
    size_t sz;
    cl_uint x;

    char temp_buffer[128];

    // get cl devices
    std::cout << "Fetching device: ";

    clGetPlatformIDs(MAX_PLATFORM_ID, platforms, &platforms_n);

    // Get The Best Device
    if((target_device = CLObject::get_best_device(platforms, platforms_n, platformId, deviceId)) == nullptr)
    {
        std::cout << "Device not found." << std::endl;
        return false;
    }

    clGetDeviceInfo(target_device, CL_DEVICE_NAME, sizeof(temp_buffer), temp_buffer, nullptr);
    std::cout << temp_buffer << " (done)" << std::endl;

    std::cout << "VRAM size: ";

    err = clGetDeviceInfo(target_device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(sz), &sz, nullptr);
    check_cl_error(err);

    std::cout << sz / 1024 / 1024 << " MiB" << std::endl;

    // create cl context
    gpu_context = clCreateContext(nullptr, 1, &target_device, nullptr, nullptr, &err);
    check_cl_error(err);

    // create command queue
    command = clCreateCommandQueue(gpu_context, target_device, 0, &err);
    check_cl_error(err);

    compile_kernels();

    return true;
}

bool CLObject::supportCompiller()
{
    cl_int status, ret = clGetDeviceInfo(target_device, CL_DEVICE_COMPILER_AVAILABLE, sizeof(status), &status, nullptr);

    return ret == CL_SUCCESS && status == CL_TRUE;
}

void CLObject::compile_kernels()
{
    cl_int err, status;
    size_t sz;

    if(!supportCompiller())
        return;
    const char *source_add = "__kernel void kernel_add(__global int * a, "
                             "__global int * b, __global int * c) {"
                             "      int idx = get_global_id(0);"
                             "      c[idx] = a[idx] + b[idx];"
                             "}"
                             "__kernel void kernel_sub(__global int * a, "
                             "__global int * b, __global int * c) {"
                             "      int idx = get_global_id(0);"
                             "      c[idx] = a[idx] - b[idx];"
                             "}"
                             "__kernel void kernel_mul(__global int * a, "
                             "__global int * b, __global int * c) {"
                             "      int idx = get_global_id(0);"
                             "      c[idx] = a[idx] * b[idx];"
                             "}"
                             "__kernel void kernel_div(__global int * a, "
                             "__global int * b, __global int * c) {"
                             "      int idx = get_global_id(0);"
                             "      c[idx] = a[idx] / b[idx];"
                             "}";

    sz = std::strlen(source_add);
    cl_program program = clCreateProgramWithSource(gpu_context, 1, &source_add, &sz, &err);

    clBuildProgram(program, 1, &target_device, nullptr, nullptr, nullptr);

    do
    {
        clGetProgramBuildInfo(program, target_device, CL_PROGRAM_BUILD_STATUS, sizeof(status), &status, nullptr);
    } while(status == CL_BUILD_IN_PROGRESS);

    __kadd = clCreateKernel(program, "kernel_add", &err);
    check_cl_error(err);
    __ksub = clCreateKernel(program, "kernel_sub", &err);
    check_cl_error(err);
    __kmul = clCreateKernel(program, "kernel_mul", &err);
    check_cl_error(err);
    __kdiv = clCreateKernel(program, "kernel_div", &err);
    check_cl_error(err);
}

void CLObject::close()
{
    if(target_device == nullptr)
        return;

    clReleaseKernel(__kadd);
    clReleaseKernel(__ksub);

    std::cout << "Program execution finished. All resources is released." << std::endl;
}

void CLObject::koperation(cl_kernel operation, const int &lhs, const int &rhs, int *result)
{
    if(target_device == nullptr || !supportCompiller())
    {
        return;
    }

    cl_int err;
    cl_mem _mem_a, _mem_b, _mem_output;

    _mem_a = clCreateBuffer(gpu_context, CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, sizeof(lhs), const_cast<int *>(&lhs), &err);
    check_cl_error(err);

    _mem_b = clCreateBuffer(gpu_context, CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, sizeof(rhs), const_cast<int *>(&rhs), &err);
    check_cl_error(err);

    _mem_output = clCreateBuffer(gpu_context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, sizeof(*result), result, &err);
    check_cl_error(err);

    err = clSetKernelArg(operation, 0, sizeof(cl_mem), &_mem_a);
    check_cl_error(err);
    err = clSetKernelArg(operation, 1, sizeof(cl_mem), &_mem_b);
    check_cl_error(err);
    err = clSetKernelArg(operation, 2, sizeof(cl_mem), &_mem_output);
    check_cl_error(err);

    size_t group_size;
    err = clGetKernelWorkGroupInfo(operation, target_device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(group_size), &group_size, nullptr);
    check_cl_error(err);

    size_t __size = 1;
    err = clEnqueueNDRangeKernel(command, operation, 1, nullptr, &__size, nullptr, 0, nullptr, nullptr);

    // Wait finising queue command's
    clFinish(command);

    // test reuslt
    err = clEnqueueReadBuffer(command, _mem_output, CL_TRUE, 0, sizeof(*result), result, 0, 0, 0);
    check_cl_error(err);

    // release
    clReleaseMemObject(_mem_a);
    clReleaseMemObject(_mem_b);
    clReleaseMemObject(_mem_output);
}

int CLObject::add(int lhs, int rhs)
{
    int result;
    koperation(__kadd, lhs, rhs, &result);
    return result;
}

int CLObject::sub(int lhs, int rhs)
{
    int result;
    koperation(__ksub, lhs, rhs, &result);
    return result;
}

int CLObject::mul(int lhs, int rhs)
{
    int result;
    koperation(__kmul, lhs, rhs, &result);
    return result;
}

int CLObject::div(int lhs, int rhs)
{
    int result;
    koperation(__kdiv, lhs, rhs, &result);
    return result;
}

void CLObject::showDevices()
{
    // get cl platrforms
    std::uint32_t platforms_n, x;
    char temp_buffer[128];

    clGetPlatformIDs(MAX_PLATFORM_ID, platforms, &platforms_n);
    std::cout << "OpenCL platforms: " << platforms_n << std::endl;
    for(x = 0; x < platforms_n; ++x)
    {
        std::cout << x << ':' << std::endl;
        check_cl_error(clGetPlatformInfo(platforms[x], CL_PLATFORM_NAME, sizeof(temp_buffer), temp_buffer, nullptr));
        std::cout << "\tName: " << temp_buffer << std::endl;
        check_cl_error(clGetPlatformInfo(platforms[x], CL_PLATFORM_VENDOR, sizeof(temp_buffer), temp_buffer, nullptr));
        std::cout << "\tVendor: " << temp_buffer << std::endl;
        check_cl_error(clGetPlatformInfo(platforms[x], CL_PLATFORM_VERSION, sizeof(temp_buffer), temp_buffer, nullptr));
        std::cout << "\tVersion: " << temp_buffer << std::endl;
    }
}

cl_device_id CLObject::get_best_device(const cl_platform_id platforms[], std::uint32_t num, int selplatform, int seldev)
{
    // gpu cost for estimate
    cl_device_id device, best_device = nullptr;

    cl_device_id devices[8];
    cl_uint numdev;
    std::string vendor_str;
    size_t bufSize, bufSize2nd;
    int best_cost, self_cost, x, y, z;

    if(selplatform == -1 || selplatform >= num)
        selplatform = 0;
    else
        num = selplatform + 1; // select only platform

    for(x = selplatform, best_cost = -1; x < num; ++x)
    {
        // get devices for platform
        if(clGetDeviceIDs(
               platforms[x],
               CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_CPU | CL_DEVICE_TYPE_ACCELERATOR,
               sizeof(devices) / sizeof(devices[0]),
               devices,
               &numdev) != CL_SUCCESS)
            continue;

        if(seldev != -1 || seldev >= numdev)
        {
            y = 0;
        }
        else
        {
            y = seldev;
            numdev = seldev + 1; // set only device
        }
        for(; y < numdev; ++y)
        {
            device = devices[y];

            // calc cost for gpu
            // get string size
            if(clGetDeviceInfo(device, CL_DEVICE_VENDOR, 0, nullptr, &bufSize) != CL_SUCCESS)
                continue; // if error than skip

            vendor_str.resize(bufSize, '\0');
            clGetDeviceInfo(device, CL_DEVICE_VENDOR, bufSize, const_cast<char *>(vendor_str.data()), &bufSize2nd);
            std::transform(
                std::begin(vendor_str), std::end(vendor_str), std::begin(vendor_str), [](unsigned char c) { return std::tolower(c); });
            if(bufSize != bufSize2nd)
                std::cout << "Warn: size dynamically changed on the device "
                             "(get_vendor_string_size) - "
                          << vendor_str << std::endl;

            // get cost
            self_cost = 0; // set default cost for unknown device
            for(z = 0; z < max_cost_vendors; ++z)
            {
                if(!std::strcmp(vendor_str.data(), std::get<0>(cost_vendors[z])))
                {
                    self_cost = std::get<1>(cost_vendors[z]);
                    break;
                }
            }

            if(self_cost > best_cost)
            {
                best_cost = self_cost; // get the best
                std::swap(best_device, device);
            }

            // release unused device
            if(device)
                clReleaseDevice(device);
        }
    }

    return best_device;
}

void check_cl_error(cl_int errcode)
{
#define CL_DEF(CODE)                                                    \
    case CODE:                                                          \
        std::cout << std::endl << "CL Error: " << (#CODE) << std::endl; \
        break
    switch(errcode)
    {
        /* CL_DEF(CL_SUCCESS); */
        CL_DEF(CL_DEVICE_NOT_FOUND);
        CL_DEF(CL_DEVICE_NOT_AVAILABLE);
        CL_DEF(CL_COMPILER_NOT_AVAILABLE);
        CL_DEF(CL_MEM_OBJECT_ALLOCATION_FAILURE);
        CL_DEF(CL_OUT_OF_RESOURCES);
        CL_DEF(CL_OUT_OF_HOST_MEMORY);
        CL_DEF(CL_PROFILING_INFO_NOT_AVAILABLE);
        CL_DEF(CL_MEM_COPY_OVERLAP);
        CL_DEF(CL_IMAGE_FORMAT_MISMATCH);
        CL_DEF(CL_IMAGE_FORMAT_NOT_SUPPORTED);
        CL_DEF(CL_BUILD_PROGRAM_FAILURE);
        CL_DEF(CL_MAP_FAILURE);
        CL_DEF(CL_MISALIGNED_SUB_BUFFER_OFFSET);
        CL_DEF(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
        CL_DEF(CL_COMPILE_PROGRAM_FAILURE);
        CL_DEF(CL_LINKER_NOT_AVAILABLE);
        CL_DEF(CL_LINK_PROGRAM_FAILURE);
        CL_DEF(CL_DEVICE_PARTITION_FAILED);
        CL_DEF(CL_KERNEL_ARG_INFO_NOT_AVAILABLE);
        CL_DEF(CL_INVALID_VALUE);
        CL_DEF(CL_INVALID_DEVICE_TYPE);
        CL_DEF(CL_INVALID_PLATFORM);
        CL_DEF(CL_INVALID_DEVICE);
        CL_DEF(CL_INVALID_CONTEXT);
        CL_DEF(CL_INVALID_QUEUE_PROPERTIES);
        CL_DEF(CL_INVALID_COMMAND_QUEUE);
        CL_DEF(CL_INVALID_HOST_PTR);
        CL_DEF(CL_INVALID_MEM_OBJECT);
        CL_DEF(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
        CL_DEF(CL_INVALID_IMAGE_SIZE);
        CL_DEF(CL_INVALID_SAMPLER);
        CL_DEF(CL_INVALID_BINARY);
        CL_DEF(CL_INVALID_BUILD_OPTIONS);
        CL_DEF(CL_INVALID_PROGRAM);
        CL_DEF(CL_INVALID_PROGRAM_EXECUTABLE);
        CL_DEF(CL_INVALID_KERNEL_NAME);
        CL_DEF(CL_INVALID_KERNEL_DEFINITION);
        CL_DEF(CL_INVALID_KERNEL);
        CL_DEF(CL_INVALID_ARG_INDEX);
        CL_DEF(CL_INVALID_ARG_VALUE);
        CL_DEF(CL_INVALID_ARG_SIZE);
        CL_DEF(CL_INVALID_KERNEL_ARGS);
        CL_DEF(CL_INVALID_WORK_DIMENSION);
        CL_DEF(CL_INVALID_WORK_GROUP_SIZE);
        CL_DEF(CL_INVALID_WORK_ITEM_SIZE);
        CL_DEF(CL_INVALID_GLOBAL_OFFSET);
        CL_DEF(CL_INVALID_EVENT_WAIT_LIST);
        CL_DEF(CL_INVALID_EVENT);
        CL_DEF(CL_INVALID_OPERATION);
        CL_DEF(CL_INVALID_GL_OBJECT);
        CL_DEF(CL_INVALID_BUFFER_SIZE);
        CL_DEF(CL_INVALID_MIP_LEVEL);
        CL_DEF(CL_INVALID_GLOBAL_WORK_SIZE);
        CL_DEF(CL_INVALID_PROPERTY);
        CL_DEF(CL_INVALID_IMAGE_DESCRIPTOR);
        CL_DEF(CL_INVALID_COMPILER_OPTIONS);
        CL_DEF(CL_INVALID_LINKER_OPTIONS);
        CL_DEF(CL_INVALID_DEVICE_PARTITION_COUNT);
        CL_DEF(CL_INVALID_PIPE_SIZE);
        CL_DEF(CL_INVALID_DEVICE_QUEUE);
        CL_DEF(CL_INVALID_SPEC_ID);
        CL_DEF(CL_MAX_SIZE_RESTRICTION_EXCEEDED);
        default:
            std::cout << "UNKNOWN_CL_ERROR: " << errcode << std::endl;
        case CL_SUCCESS:
            break;
    }
    if(errcode)
    {
        // exit(EXIT_FAILURE);
    }
#undef CL_DEF
}
