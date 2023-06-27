#if not defined(__WIN32__)
// #include <unistd.h>
#include <stdio.h>
// #include <sys/stat.h>
// #include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

void awake_get_file_uid();

int main()
{
    //    char a[1024];
    //    int r = gethostname(a,1024);

    //    __uid_t id = getuid();

    //    struct stat s;
    //    r = stat("/tmp/packagekit-alpm-updatesa", &s);
    //    auto b = ctime((time_t*)&s.st_atim);

    awake_get_file_uid();
    return 0;
}

#include <vector>
#include <stdlib.h>
#include <string.h>

void awake_get_file_uid()
{
    std::vector<passwd> v;

    __uid_t id;
    for (id = 0; id < 2000; ++id) {
        passwd* pw = getpwuid(id);
        if (pw == nullptr)
            continue;
        v.push_back(*pw);
        passwd& _x = v[id];
#define __ref(X) ((X) = (char*)memcpy(malloc(strlen((X))), (X), strlen(X)))
        __ref(_x.pw_dir);
        __ref(_x.pw_gecos);
        __ref(_x.pw_name);
        __ref(_x.pw_passwd);
        __ref(_x.pw_shell);
#undef __ref
    }
    int ll = 0;
}
#else
#include <iostream>
int main(void){
    std::cout << "This is application not supported" << std::endl;
}
#endif
