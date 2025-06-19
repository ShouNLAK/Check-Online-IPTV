#include "log.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// CSV log functions
void initialize_log_csv(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (fp) {
        fprintf(fp, "Time,Channel Name,Status,Resolution,Original URL,Fetched URL\n");
        fclose(fp);
    }
}

void write_log_csv(const char* filename,
    const char* channel_name,
    const char* status,
    const char* resolution,
    const char* url,
    const char* fixed_url)
{
    FILE* fp = fopen(filename, "a");
    if (!fp) {
        perror("Error opening log file in write_log_csv");
        return;
    }
    time_t now = time(NULL);
    struct tm* local = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local);

    fprintf(fp, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n",
        time_str,
        channel_name ? channel_name : "",
        status ? status : "",
        resolution ? resolution : "",
        url ? url : "",
        fixed_url ? fixed_url : "");
    fflush(fp);
    fclose(fp);
}

// TXT log functions (ASCII table)
void initialize_log_txt(const char* log_filename) {
    FILE* fp = fopen(log_filename, "wt");
    if (fp) {
        fprintf(fp, "%-20s | %-30s | %-12s | %-13s | %s\n",
            "Time", "Channel Name", "Status", "Resolution", "URL");
        fprintf(fp, "---------------------------------------------------------------------------------------------\n");
        fclose(fp);
#ifdef _WIN32
        Sleep(500);
#else
        usleep(500 * 1000);
#endif
    }
    else {
        perror("Error opening log file in initialize_log_txt");
    }
}

void write_log_txt(const char* log_filename,
    const char* channel_name,
    const char* status,
    const char* resolution,
    const char* url,
    const char* fixed_url)
{
    FILE* fp = fopen(log_filename, "a");
    if (!fp) {
        perror("Error opening log file in write_log_txt");
        return;
    }

    time_t now = time(NULL);
    struct tm* local = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local);

    fprintf(fp, "%-20s | %-30s | %-12s | %-13s | %s\n",
        time_str, channel_name, status, resolution, url);

    char arrow_text[1024];
    if (strcmp(status, "Inactive") == 0) {
        snprintf(arrow_text, sizeof(arrow_text), "   ---> Unable to reach");
    }
    else {
        if (strcmp(url, fixed_url) != 0) {
            snprintf(arrow_text, sizeof(arrow_text), "   ---> %s", fixed_url);
        }
        else {
            snprintf(arrow_text, sizeof(arrow_text), "   ---> %s", url);
        }
    }
    fprintf(fp, "%-20s | %-30s | %-12s | %-13s | %s\n",
        "", "", "", "", arrow_text);
    fprintf(fp, "---------------------------------------------------------------------------------------------\n");
    fflush(fp);
#ifdef _WIN32
    Sleep(500);
#else
    usleep(500 * 1000);
#endif
    fclose(fp);
}