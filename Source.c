#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#else
#include <strings.h>
#endif

// Function to check internet connectivity
int check_internet() {
#ifdef _WIN32
    return system("ping -n 1 google.com >nul") == 0;
#else
    return system("ping -c 1 google.com >/dev/null 2>&1") == 0;
#endif
}

typedef struct {
        char name[256];
        char url[1024];
        char resolution[64]; // resolution info if available
        int quality; 
        int active;
        int timeout;
    } ChannelStatus;

// Define input and output file names
#define Input "input.txt"
#define Output "output.m3u"


void Check_Active(const char* input, const char* output) 
{
    // Count total #EXTINF lines
    int total_channels = 0; // Total channels to process
    char line[1024]; // Buffer for reading lines

    // Initialize counters
    int processed = 0; // Number of channels processed
    int active_count = 0; // Number of active channels
    int inactive_count = 0; // Number of inactive channels
    int timeout_count = 0; // Number of channels that timed out

    // Quality mapping for standard resolutions
    struct { int q; const char* res; } qmap[] = {
                    {480, "SD"}, {576, "SD"}, {720, "HD"}, {1080, "FHD"}, {1440, "QHD"}, {2160, "4K"}, {4320, "8K"}
                };

    // URL buffer for reading channel URLs
    char url[1024];

    // Command buffer for curl (CMD)
    char command[1400];

    // Start timer
    clock_t start_time = clock();
    // Open input
    FILE* fi = fopen(input, "r");
    if (fi == NULL) {
        printf("Error opening input: %s\n", input);
        return;
    }
    // Open output
    FILE* fo = fopen(output, "w");
    if (fo == NULL) {
        printf("Error opening output: %s\n", output);
        fclose(fi);
        return;
    }

    // First line of output always : "#EXTM3U"
    fprintf(fo, "#EXTM3U\n");
    // Always skip the first line of the input
    fscanf(fi, "%*[^\n]\n");

    // Read through the file to count channels
    while (fgets(line, sizeof(line), fi)) {
        if (strstr(line, "#EXTINF"))
            total_channels++;
    }
    // Rewind file pointer and skip the first line again
    rewind(fi);
    fscanf(fi, "%*[^\n]\n");

    ChannelStatus* status_list = (ChannelStatus*)malloc(total_channels * sizeof(ChannelStatus));
    if (!status_list) {
        printf("Memory allocation failed.\n");
        fclose(fi);
        fclose(fo);
        return;
    }

    // For TV scan effect
    const char* scan_chars = "|/-\\";
    int scan_idx = 0;

    // UI: Print header
    printf("==============================================\n");
    printf("         TV Channel Active Scanner\n");
    printf("==============================================\n");
    printf("Scanning %d channels. Please wait...\n\n", total_channels);

    // Read through the file again to process channels
    while (fgets(line, sizeof(line), fi)) {
        // Remove newline character
        line[strcspn(line, "\n")] = '\0';

        // Process only lines with "#EXTINF"
        if (strstr(line, "#EXTINF") == NULL)
            continue;

        // Extract channel name after last comma
        char channel_name[256] = "Active Channel";
        char* last_comma = strrchr(line, ',');
        if (last_comma && *(last_comma + 1)) {
            strncpy(channel_name, last_comma + 1, sizeof(channel_name) - 1);
            channel_name[sizeof(channel_name) - 1] = '\0';
        }
        // Extract resolution or quality info from channel name (e.g., "Channel Name (HD)" or "Channel Name (1080p)")
        char resolution[64] = "";
        int quality = 0; // Temporary, will assign to status_list later
        char* open_paren = strrchr(channel_name, '(');
        char* close_paren = strrchr(channel_name, ')');
        char quality_str[64] = ""; // Store original quality string if numeric

        if (open_paren && close_paren && close_paren > open_paren) {
            size_t len = close_paren - open_paren - 1;
            if (len > 0 && len < sizeof(resolution)) {
            strncpy(resolution, open_paren + 1, len);
            resolution[len] = '\0';
            // Remove trailing spaces
            while (len > 0 && resolution[len - 1] == ' ') {
                resolution[--len] = '\0';
            }
            // Remove trailing 'p' if present (e.g., "1080p")
            size_t rlen = strlen(resolution);
            if (rlen > 0 && (resolution[rlen - 1] == 'p' || resolution[rlen - 1] == 'P')) {
                resolution[rlen - 1] = '\0';
            }
            // Save original numeric quality string if it's a number
            strncpy(quality_str, resolution, sizeof(quality_str) - 1);
            quality_str[sizeof(quality_str) - 1] = '\0';

            // Make resolution always uppercase
            for (size_t i = 0; i < strlen(resolution); ++i) {
                if (resolution[i] >= 'a' && resolution[i] <= 'z') {
                resolution[i] = resolution[i] - 'a' + 'A';
                }
            }
            // Remove resolution/quality info from channel_name
            size_t name_len = open_paren - channel_name;
            if (name_len > 0 && name_len < sizeof(channel_name)) {
                channel_name[name_len - 1] = '\0'; // Remove space before '('
            } else if (name_len < sizeof(channel_name)) {
                channel_name[name_len] = '\0';
            }
            // If resolution is blank or "0", skip this channel
            if (resolution[0] == '\0' || (strlen(resolution) == 1 && resolution[0] == '0')) {
                continue;
            }
            // Decide if it's a number (quality) or string (resolution)
            char* endptr = NULL;
            int res_val = (int)strtol(resolution, &endptr, 10);
            if (*resolution && *endptr == '\0') {
                // All digits: treat as quality (int)
                quality = res_val;
                // Save the original numeric string as resolution
                strncpy(resolution, quality_str, sizeof(resolution) - 1);
                resolution[sizeof(resolution) - 1] = '\0';
            } else {
                // Not all digits: treat as resolution (string)
                quality = 0;
                // Map string to standard quality if possible
                if (strcasecmp(resolution, "SD") == 0) quality = 480;
                else if (strcasecmp(resolution, "HD") == 0) quality = 720;
                else if (strcasecmp(resolution, "FHD") == 0) quality = 1080;
                else if (strcasecmp(resolution, "QHD") == 0) quality = 1440;
                else if (strcasecmp(resolution, "4K") == 0) quality = 2160;
                else if (strcasecmp(resolution, "8K") == 0) quality = 4320;
            }
            // If quality is a number, map it back to a standard resolution string if possible
            if (quality > 0) {
                const char* mapped_res = NULL;
                int min_diff = 100000;
                for (size_t qi = 0; qi < sizeof(qmap)/sizeof(qmap[0]); ++qi) {
                    int diff = abs(quality - qmap[qi].q);
                    if (diff < min_diff) {
                        min_diff = diff;
                        mapped_res = qmap[qi].res;
                    }
                }
                if (mapped_res) {
                    strncpy(resolution, mapped_res, sizeof(resolution) - 1);
                    resolution[sizeof(resolution) - 1] = '\0';
                }
            }
            }
        }
        // Save quality and resolution to status_list
        if (status_list && processed < total_channels) {
            status_list[processed].quality = quality;
            strncpy(status_list[processed].resolution, resolution, sizeof(status_list[processed].resolution) - 1);
            status_list[processed].resolution[sizeof(status_list[processed].resolution) - 1] = '\0';
        }
        processed++;
        // TV scan style progress
        // Clear line with ANSI escape code, then print
        int barWidth = 30;
        double progress = (double)processed / total_channels;
        int pos = (int)(barWidth * progress);

        // Clear line and print progress bar with percentage (no \n at end)
        printf("\33[2K\rScanning [%c] %3d/%d: %-30.30s | [", 
        scan_chars[scan_idx], processed, total_channels, channel_name);
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) printf("=");
            else if (i == pos) printf(">");
            else printf(" ");
        }
        printf("] %3d%%", (int)(progress * 100));
        fflush(stdout);
        scan_idx = (scan_idx + 1) % 4;

        // Read next line (should be URL)
        if (!fgets(url, sizeof(url), fi))
            break;
        url[strcspn(url, "\n")] = '\0';

        int is_active = 0;
        int is_timeout = 0;
        // Check if URL contains "http://" or "https://"
        if (strstr(url, "http://") || strstr(url, "https://")) {
            // Use curl to fetch headers, follow redirects, 15s timeout
            snprintf(command, sizeof(command), "curl -I -L -s --max-time 15 \"%s\" 2>&1", url);

            int status_code = 0;
            int got_response = 0;
            int is_download = 0;

            FILE* fp = _popen(command, "r");
            if (fp) {
            char resp[256];
            while (fgets(resp, sizeof(resp), fp)) {
                if (strncmp(resp, "HTTP/", 5) == 0) {
                char* token = strtok(resp, " ");
                token = strtok(NULL, " ");
                if (token)
                    status_code = atoi(token);
                got_response = 1;
                }
                // Check for Content-Disposition: attachment (download)
                if (strncasecmp(resp, "Content-Disposition:", 19) == 0 && strstr(resp, "attachment"))
                is_download = 1;
                // Check for curl timeout message
                if (strstr(resp, "Operation timed out")) {
                is_timeout = 1;
                }
            }
            _pclose(fp);

            // Consider active if HTTP 2xx/3xx or Content-Disposition: attachment
            if (is_timeout) {
                is_active = 0;
                timeout_count++;
                inactive_count++;
            } else if ((got_response && status_code >= 200 && status_code < 400) || is_download) {
                is_active = 1;
                active_count++;
            } else {
                is_active = 0;
                inactive_count++;
            }
            } else {
            // Could not run curl, count as inactive
            is_active = 0;
            inactive_count++;
            }
        } else {
            is_active = 0;
            inactive_count++;
        }

        // Save status for summary and output
        if (status_list && processed-1 < total_channels) {
            strncpy(status_list[processed-1].name, channel_name, sizeof(status_list[processed-1].name)-1);
            status_list[processed-1].name[sizeof(status_list[processed-1].name)-1] = '\0';
            strncpy(status_list[processed-1].url, url, sizeof(status_list[processed-1].url)-1);
            status_list[processed-1].url[sizeof(status_list[processed-1].url)-1] = '\0';
            status_list[processed-1].active = is_active;
            status_list[processed-1].timeout = is_timeout;
        }
        // Save resolution info
        strncpy(status_list[processed-1].resolution, resolution, sizeof(status_list[processed-1].resolution)-1);
        status_list[processed-1].resolution[sizeof(status_list[processed-1].resolution)-1] = '\0';
        // After the scan is done, clear the progress line so it's blank
        printf("\33[2K\r");
        fflush(stdout);
    }

    //Sort the status list by channel name (In alphabetical order) (if  matching, sort by quality from low to high)
    for (int i = 0; i < processed - 1; ++i) {
        for (int j = i + 1; j < processed; ++j) {
            // Compare names first
            int cmp = strcasecmp(status_list[i].name, status_list[j].name);
            if (cmp > 0 || (cmp == 0 && status_list[i].quality > status_list[j].quality)) {
                // Swap if out of order
                ChannelStatus temp = status_list[i];
                status_list[i] = status_list[j];
                status_list[j] = temp;
            }
        }
    }

    // Clear progress line and print done message
    printf("\33[2K\rScanning [✓] Complete! %d/%d channels processed.\n", processed, total_channels);
    clock_t end_time = clock();
    double elapsed_sec = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // Calculate elapsed time in hours, minutes, seconds
    int elapsed_sec_int = (int)elapsed_sec;
    int hours = elapsed_sec_int / 3600;
    int minutes = (elapsed_sec_int % 3600) / 60;
    int seconds = elapsed_sec_int % 60;

    // Print summary table
    printf("+------+----------------------------------------+----------------------+------------+\n");
printf("| No.  | Channel Name                           | Quality - Resolution | Status     |\n");
printf("+------+----------------------------------------+----------------------+------------+\n");
    int idx = 1;
    // Print ACTIVE channels
    for (int i = 0; i < processed; ++i) {
        if (status_list[i].active) {
            char qual_res[32] = "";
            if (status_list[i].quality > 0 && strlen(status_list[i].resolution) > 0) {
                snprintf(qual_res, sizeof(qual_res), "%-7d - %-13s", status_list[i].quality, status_list[i].resolution);
            } else if (status_list[i].quality > 0) {
                snprintf(qual_res, sizeof(qual_res), "%d", status_list[i].quality);
            } else if (strlen(status_list[i].resolution) > 0) {
                snprintf(qual_res, sizeof(qual_res), "%s", status_list[i].resolution);
            }
            printf("%-6d | %-40.40s | %-18.18s | \033[1;32mACTIVE\033[0m\n", idx++, status_list[i].name, qual_res);
        }
    }
    printf("-------+------------------------------------------+----------------------+------------\n");
    // Print INACTIVE channels (including timeout)
    idx = 1;
    for (int i = 0; i < processed; ++i) {
        if (!status_list[i].active) {
            char qual_res[32] = "";
            if (status_list[i].quality > 0 && strlen(status_list[i].resolution) > 0) {
                snprintf(qual_res, sizeof(qual_res), "%-7d - %-13s", status_list[i].quality, status_list[i].resolution);
            } else if (status_list[i].quality > 0) {
                snprintf(qual_res, sizeof(qual_res), "%d", status_list[i].quality);
            } else if (strlen(status_list[i].resolution) > 0) {
                snprintf(qual_res, sizeof(qual_res), "%s", status_list[i].resolution);
            }
            if (status_list[i].timeout) {
                printf("%-6d | %-40.40s | %-18.18s | \033[1;33mTIMEOUT\033[0m\n", idx++, status_list[i].name, qual_res);
            } else {
                printf("%-6d | %-40.40s | %-18.18s | \033[1;31mINACTIVE\033[0m\n", idx++, status_list[i].name, qual_res);
            }
        }
    }
    printf("+------+----------------------------------------+----------------------+------------+\n");
    printf("Total: %d, Will be added to output: \033[1;32m%d\033[0m, Inactive: \033[1;31m%d\033[0m, Timeout: \033[1;33m%d\033[0m\n", processed, active_count, inactive_count, timeout_count);
    printf("Total scan time: %02d:%02d:%02d (hh:mm:ss)\n", hours, minutes, seconds);

    // Write only ACTIVE channels to the output file
    for (int i = 0; i < processed; ++i) {
        if (status_list[i].active) {
            // Include resolution in the channel name if available
            if (strlen(status_list[i].resolution) > 0) {
                fprintf(fo, "#EXTINF:-1,%s (%s | %dp)\n", status_list[i].name, status_list[i].resolution,status_list[i].quality);
            } else {
                fprintf(fo, "#EXTINF:-1,%s\n", status_list[i].name);
            }
            fprintf(fo, "%s\n", status_list[i].url);
        }
    }

    // Close files and free memory
    if (status_list) free(status_list);
    fclose(fi);
    fclose(fo);
}

void main()
{
     if (!check_internet()) {
        printf("No internet connection detected. Please check your network and try again.\n");
        return;
    }
    else
    {
        printf("Internet connection detected. Proceeding with scan...\n");
	    // Check the active file
	    Check_Active(Input, Output);
        //Ask user if they want to run VLC the output file? (Optional)
        char choice;
        printf("Do you want to open the output file in VLC? (y/n): ");
        scanf(" %c", &choice);
        if (choice == 'y' || choice == 'Y') {
            #ifdef _WIN32
            system("start vlc.exe output.m3u");
        #else
            system("vlc output.m3u &");
        #endif
        } else {
            printf("Exiting without opening VLC.\n");
        }
    }
}
