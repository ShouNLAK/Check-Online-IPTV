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
