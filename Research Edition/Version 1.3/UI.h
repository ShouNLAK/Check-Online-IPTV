#ifndef UI_H
#define UI_H

#include "Header.h"
#include "Header.h"
#include "System.h"
#include "Sorting.h"

// Reset and Text Attributes
#define RESET             "\033[0m"
#define BOLD              "\033[1m"
#define DIM               "\033[2m"
#define ITALIC            "\033[3m"   // Not widely supported
#define UNDERLINE         "\033[4m"
#define BLINK             "\033[5m"   // May not work in many modern terminals
#define REVERSE           "\033[7m"
#define HIDDEN            "\033[8m"

// Standard Colors
#define BLACK          "\033[30m"
#define RED            "\033[31m"
#define GREEN          "\033[32m"
#define YELLOW         "\033[33m"
#define BLUE           "\033[34m"
#define MAGENTA        "\033[35m"
#define CYAN           "\033[36m"
#define WHITE          "\033[37m"

// Bright Colors
#define BRIGHT_BLACK   "\033[90m"
#define BRIGHT_RED     "\033[91m"
#define BRIGHT_GREEN   "\033[92m"
#define BRIGHT_YELLOW  "\033[93m"
#define BRIGHT_BLUE    "\033[94m"
#define BRIGHT_MAGENTA "\033[95m"
#define BRIGHT_CYAN    "\033[96m"
#define BRIGHT_WHITE   "\033[97m"

// Standard Background Colors
#define BG_BLACK          "\033[40m"
#define BG_RED            "\033[41m"
#define BG_GREEN          "\033[42m"
#define BG_YELLOW         "\033[43m"
#define BG_BLUE           "\033[44m"
#define BG_MAGENTA        "\033[45m"
#define BG_CYAN           "\033[46m"
#define BG_WHITE          "\033[47m"

// Bright Background Colors
#define BG_BRIGHT_BLACK   "\033[100m"
#define BG_BRIGHT_RED     "\033[101m"
#define BG_BRIGHT_GREEN   "\033[102m"
#define BG_BRIGHT_YELLOW  "\033[103m"
#define BG_BRIGHT_BLUE    "\033[104m"
#define BG_BRIGHT_MAGENTA "\033[105m"
#define BG_BRIGHT_CYAN    "\033[106m"
#define BG_BRIGHT_WHITE   "\033[107m"


void print_banner(const char* title, const char* color);
void countdown(const char* message, int seconds);
int utf8_display_width(const char* str);
void print_utf8_fixed_width(const char* str, int width);
void update_progress_bar(int current, int total, const char* channel_name, char spinner, int eta_hours, int eta_minutes, int eta_secs);
void print_scan_summary(ChannelStatus* status_list, int processed, double elapsed_sec);

#endif