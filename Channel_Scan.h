#ifndef CHANNEL_SCAN_H
#define CHANNEL_SCAN_H

#include "Header.h"
#include "Sorting.h"
#include "System.h"
#include "UI.h" // Needed for progress bar and summary UI functions

#ifdef _WIN32
#include <windows.h>
typedef HANDLE thread_handle_t;
typedef CRITICAL_SECTION mutex_t;
#define THREAD_FUNC_RETURN DWORD WINAPI
#define THREAD_FUNC_CALL __stdcall
#else
#include <pthread.h>
typedef pthread_t thread_handle_t;
typedef pthread_mutex_t mutex_t;
#define THREAD_FUNC_RETURN void*
#define THREAD_FUNC_CALL
#endif

int count_total_channels(FILE* fi);
void rollback_to_original_if_needed(const char* original, char* fixed, size_t fixed_size);
bool read_channel_entry(FILE* fi, char* extinf_line, size_t extinf_size, char* url_line, size_t url_size);
void extract_channel_name(const char* extinf_line, char* out_name, size_t name_size);
void Remove_Duplicates(const char* output);
int test_channel_stream(char* url, char* resolved_url, size_t resolved_size, char* fetched_resolution, size_t reso_size, int* is_timeout, int* is_ts);
void store_channel_status(ChannelStatus* status,
    const char* name,
    const char* url,
    const char* resolution,
    int active,
    int timeout);
void Check_Active(const char* input, const char* output, int concurrency, int enable_live_log);
int getResolutionArea(const char* resolution);

typedef struct {
    int index;
    char extinf_line[1024];
    char url_line[1024];
    ChannelStatus* status_list;
    int* processed;
    int total_channels;
    char* scan_chars;
    int* scan_idx;
    int* eta_h;
    int* eta_m;
    int* eta_s;
    clock_t* start_clock;
    mutex_t* progress_mutex;
} ThreadData;

THREAD_FUNC_RETURN THREAD_FUNC_CALL channel_worker(void* arg);

#endif
