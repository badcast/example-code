#if __linux__
// #include <unistd.h>
#include <string>
#include <stdio.h>
// #include <sys/stat.h>
// #include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

void awake_get_uid();

struct passwd_struct {
    std::string pw_name; /* Username.  */
    std::string pw_passwd; /* Hashed passphrase, if shadow database
                      not in use (see shadow.h).  */
    __uid_t pw_uid; /* User ID.  */
    __gid_t pw_gid; /* Group ID.  */
    std::string pw_gecos; /* Real name.  */
    std::string pw_dir; /* Home directory.  */
    std::string pw_shell; /* Shell program.  */

    passwd_struct(passwd* pw)
    {
        this->pw_name = pw->pw_name;
        this->pw_passwd = pw->pw_passwd;
        this->pw_uid = pw->pw_uid;
        this->pw_gid = pw->pw_gid;
        this->pw_gecos = pw->pw_gecos;
        this->pw_dir = pw->pw_dir;
        this->pw_shell = pw->pw_shell;
    }
};

int main()
{
    //    char a[1024];
    //    int r = gethostname(a,1024);

    //    __uid_t id = getuid();

    //    struct stat s;
    //    r = stat("/tmp/packagekit-alpm-updatesa", &s);
    //    auto b = ctime((time_t*)&s.st_atim);

    awake_get_uid();
    return 0;
}

#include <vector>
#include <stdlib.h>
#include <string.h>

void awake_get_uid()
{
    std::vector<passwd_struct> v;

    __uid_t id;
    for (id = 0; id < 2000; ++id) {
        passwd* pw = getpwuid(id);
        if (pw == nullptr)
            continue;

        v.emplace_back(pw);
    }
}
#else
#include <iostream>
int main(void) { std::cout << "This is application not supported" << std::endl; }
#endif
