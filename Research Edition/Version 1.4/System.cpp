#include "System.h"

// msleep: Sleeps for the specified number of milliseconds.
// On Windows, it calls Sleep(ms), and on Unix-like systems, it calls usleep(ms * 1000).
void msleep(int ms) 
{
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000); // Convert milliseconds to microseconds
#endif
}

int get_number_of_processors(void) {
#ifdef _WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    int n = (int)sysinfo.dwNumberOfProcessors;
    return n > 0 ? n : 1;
#else
    FILE* fp = popen("nproc", "r");
    if (fp) {
        char buf[32] = {0};
        if (fgets(buf, sizeof(buf), fp)) {
            int n = atoi(buf);
            pclose(fp);
            return n > 0 ? n : 1;
        }
        pclose(fp);
    }
    return 1;
#endif
}
