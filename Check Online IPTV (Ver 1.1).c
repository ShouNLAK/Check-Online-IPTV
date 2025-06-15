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

    // UI: Print header with colors and timestamp
    time_t now = time(NULL);
    char timebuf[64];
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    printf("\033[1;34m==============================================\033[0m\n");
    printf("\033[1;36m         TV Channel Active Scanner            \033[0m\n");
    printf("\033[1;34m==============================================\033[0m\n");
    printf("\033[1;33m[INFO]\033[0m Started at: %s\n", timebuf);
    printf("\033[1;36m[INFO]\033[0m Scanning \033[1;32m%d\033[0m channels. Please wait...\n\n", total_channels);

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
            // Map string to standard quality if possible
            if (strcasecmp(resolution, "SD") == 0) quality = 480;
            else if (strcasecmp(resolution, "HD") == 0) quality = 720;
            else if (strcasecmp(resolution, "FHD") == 0) quality = 1080;
            else if (strcasecmp(resolution, "QHD") == 0) quality = 1440;
            else if (strcasecmp(resolution, "4K") == 0) quality = 2160;
            else if (strcasecmp(resolution, "8K") == 0) quality = 4320;
            else quality = 0; // Only set to 0 if not matched
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

    // Print summary table with improved UI
    printf("\n\033[1;34m=====================================================================================\033[0m\n");
    printf("\033[1;36m| %-4s | %-40s | %-20s | %-10s |\033[0m\n", "No.", "Channel Name", "Quality/Resolution", "Status");
    printf("\033[1;34m-------------------------------------------------------------------------------------\033[0m\n");
    int idx = 1;
    // Print ACTIVE channels
    for (int i = 0; i < processed; ++i) {
        if (status_list[i].active) {
            char qual_res[32] = "";
            if (status_list[i].quality > 0 && strlen(status_list[i].resolution) > 0) {
                snprintf(qual_res, sizeof(qual_res), "%dp / %-10s", status_list[i].quality, status_list[i].resolution);
            } else if (status_list[i].quality > 0) {
                snprintf(qual_res, sizeof(qual_res), "%dp", status_list[i].quality);
            } else if (strlen(status_list[i].resolution) > 0) {
                snprintf(qual_res, sizeof(qual_res), "%s", status_list[i].resolution);
            }
            printf("| \033[1;32m%-4d\033[0m | %-40.40s | %-20.20s | \033[1;32m%-10s\033[0m |\n", idx++, status_list[i].name, qual_res, "ACTIVE");
        }
    }
    printf("\033[1;34m-------------------------------------------------------------------------------------\033[0m\n");
    // Print INACTIVE channels (including timeout)
    idx = 1;
    for (int i = 0; i < processed; ++i) {
        if (!status_list[i].active) {
            char qual_res[32] = "";
            if (status_list[i].quality > 0 && strlen(status_list[i].resolution) > 0) {
                snprintf(qual_res, sizeof(qual_res), "%dp / %-10s", status_list[i].quality, status_list[i].resolution);
            } else if (status_list[i].quality > 0) {
                snprintf(qual_res, sizeof(qual_res), "%dp", status_list[i].quality);
            } else if (strlen(status_list[i].resolution) > 0) {
                snprintf(qual_res, sizeof(qual_res), "%s", status_list[i].resolution);
            }
            if (status_list[i].timeout) {
                printf("| \033[1;33m%-4d\033[0m | %-40.40s | %-20.20s | \033[1;33m%-10s\033[0m |\n", idx++, status_list[i].name, qual_res, "TIMEOUT");
            } else {
                printf("| \033[1;31m%-4d\033[0m | %-40.40s | %-20.20s | \033[1;31m%-10s\033[0m |\n", idx++, status_list[i].name, qual_res, "INACTIVE");
            }
        }
    }
    printf("\033[1;34m=====================================================================================\033[0m\n");
    printf("Total channels: \033[1;36m%d\033[0m | Active: \033[1;32m%d\033[0m | Inactive: \033[1;31m%d\033[0m | Timeout: \033[1;33m%d\033[0m\n", processed, active_count, inactive_count, timeout_count);
    printf("Total scan time: \033[1;36m%02d:%02d:%02d\033[0m (hh:mm:ss)\n", hours, minutes, seconds);

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

// Function to remove duplicate channels from the output file
void Remove_Duplicates(const char* output) 
{
    FILE* fo = fopen(output, "r");
    if (fo == NULL) {
        printf("\033[1;31m[ERROR]\033[0m Error opening output file: %s\n", output);
        return;
    }

    FILE* temp = fopen("temp.m3u", "w");
    if (temp == NULL) {
        printf("\033[1;31m[ERROR]\033[0m Error creating temporary file.\n");
        fclose(fo);
        return;
    }

    char last_extinf[1024] = "";
    char last_url[1024] = "";
    char curr_extinf[1024] = "";
    char curr_url[1024] = "";
    int has_last = 0;
    int duplicate_count = 0;

    printf("\n\033[1;34m============================================================\033[0m\n");
    printf("\033[1;36m|           Duplicate Channel Removal Process              |\033[0m\n");
    printf("\033[1;34m============================================================\033[0m\n");

    while (fgets(curr_extinf, sizeof(curr_extinf), fo)) {
        if (strncmp(curr_extinf, "#EXTINF", 7) != 0) {
            fputs(curr_extinf, temp);
            continue;
        }
        if (!fgets(curr_url, sizeof(curr_url), fo)) break;

        if (has_last && strcmp(curr_url, last_url) == 0) {
            // Extract channel names for UI summary
            char* curr_name = strchr(curr_extinf, ',');
            char* last_name = strchr(last_extinf, ',');
            char curr_name_buf[256] = "", last_name_buf[256] = "";
            if (curr_name) {
                strncpy(curr_name_buf, curr_name + 1, sizeof(curr_name_buf) - 1);
                curr_name_buf[sizeof(curr_name_buf) - 1] = '\0';
                // Remove trailing newline
                size_t len = strlen(curr_name_buf);
                if (len > 0 && curr_name_buf[len - 1] == '\n') curr_name_buf[len - 1] = '\0';
            }
            if (last_name) {
                strncpy(last_name_buf, last_name + 1, sizeof(last_name_buf) - 1);
                last_name_buf[sizeof(last_name_buf) - 1] = '\0';
                // Remove trailing newline
                size_t len = strlen(last_name_buf);
                if (len > 0 && last_name_buf[len - 1] == '\n') last_name_buf[len - 1] = '\0';
            }
            // Print duplicate info in a compact, clear format
            printf("\033[1;33m[Duplicate]\033[0m \"%s\" <=> \"%s\" [URL: %.*s...]\n",
                   last_name_buf, curr_name_buf, 40, curr_url);
            if (duplicate_count < 5) {
                printf("    \033[1;32mKeeping the higher quality version.\033[0m\n");
            } else if (duplicate_count == 5) {
                printf("    \033[1;36m(More duplicates found, suppressing further details...)\033[0m\n");
            }
            duplicate_count++;

            int curr_quality = 0, last_quality = 0;
            char* curr_p = strstr(curr_extinf, " | ");
            char* last_p = strstr(last_extinf, " | ");
            if (curr_p) {
                curr_p += 3;
                curr_quality = atoi(curr_p);
            }
            if (last_p) {
                last_p += 3;
                last_quality = atoi(last_p);
            }
            if (curr_quality > last_quality) {
                strcpy(last_extinf, curr_extinf);
                strcpy(last_url, curr_url);
            }
        } else {
            if (has_last) {
                fputs(last_extinf, temp);
                fputs(last_url, temp);
            }
            strcpy(last_extinf, curr_extinf);
            strcpy(last_url, curr_url);
            has_last = 1;
        }
    }
    if (has_last) {
        fputs(last_extinf, temp);
        fputs(last_url, temp);
    }

    fclose(fo);
    fclose(temp);

    remove(output);
    rename("temp.m3u", output);

    printf("\033[1;34m------------------------------------------------------------\033[0m\n");
    if (duplicate_count > 0) {
        printf("\033[1;32m[SUMMARY]\033[0m \033[1;33m%d\033[0m duplicate channel(s) removed based on identical URLs.\n", duplicate_count);
        printf("\033[1;36m[RESULT]\033[0m Output file cleaned and optimized for unique channels.\n");
    } else {
        printf("\033[1;32m[SUMMARY]\033[0m No duplicate channels found. Output file is already clean.\n");
    }
    printf("\033[1;34m============================================================\033[0m\n\n");
}

void measure_speed() {
    const char* url = "http://speedtest.tele2.net/1MB.zip"; // Small test file
    const char* outfile = "speedtest_tmp.bin";
    clock_t start = clock();
    int ret = system("curl -s -o speedtest_tmp.bin http://speedtest.tele2.net/1MB.zip");
    clock_t end = clock();
    if (ret == 0) {
        double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
        double mbps = (1.0 * 8) / elapsed; // 1MB file, 8 megabits
        printf("[RESULT] Approximate download speed: %.2f Mbps\n\n", mbps);
    } else {
        printf("[ERROR] curl not found or download failed.\n\n");
    }
    remove(outfile);
}

void main()
{
    int First = 1; // Flag to print error message only once
    // Minimum for user to shrink the VSCode Terminal window / Command Prompt
    printf("------------------------------------------------------------------------------------------------------\n");
    // Print info and wait 10 seconds before starting scan
        printf("\033[1;31m[CAUTION]\033[0m The line above is the minimum size for the terminal window for UI working correctly.");
        fflush(stdout);
        #ifdef _WIN32
            Sleep(10000);
        #else
            sleep(10);
        #endif
        // Reset the line after waiting
        printf("\033[2K\r");
        fflush(stdout);
     while (!check_internet()) {
        time_t now = time(NULL);
    char timecheck[64];
    strftime(timecheck, sizeof(timecheck), "%Y-%m-%d %H:%M:%S", localtime(&now));
    if (First) 
    {
        printf("\033[1;34m=====================================================================================\033[0m\n");
        printf("\033[1;31m[ERROR]\033[0m No internet connection detected. Please check your network and try again.\n");
        printf("\033[1;33m[Re-checking]\033[0m Ensure that your Wi-Fi or Ethernet connection is functioning properly.\nIf you're using a VPN, consider turning it off, as it may cause errors and affect the accuracy of the results.\n");
        printf("\033[1;34m=====================================================================================\033[0m\n");
        First = 0; // Set flag to false after first message
    }
        // Wait for 5 seconds before re-checking the internet connection
        printf("\033[1;33m[Re-Checking]\033[0m Last time checked at: %s", timecheck);
        #ifdef _WIN32
        Sleep(1000);
        #else
        sleep(10);
        #endif
        printf("\033[2K\r");
    }
    printf("\033[F\033[K");printf("\033[F\033[K");printf("\033[F\033[K");printf("\033[F\033[K");printf("\033[F\033[K");printf("\033[F\033[K");
    if (check_internet())  // If internet connection is available
    {
        
        printf("\n\033[1;32m[INFO]\033[0m Internet connection detected. Proceeding with scan...\n\n");
        //Measure internet connection speed (Optional)
        printf("\033[1;34m[CHECK]\033[0m Measuring internet connection speed...\n");
        measure_speed();
        // Print welcome message
        printf("\033[1;34m=============================================\033[0m\n");
        printf("\033[1;34m|         Online IPTV Channel Scanner        |\033[0m\n");
        printf("\033[1;34m|         Version 1.1 - C17 Standard         |\033[0m\n");
        printf("\033[1;34m|           Developed by ShouNLAK            |\033[0m\n");
        printf("\033[1;34m=============================================\033[0m\n");
        // Print info and wait 10 seconds before starting scan
        printf("\033[1;36m[COUNTDOWN]\033[0m The scan will start in 5 seconds...");
        fflush(stdout);
        #ifdef _WIN32
            Sleep(5000);
        #else
            sleep(5);
        #endif
        // Reset the line after waiting
        printf("\033[2K\r");
        fflush(stdout);
        // Check the active file
        printf("\n\033[1;36m[STEP 1]\033[0m Scanning channels for activity...\n");
        Check_Active(Input, Output);

        // Remove any duplicate channels in the output file
        printf("\n\033[1;36m[STEP 2]\033[0m Removing duplicate channels from output file...\n");
        Remove_Duplicates(Output);

        // Print final message
        // Success message
        printf("\n\033[1;34m=====================================================================================\033[0m\n");
        printf("\033[1;32m[SUCCESS]\033[0m \033[1;36mScan complete!\033[0m Active channels saved to \033[1;33m%s\033[0m\n", Output);
        printf("\033[1;36m[INFO]\033[0m You can now use this file with \033[1;32mVLC\033[0m or any other media player that supports \033[1;33mM3U\033[0m playlists.\n");
        printf("\033[1;34m=====================================================================================\033[0m\n");

        // Ask user if they want to run VLC the output file (Optional)
        char choice;
        printf("\n\033[1;33m[OPTION]\033[0m Do you want to open the output file in \033[1;32mVLC\033[0m? (\033[1;32my\033[0m/\033[1;31mn\033[0m): ");
        scanf(" %c", &choice);
        printf("\033[F\033[K");
        if (choice == 'y' || choice == 'Y') {
        #ifdef _WIN32
            printf("\033[1;34m[INFO]\033[0m Launching \033[1;32mVLC\033[0m...\n");
            system("start vlc.exe output.m3u");
        #else
            printf("\033[1;34m[INFO]\033[0m Launching \033[1;32mVLC\033[0m...\n");
            system("vlc output.m3u &");
        #endif
        } else {
            printf("\033[1;34m[INFO]\033[0m Exiting without opening VLC.\n");
            printf("\033[1;34m[INFO]\033[0m You can open the output file manually in VLC or any other media player.\n");
        }
        // Final countdown before exit
        printf("\033[1;36m[COUNTDOWN]\033[0m Exiting in 10 seconds...");
        fflush(stdout);
        #ifdef _WIN32
        Sleep(5000);
        #else
        sleep(50);
        #endif
        printf("\033[2K\r");
        fflush(stdout);
    }
    // Print goodbye message
        printf("\033[1;34m=====================================================================================\033[0m\n");
        printf("\033[1;34m[INFO]\033[0m Thank you for using the Online IPTV Channel Scanner!\n");
        printf("\033[1;34m[INFO]\033[0m Developed by ShouNLAK. Goodbye!\n");
        printf("\033[1;34m=====================================================================================\033[0m\n");
        #ifdef _WIN32
        Sleep(5000);
        #else
        sleep(50);
        #endif
}
