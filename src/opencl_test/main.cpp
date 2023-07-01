#include <iostream>
#include <cstdint>
#include <vector>
#include <cstring>
#include <string>
#include <cmath>
#include <chrono>
#include <thread>
#include <CL/cl.h>

using namespace std;

struct cl_info_data {
    char name[64];
    char vendor[64];
    char version[38];
};

void check_cl_error(cl_int errcode)
{

#define CL_DEF(CODE)                                                    \
    case CODE:                                                          \
        std::cout << std::endl << "CL Error: " << (#CODE) << std::endl; \
        break

    switch (errcode) {
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
    default:
        break;
    }
    if (errcode) {
        exit(EXIT_FAILURE);
    }
#undef CL_DEF
}

void printinfo(cl_info_data& info)
{
    cout << "\tName: " << info.name << endl;
    cout << "\tVendor: " << info.vendor << endl;
    cout << "\tVersion: " << info.version << endl;
}

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
    check_cl_error(clGetPlatformIDs(MAX_PLATFORM_ID, platforms, &platforms_n));

    for (x = 0; x < platforms_n; ++x) {
        check_cl_error(clGetPlatformInfo(platforms[x], CL_PLATFORM_NAME, sizeof(__temp_info.name), __temp_info.name, nullptr));
        check_cl_error(clGetPlatformInfo(platforms[x], CL_PLATFORM_VENDOR, sizeof(__temp_info.vendor), __temp_info.vendor, nullptr));
        check_cl_error(clGetPlatformInfo(platforms[x], CL_PLATFORM_VERSION, sizeof(__temp_info.version), &__temp_info.version, nullptr));
        platform_info.emplace_back(__temp_info);
    }

    cout << "OpenCL platforms: " << platforms_n << endl;
    for (x = 0; x < platforms_n; ++x) {
        cout << x << ':' << endl;
        printinfo(platform_info[x]);
    }

    // get cl devices
    cl_device_id devices[MAX_DEVICE_ID] {};
    cl_uint devices_n;
    check_cl_error(clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_DEFAULT, MAX_DEVICE_ID, devices, &devices_n));
    for (x = 0; x < devices_n; ++x) {
        clGetDeviceInfo(devices[x], CL_DEVICE_NAME, sizeof(__temp_info.name), __temp_info.name, nullptr);
        clGetDeviceInfo(devices[x], CL_DEVICE_VENDOR, sizeof(__temp_info.vendor), __temp_info.vendor, nullptr);
        clGetDeviceInfo(devices[x], CL_DEVICE_VERSION, sizeof(__temp_info.version), &__temp_info.version, nullptr);
        device_info.emplace_back(__temp_info);
    }
    cl_device_id target_device = devices[0];
    size_t sz;
    check_cl_error(clGetDeviceInfo(target_device, CL_DEVICE_GLOBAL_MEM_SIZE, 0, 0, &sz));
    size_t* m_sz = new size_t;
    check_cl_error(clGetDeviceInfo(target_device, CL_DEVICE_GLOBAL_MEM_SIZE, sz, m_sz, &sz));
    cout << "Размер видеопамяти: ";

    check_cl_error(clGetDeviceInfo(target_device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sz, m_sz, &sz));

    cout << *m_sz / 1024 / 1024 << " MiB" << endl;
    delete m_sz;

    cout << "Целевой графический процессор: " << device_info[0].name << std::endl << std::endl;

    // create cl context
    cl_context gpu_context = clCreateContext(nullptr, 1, devices, nullptr, nullptr, &err);
    check_cl_error(err);

    // create command queue
    cl_command_queue command = clCreateCommandQueueWithProperties(gpu_context, target_device, nullptr, &err);
    check_cl_error(err);

    // create cl buffer (GPU Memory)
    constexpr auto mem_size = 1024 * 1024;
    cl_mem mem_gpu = clCreateBuffer(gpu_context, CL_MEM_READ_WRITE, mem_size, nullptr, &err);
    check_cl_error(err);

    cout << "Выделено памяти из графического процессора: " << mem_size / 1024 << " КиБ" << endl;

    // make cl mem
    char* text_to_gpu_mem = strdup("Вадим");
    cout << "КОМПЬЮТЕР -> ГПУ: \"" << text_to_gpu_mem << "\" ... ";
    check_cl_error(clEnqueueWriteBuffer(command, mem_gpu, CL_TRUE, 0, strlen(text_to_gpu_mem), text_to_gpu_mem, 0, nullptr, nullptr));
    check_cl_error(clFinish(command));
    free(text_to_gpu_mem);
    cout << "ваша имя записана успешно" << endl << endl;

    std::this_thread::sleep_for(chrono::seconds(2));

    cout << "Теперь читаем ваше имя из графического процессора" << endl;
    cout << "КОМПЬЮТЕР <- ГПУ: ";
    char text_from_gpu_mem[64] {};
    check_cl_error(clEnqueueReadBuffer(command, mem_gpu, CL_TRUE, 0, 64, text_from_gpu_mem, 0, 0, 0));

    cout << '"' << text_from_gpu_mem << '"' << endl << endl;
    check_cl_error(clFinish(command));

    check_cl_error(clReleaseMemObject(mem_gpu));

    // Out information

    for (x = 0; x < devices_n; ++x)
        check_cl_error(clReleaseDevice(devices[x]));

    check_cl_error(clReleaseContext(gpu_context));

    cout << "Программа завершена. Все ресурсы освобождены." << endl;
    return 0;
}
