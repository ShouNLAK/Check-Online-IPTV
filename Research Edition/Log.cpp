#include "log.h"
#include "System.h"

// Initializes the log file by clearing it and writing an ASCII header.
void initialize_log_file(const char* log_filename) {
    FILE* fp = fopen(log_filename, "w");
    if (fp) {
        // Print header (no trailing "|" for URL since it is variable length)
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
        perror("Error opening log file in initialize_log_file");
    }
}

// Writes a log entry in two rows.  
// Main row displays: Time | Channel Name | Status | Resolution | URL.
// Arrow row displays:
//   - If active: "   ---> " followed by Fixed_URL if it differs, or URL if they are the same.
//   - If inactive: "   ---> Unable to reach"
void write_log_line(const char* log_filename,
    const char* channel_name,
    const char* status,
    const char* resolution,
    const char* url,
    const char* fixed_url)
{
    FILE* fp = fopen(log_filename, "a");
    if (!fp) {
        perror("Error opening log file in write_log_line");
        return;
    }

    // Obtain the current local time.
    time_t now = time(NULL);
    struct tm* local = localtime(&now);
    char time_str[20];  // Format: "YYYY-MM-DD HH:MM:SS"
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local);

    // Print the main row.
    fprintf(fp, "%-20s | %-30s | %-12s | %-13s | %s\n",
        time_str, channel_name, status, resolution, url);

    // Prepare the arrow row.
    char arrow_text[1024];
    if (strcmp(status, "Inactive") == 0) {
        // For inactive channels, always point to "Unable to reach".
        snprintf(arrow_text, sizeof(arrow_text), "   ---> Unable to reach");
    }
    else {
        // For active channels, if fixed URL differs from the original, show it.
        if (strcmp(url, fixed_url) != 0) {
            snprintf(arrow_text, sizeof(arrow_text), "   ---> %s", fixed_url);
        }
        else {
            // If both URLs are the same, point to the original URL for confirmation.
            snprintf(arrow_text, sizeof(arrow_text), "   ---> %s", url);
        }
    }

    // Print the arrow row (leaving the first four fixed-width cells blank).
    fprintf(fp, "%-20s | %-30s | %-12s | %-13s | %s\n",
        "", "", "", "", arrow_text);

    // Print a separator line after the entry.
    fprintf(fp, "---------------------------------------------------------------------------------------------\n");

    fclose(fp);
#ifdef _WIN32
    Sleep(500);
#else
    usleep(500 * 1000);
#endif
}
