#ifndef LOG_H
#define LOG_H

#include "Header.h"

// CSV log functions
void initialize_log_csv(const char* filename);
void write_log_csv(const char* filename,
    const char* channel_name,
    const char* status,
    const char* resolution,
    const char* url,
    const char* fixed_url);

// TXT log functions (ASCII table)
void initialize_log_txt(const char* filename);
void write_log_txt(const char* filename,
    const char* channel_name,
    const char* status,
    const char* resolution,
    const char* url,
    const char* fixed_url);

#endif // LOG_H