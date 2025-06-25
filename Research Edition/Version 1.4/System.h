#ifndef SYSTEM_H
#define SYSTEM_H

#ifdef _WIN32
#include <windows.h>
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#else
#include <strings.h>
#endif

#ifdef _WIN32
#define POPEN(cmd, mode) _popen(cmd, mode)
#define PCLOSE(fp)       _pclose(fp)
#else
#define POPEN(cmd, mode) popen(cmd, mode)
#define PCLOSE(fp)       pclose(fp)
#endif

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

void msleep(int ms);
int get_number_of_processors(void);

#endif

