#include <stdio.h>
#include <dlfcn.h>

int main(int argc, char *argv[])
{
    void *handle;

    if(argc < 2) return 1;

    printf("start test dll %s...\n", argv[1]);
    handle = dlopen(argv[1], RTLD_NOW);
    if(NULL == handle) {
        printf("failed to open dll: %s\n", dlerror());
        return 1;
    }

    printf("open dll OK: %s\n", dlerror());
    dlclose(handle);

    return 0;
}
