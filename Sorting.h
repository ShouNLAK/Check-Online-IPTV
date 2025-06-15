#ifndef SORTING_H
#define SORTING_H

#include "Header.h"


int compare_resolutions(const void* p, const void* q);
int cmp_streams(const void* a, const void* b);
void sortStatusList(ChannelStatus* status_list, int processed);
void convertResolutionToLabel(const char* resolution, char* label, size_t label_size);

#endif
