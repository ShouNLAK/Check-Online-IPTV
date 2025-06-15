#ifndef LOG_H
#define LOG_H

#include "Header.h"

// Writes a log entry with the specified fields.
void write_log_line(const char* log_filename,
    const char* channel_name,
    const char* status,       // e.g., "Redirected", "Live", or "Inactive"
    const char* resolution,   // e.g., "1920x1080" or empty if N/A
    const char* url,          // Original URL
    const char* fixed_url);   // Final (updated) URL

// Clears the log file contents and writes a header row.
void initialize_log_file(const char* log_filename);




#endif // LOG_H