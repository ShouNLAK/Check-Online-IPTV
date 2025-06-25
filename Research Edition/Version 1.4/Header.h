#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>

// Channel information
typedef struct {
    char name[256]; // Channel Name
    char url[1024]; // Channel URL
    char resolution[128]; // Resolution information
	int active; // Active : 1 | Inactive : 0
	int timeout; // Timeout for responding to requests
} ChannelStatus;

typedef struct {
    char resolution[64]; // Resolution for specific stream
    char stream_url[1024]; // URL for the stream
    int area;  // Known as resolution area
} StreamInfo;

#endif