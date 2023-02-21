#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

int main(){
    char a[1024];
    int r = gethostname(a,1024);

    __uid_t id = getuid();


    struct stat s;
    r = stat("/tmp/packagekit-alpm-updatesa", &s);
    auto b = ctime((time_t*)&s.st_atim);

}
