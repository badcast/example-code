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
        exit(EXIT_FAILURE);
    }
#undef CL_DEF
}

cl_device_id get_best_device(const cl_platform_id platforms[], std::uint32_t num, int selplatform = -1, int seldev = -1)
{
    cl_device_id device, best_device = nullptr;
    // gpu cost for estimate

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
               platforms[x], CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_CPU | CL_DEVICE_TYPE_ACCELERATOR, sizeof(devices), devices, &numdev) !=
           CL_SUCCESS)
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

int main()
{
    // OpenCL::OpenCL
    constexpr int MAX_PLATFORM_ID = 16;
    cl_platform_id platforms[MAX_PLATFORM_ID] {};

    cl_int err, status;
    size_t sz;
    cl_uint x;
    cl_device_id target_device;
    cl_command_queue command;
    char temp_buffer[128];

    // get cl platrforms
    std::uint32_t platforms_n;
    check_cl_error(clGetPlatformIDs(MAX_PLATFORM_ID, platforms, &platforms_n));
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

    // get cl devices
    std::cout << std::endl;
    std::cout << "Fetching device: ";

    if((target_device = get_best_device(platforms, platforms_n)) == nullptr)
    {
        std::cout << "Device not found." << std::endl;
        return 1;
    }

    clGetDeviceInfo(target_device, CL_DEVICE_NAME, sizeof(temp_buffer), temp_buffer, nullptr);
    std::cout << temp_buffer << " (done)" << std::endl;

    std::cout << "VRAM size: ";

    err = clGetDeviceInfo(target_device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(sz), &sz, nullptr);
    check_cl_error(err);

    std::cout << sz / 1024 / 1024 << " MiB" << std::endl;

    // create cl context
    cl_context gpu_context = clCreateContext(nullptr, 1, &target_device, nullptr, nullptr, &err);
    check_cl_error(err);

    // create command queue
    command = clCreateCommandQueue(gpu_context, target_device, 0, &err);
    check_cl_error(err);

    // can exec program on gpu
    std::cout << "Executing adding vectors on GPU: ";
    err = clGetDeviceInfo(target_device, CL_DEVICE_COMPILER_AVAILABLE, sizeof(status), &status, nullptr);
    check_cl_error(err);
    if(status == CL_TRUE)
    {
        std::cout << "supported" << std::endl;

        std::size_t vector_size = 0x1000000;

        std::vector<float> _vector_a(vector_size, 0);
        std::vector<float> _vector_b(vector_size, 0);
        std::vector<float> _vector_c(vector_size, 0);

        for(x = 0; x < vector_size; ++x)
        {
            _vector_a[x] = x + 1;
            _vector_b[x] = x + 1;
        }

        const char *gpu_source = "__kernel void add_vectors(__global float * a, "
                                 "__global float * b, __global float * c) {"
                                 "      int idx = get_global_id(0);"
                                 "      c[idx] = a[idx] + b[idx];"
                                 "}";
        sz = std::strlen(gpu_source);
        cl_program program = clCreateProgramWithSource(gpu_context, 1, &gpu_source, &sz, &err);
        check_cl_error(err);

        err = clBuildProgram(program, 1, &target_device, nullptr, nullptr, nullptr);
        check_cl_error(err);
        do
        {
            check_cl_error(clGetProgramBuildInfo(program, target_device, CL_PROGRAM_BUILD_STATUS, sizeof(status), &status, nullptr));
        } while(status == CL_BUILD_IN_PROGRESS);

        cl_kernel kernel = clCreateKernel(program, "add_vectors", &err);
        check_cl_error(err);

        cl_mem _mem_a, _mem_b, _mem_output;

        _mem_a = clCreateBuffer(gpu_context, CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, vector_size * sizeof(float), _vector_a.data(), &err);
        check_cl_error(err);

        _mem_b = clCreateBuffer(gpu_context, CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, vector_size * sizeof(float), _vector_b.data(), &err);
        check_cl_error(err);

        _mem_output =
            clCreateBuffer(gpu_context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, vector_size * sizeof(float), _vector_c.data(), &err);
        check_cl_error(err);

        err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &_mem_a);
        check_cl_error(err);
        err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &_mem_b);
        check_cl_error(err);
        err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &_mem_output);
        check_cl_error(err);

        size_t group_size;
        err = clGetKernelWorkGroupInfo(kernel, target_device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(group_size), &group_size, nullptr);
        check_cl_error(err);

        // execute
        auto last_time = std::chrono::steady_clock::now();
        err = clEnqueueNDRangeKernel(command, kernel, 1, nullptr, &vector_size, nullptr, 0, nullptr, nullptr);

        auto new_time = std::chrono::steady_clock::now();
        auto ellapsed = std::chrono::duration_cast<std::chrono::microseconds>(new_time - last_time);

        std::cout << "Execution delay: " << ellapsed.count() << " mcs" << std::endl;

        check_cl_error(err);

        // Wait finising queue command's
        clFinish(command);

        // test reuslt
        err = clEnqueueReadBuffer(command, _mem_output, CL_TRUE, 0, vector_size * sizeof(float), _vector_c.data(), 0, 0, 0);
        check_cl_error(err);

        status = 0;
        for(x = 0; x < vector_size; ++x)
        {
            if(_vector_a[x] + _vector_b[x] != _vector_c[x])
            {
                ++status;
            }
        }

        std::cout << "Execution complete. Errors: " << status << std::endl;

        // release
        clReleaseMemObject(_mem_a);
        clReleaseMemObject(_mem_b);
        clReleaseMemObject(_mem_output);

        clReleaseKernel(kernel);
    }
    else
    {
        std::cout << "is not supported (skip)" << std::endl;
    }
    // create cl buffer (GPU Memory)
    constexpr auto mem_size = 1024 * 1024;
    cl_mem mem_gpu = clCreateBuffer(gpu_context, CL_MEM_READ_WRITE, mem_size, nullptr, &err);
    check_cl_error(err);
    std::cout << std::endl << "VRAM allocated size: " << mem_size / 1024 << " KiB" << std::endl;

    strcpy(temp_buffer, getenv("USER"));
    std::cout << "COMPUTER -> GPU: \"" << temp_buffer << "\" ... ";
    check_cl_error(clEnqueueWriteBuffer(command, mem_gpu, CL_TRUE, 0, sz = strlen(temp_buffer), temp_buffer, 0, nullptr, nullptr));
    check_cl_error(clFinish(command));

    // clear buffer
    memset(temp_buffer, 0, sizeof(temp_buffer));
    std::cout << "your name writed to GPU" << std::endl << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "So, read your name from GPU" << std::endl;
    std::cout << "COMPUTER <- GPU: ";

    check_cl_error(clEnqueueReadBuffer(command, mem_gpu, CL_TRUE, 0, sz, temp_buffer, 0, 0, 0));

    std::cout << '"' << temp_buffer << '"' << std::endl << std::endl;
    check_cl_error(clFinish(command));

    check_cl_error(clReleaseMemObject(mem_gpu));

    // Out information
    check_cl_error(clReleaseContext(gpu_context));

    std::cout << "Program execution finished. All resources is released." << std::endl;
    return 0;
}
