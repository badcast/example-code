#include <iostream>
#include <vector>
#include <cstdint>

#ifdef __WIN32__
#include <windows.h>
#elif __unix__
#include <unistd.h>
#endif

int system_get_page_size()
{
    int _PAGE_SIZE;
#if __unix__ || __linux__
    _PAGE_SIZE = sysconf(_SC_PAGE_SIZE);
#elif WIN32
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    _PAGE_SIZE = static_cast<int>(sysInfo.dwPageSize);
#else
    _PAGE_SIZE = 4096;
#endif
    return _PAGE_SIZE;
}

int main(int argc, char* argv[])
{
    std::uint32_t page_size = -1;

    if (argc == 2) {
        try {
            page_size = std::atoi(argv[1]);
        } catch (const std::exception&) {
            page_size = -1;
        }
    }

    if (page_size == -1)
        page_size = system_get_page_size();

    std::cout << "Page Size in bytes: " << page_size << std::endl;

    void* buffer;
    std::vector<void*> __buffer_list {};
    __buffer_list.reserve(1024);

    while ((buffer = std::malloc(page_size)) != nullptr) {
        __buffer_list.push_back(buffer);
    }
    std::cout << "Max total allocated: " << __buffer_list.size() << std::endl;

    std::cout << "Release garbage" << std::endl;
    for (int x = 0; x < __buffer_list.size(); ++x) {
        std::free(__buffer_list[x]);
    }
    std::cout << "End program" << std::endl;
    return 0;
}
