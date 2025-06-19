#include "UI.h"


void print_banner(const char* title, const char* color) {
    const int width = 60; // adjust to fit your terminal width
    // Top border
    printf("%s+%.*s+%s\n", color, width, "------------------------------------------------------------", RESET);
    // Centered title
    int pad = (width - (int)strlen(title)) / 2;
    printf("%s|%*s%s%*s|%s\n", color, pad, "", title, width - pad - (int)strlen(title), "", RESET);
    // Bottom border
    printf("%s+%.*s+%s\n", color, width, "------------------------------------------------------------", RESET);
}


void countdown(const char* message, int seconds) {
    for (int i = seconds; i > 0; i--) {
        printf("\r" CYAN "%s" RESET " %d second%s remaining...", message, i, (i == 1 ? "" : "s"));
        fflush(stdout);
        msleep(1000);
    }
    // Clear the countdown line
    printf("\r\033[2K\r");
}

int wcwidth(wchar_t ucs) {
    // Return 0 for NULL character.
    if (ucs == 0) return 0;
    // Control characters: return -1 (non printable)
    if (ucs < 32 || (ucs >= 0x7F && ucs < 0xA0))
        return -1;

    // Check for wide characters in common East Asian ranges.
    if (ucs >= 0x1100) {
        if ((ucs >= 0x1100 && ucs <= 0x115F) || /* Hangul Jamo init. consonants */
            (ucs == 0x2329 || ucs == 0x232A) ||
            (ucs >= 0x2E80 && ucs <= 0xA4CF && ucs != 0x303F) ||
            (ucs >= 0xAC00 && ucs <= 0xD7A3) ||
            (ucs >= 0xF900 && ucs <= 0xFAFF) ||
            (ucs >= 0xFE10 && ucs <= 0xFE19) ||
            (ucs >= 0xFE30 && ucs <= 0xFE6F) ||
            (ucs >= 0xFF00 && ucs <= 0xFF60) ||
            (ucs >= 0xFFE0 && ucs <= 0xFFE6))
        {
            return 2;
        }
    }
    return 1;
}

// Function to calculate the display width of a UTF-8 string
int utf8_display_width(const char* str) {
    setlocale(LC_ALL, "");
    int width = 0;
    mbstate_t state;
    memset(&state, 0, sizeof state);
    wchar_t wc;
    while (*str) {
        int len = mbrtowc(&wc, str, MB_CUR_MAX, &state);
        if (len > 0) {
            width += wcwidth(wc);
            str += len;
        }
        else {
            break;
        }
    }
    return width;
}

// Function to print a UTF-8 string with a fixed display width
void print_utf8_fixed_width(const char* str, int width) {
    int display_width = utf8_display_width(str);
    printf("%s", str);
    for (int i = display_width; i < width; ++i) {
        printf(" ");
    }
}


void update_progress_bar(int current, int total, const char* channel_name, char spinner, int eta_hours, int eta_minutes, int eta_secs) {
    int barWidth = 30;
    double progress = (double)current / total;
    int pos = (int)(barWidth * progress);

    // Clear the current line and begin printing the progress bar
    printf("\033[2K\r" CYAN "Scanning " RESET "[ " YELLOW "%c" RESET " ] "
        BLUE "%3d" RESET "/" BLUE "%d" RESET ": " WHITE "%-30.30s" RESET " | [",
        spinner, current, total, channel_name);

    // Draw the bar: completed progress is shown with '=' and current position with '>'
    for (int i = 0; i < barWidth; i++) {
        if (i < pos) {
            printf(GREEN "=" RESET);
        }
        else if (i == pos) {
            printf(RED ">" RESET);
        }
        else {
            printf(" ");
        }
    }

    // Print the percentage and estimated time remaining.
    printf("]" CYAN "%3d%%" RESET " | " MAGENTA "ETA: %02d:%02d:%02d" RESET,
        (int)(progress * 100), eta_hours, eta_minutes, eta_secs);
    fflush(stdout);
}

void print_scan_summary(ChannelStatus* status_list, int processed, double elapsed_sec) {
    int active_count = 0, inactive_count = 0, timeout_count = 0;
    int i, idx;

    // Count the number of active, inactive, and timeout channels.
    for (i = 0; i < processed; i++) {
        if (status_list[i].active)
            active_count++;
        else {
            inactive_count++;
            if (status_list[i].timeout)
                timeout_count++;
        }
    }

    // Print the table header.
    printf("\n" BLUE "=====================================================================================\n" RESET);
    printf("| " WHITE "%-4s" RESET " | " WHITE "%-40s" RESET " | " WHITE "%-20s" RESET " | " WHITE "%-10s" RESET " |\n",
        "No.", "Channel Name", "Resolution", "Status");
    printf(BLUE "-------------------------------------------------------------------------------------\n" RESET);

    // Print Active channels first.
    idx = 1;
    for (i = 0; i < processed; i++) {
        if (status_list[i].active) {
            char res_label[32] = "";
            if (strlen(status_list[i].resolution) > 0) {
                // Convert resolution (e.g., "1280x720") into a standard label such as "720p"
                convertResolutionToLabel(status_list[i].resolution, res_label, sizeof(res_label));
            }
            printf("| " CYAN "%-4d" RESET " | ", idx++);
            // Use a helper to print UTF-8 strings with fixed display width.
            print_utf8_fixed_width(status_list[i].name, 40);
            printf(" | " WHITE "%-20.20s" RESET " | " GREEN "%-10s" RESET " |\n", res_label, "ACTIVE");
			strcpy(status_list[i].resolution, res_label); // Update resolution in the status list
        }
    }

    // Print Inactive channels.
    printf(BLUE "-------------------------------------------------------------------------------------\n" RESET);
    idx = 1;
    for (i = 0; i < processed; i++) {
        if (!status_list[i].active) {
            char res_label[32] = "";
            if (strlen(status_list[i].resolution) > 0) {
                convertResolutionToLabel(status_list[i].resolution, res_label, sizeof(res_label));
            }
            printf("| " CYAN "%-4d" RESET " | ", idx++);
            print_utf8_fixed_width(status_list[i].name, 40);
            printf(" | " WHITE "%-20.20s" RESET " | ", res_label);
            if (status_list[i].timeout)
                printf(YELLOW "%-10s" RESET " |\n", "TIMEOUT");
            else
                printf(RED "%-10s" RESET " |\n", "INACTIVE");
        }
    }

    printf(BLUE "=====================================================================================\n" RESET);

    // Print summary statistics.
    printf("\nTotal channels: " WHITE "%d" RESET " | Active: " GREEN "%d" RESET " | Inactive: " RED "%d" RESET " | Timeout: " YELLOW "%d" RESET "\n",
        processed, active_count, inactive_count, timeout_count);

    // Convert elapsed time to HH:MM:SS format.
    int tot_sec = (int)elapsed_sec;
    int hours = tot_sec / 3600;
    int minutes = (tot_sec % 3600) / 60;
    int seconds = tot_sec % 60;
    printf("Total scan time: " CYAN "%02d:%02d:%02d" RESET " (hh:mm:ss)\n", hours, minutes, seconds);
}

int ask_concurrency(int max_threads) {
    // ASCII UI banner
    printf(BLUE "+------------------------------------------------------------+\n" RESET);
    printf(BLUE "|                  " CYAN "CHANNEL SCAN CONCURRENCY" BLUE "                  |\n" RESET);      
    printf(BLUE "+------------------------------------------------------------+\n" RESET);
    printf("|  " YELLOW "How many channels do you want to scan at the same time?" RESET "   |\n");
    printf("|  " WHITE "Recommended: " GREEN "%d" WHITE " (number of CPU cores)" RESET "                      |\n", max_threads);
    printf("|  " CYAN "Enter a value between " YELLOW "1" CYAN " and " YELLOW "%d" CYAN " and press Enter." RESET "            |\n", max_threads);
    printf(BLUE "+------------------------------------------------------------+\n" RESET);
    printf("" CYAN "Concurrency" RESET " [" YELLOW "1-%d" RESET "]: ", max_threads);
    fflush(stdout);
    int user_threads = 0;
    while (1) {
        fflush(stdout);
        char input[32];
        if (!fgets(input, sizeof(input), stdin)) {
            // Input error, try again
            printf("\033[F\033[K");
            printf(RED "[ERROR]" RESET " Please enter a number between 1 and %d: ", max_threads);
            continue;
        }
        // Check if input is just a newline (user pressed Enter)
        if (input[0] == '\n') {
            printf("\033[F\033[K");
            printf(CYAN "Please enter a number between 1 and %d: ", max_threads);
            continue;
        }
        // Try to parse the number
        if (sscanf(input, "%d", &user_threads) != 1) {
            printf("\033[F\033[K");
            printf(RED "[ERROR]" RESET " Please enter a number between 1 and %d: ", max_threads);
            continue;
        }
        if (user_threads >= 1 && user_threads <= max_threads) {
            break;
        } else {
            printf("\033[F\033[K");
            printf(RED "[ERROR]" RESET " Please enter a number between 1 and %d: ", max_threads);
        }
    }
    for (int i = 0; i < 5; i++) {
        printf("\033[F\033[K");
    }
    printf(GREEN "[CONFIRM]" RESET " Using " YELLOW "%d" RESET " concurrent scan(s).\n\n", user_threads);
    return user_threads;
}
