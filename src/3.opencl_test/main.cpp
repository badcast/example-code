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
    char version[128];
};

void check_cl_error(cl_int errcode)
{
#define CL_DEF(CODE)                                                    \
    case CODE:                                                          \
        std::cout << std::endl << "CL Error: " << (#CODE) << std::endl; \
        break
    switch (errcode) {
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
        cout << "UNKNOWN_CL_ERROR: " << errcode << endl;
    case CL_SUCCESS:
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
    cl_int err, status;

    // get cl platrforms
    std::uint32_t platforms_n;
    check_cl_error(clGetPlatformIDs(MAX_PLATFORM_ID, platforms, &platforms_n));

    for (x = 0; x < platforms_n; ++x) {
        check_cl_error(clGetPlatformInfo(platforms[x], CL_PLATFORM_NAME, sizeof(__temp_info.name), __temp_info.name, nullptr));
        check_cl_error(clGetPlatformInfo(platforms[x], CL_PLATFORM_VENDOR, sizeof(__temp_info.vendor), __temp_info.vendor, nullptr));
        check_cl_error(clGetPlatformInfo(platforms[x], CL_PLATFORM_VERSION, sizeof(__temp_info.version), __temp_info.version, nullptr));
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
    check_cl_error(clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, MAX_DEVICE_ID, devices, &devices_n));
    for (x = 0; x < devices_n; ++x) {
        clGetDeviceInfo(devices[x], CL_DEVICE_NAME, sizeof(__temp_info.name), __temp_info.name, nullptr);
        clGetDeviceInfo(devices[x], CL_DEVICE_VENDOR, sizeof(__temp_info.vendor), __temp_info.vendor, nullptr);
        clGetDeviceInfo(devices[x], CL_DEVICE_VERSION, sizeof(__temp_info.version), __temp_info.version, nullptr);
        device_info.emplace_back(__temp_info);
    }
    cl_device_id target_device = devices[0];
    size_t sz;
    cout << "Размер видеопамяти: ";
    err = clGetDeviceInfo(target_device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(sz), &sz, nullptr);
    check_cl_error(err);

    cout << sz / 1024 / 1024 << " MiB" << endl;

    cout << "Целевой графический процессор: " << device_info[0].name << std::endl << std::endl;

    // create cl context
    cl_context gpu_context = clCreateContext(nullptr, 1, &target_device, nullptr, nullptr, &err);
    check_cl_error(err);

    // create command queue
    cl_command_queue command = clCreateCommandQueue(gpu_context, target_device, 0, &err);
    check_cl_error(err);

    // can exec program on gpu
    cout << "Выполняем сложение двух векторов в GPU: ";
    err = clGetDeviceInfo(target_device, CL_DEVICE_COMPILER_AVAILABLE, sizeof(status), &status, nullptr);
    check_cl_error(err);
    if (status == CL_TRUE) {
        std::size_t vector_size = 100000;

        std::vector<int> _vector_a(vector_size, 0);
        std::vector<int> _vector_b(vector_size, 0);
        std::vector<int> _vector_c(vector_size, 0);

        for (int x = 0; x < vector_size; ++x) {
            _vector_a[x] = x + 1;
            _vector_b[x] = x + 1;
        }

        cout << "поддерживается" << std::endl;

        const char* source = "__kernel void add_vectors(__global int * a, __global int * b, __global int * c) {"
                             "int idx = get_global_id(0);"
                             "c[idx] = a[idx]+b[idx];"
                             "}";
        sz = std::strlen(source);
        cl_program program = clCreateProgramWithSource(gpu_context, 1, &source, &sz, &err);
        check_cl_error(err);

        err = clBuildProgram(program, 1, &target_device, nullptr, nullptr, nullptr);
        check_cl_error(err);
        do {
            check_cl_error(clGetProgramBuildInfo(program, target_device, CL_PROGRAM_BUILD_STATUS, sizeof(status), &status, nullptr));
        } while (status == CL_BUILD_IN_PROGRESS);

        cl_kernel kernel = clCreateKernel(program, "add_vectors", &err);
        check_cl_error(err);

        cl_mem _mem_a, _mem_b, _mem_output;

        _mem_a = clCreateBuffer(gpu_context, CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, vector_size * sizeof(int), _vector_a.data(), &err);
        check_cl_error(err);

        _mem_b = clCreateBuffer(gpu_context, CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, vector_size * sizeof(int), _vector_b.data(), &err);
        check_cl_error(err);

        _mem_output = clCreateBuffer(gpu_context, CL_MEM_READ_WRITE, vector_size * sizeof(int), _vector_c.data(), &err);
        check_cl_error(err);

        err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &_mem_a);
        check_cl_error(err);
        err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &_mem_b);
        check_cl_error(err);
        err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &_mem_output);
        check_cl_error(err);

        // execute
        size_t group_size;
        err = clGetKernelWorkGroupInfo(kernel, target_device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &group_size, nullptr);
        check_cl_error(err);

        err = clEnqueueNDRangeKernel(command, kernel, 1, nullptr, &group_size, &vector_size, 0, nullptr, nullptr);
        check_cl_error(err);

        // test reuslt
        err = clEnqueueReadBuffer(command, _mem_output, CL_TRUE, 0, vector_size * sizeof(int), _vector_c.data(), 0, 0, 0);
        check_cl_error(err);

        // Wait finising queue command's
        clFinish(command);

        status = 0;
        for (x = 0; x < vector_size; ++x) {
            if (_vector_a[x] + _vector_b[x] != _vector_c[x]) {
                ++status;
            }
        }

        std::cout << "Результат векторов завершен. Ошибок: " << status << std::endl;

        // release
        clReleaseMemObject(_mem_a);
        clReleaseMemObject(_mem_b);
        clReleaseMemObject(_mem_output);

        clReleaseKernel(kernel);
    } else {
        cout << "Компилятор не поддерживается (пропуск)" << std::endl;
    }
    // create cl buffer (GPU Memory)
    constexpr auto mem_size = 1024 * 1024;
    cl_mem mem_gpu = clCreateBuffer(gpu_context, CL_MEM_READ_WRITE, mem_size, nullptr, &err);
    check_cl_error(err);
    cout << std::endl << "Выделено памяти из графического процессора: " << mem_size / 1024 << " КиБ" << endl;

    char* text_to_gpu_mem = strdup(getenv("USER"));
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
