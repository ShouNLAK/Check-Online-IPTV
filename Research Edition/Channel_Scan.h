#ifndef CHANNEL_SCAN_H
#define CHANNEL_SCAN_H

#include "Header.h"
#include "Sorting.h"
#include "System.h"
#include "UI.h"

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
void Check_Active(const char* input, const char* output);
int getResolutionArea(const char* resolution);

#endif
