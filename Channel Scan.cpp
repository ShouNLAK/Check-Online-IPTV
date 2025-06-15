#include "Channel_Scan.h"
#include "Log.h"

// Count the number of channels in the input file by counting lines with "#EXTINF"
int count_total_channels(FILE* fi) {
    int count = 0;
    char line[1024];
    while (fgets(line, sizeof(line), fi)) {
        if (strstr(line, "#EXTINF")) {
            count++;
        }
    }
    // Rewind the file pointer so that file can read back to the start.
    rewind(fi);
    // Skip the header line nếu cần.
    fscanf(fi, "%*[^\n]\n");
    return count;
}

// Rollback the fixed URL to the original URL if it contains "/../"
void rollback_to_original_if_needed(const char* original, char* fixed, size_t fixed_size) {
    // Check if the fixed URL contains any '/../' sequences
    if (strstr(fixed, "/../") != NULL) {
        // Copy the original URL back into fixed, ensuring safety.
        strncpy(fixed, original, fixed_size - 1);
        fixed[fixed_size - 1] = '\0';
    }
}

// Read a pair of lines : one with "#EXTINF" and the line after it that contains the URL.
// Returns true if an entry was read, false nếu reached the end of the file.
bool read_channel_entry(FILE* fi, char* extinf_line, size_t extinf_len, char* url_line, size_t url_len) {
    char line[1024];
    bool got_extinf = false;

    // Loop until we reach the end or find both EXTINF and URL.
    while (fgets(line, sizeof(line), fi)) {
        // Remove newline characters.
        size_t len = strcspn(line, "\r\n");
        line[len] = '\0';

        // Skip blank lines or lines that are only white space.
        bool is_blank = true;
        for (size_t i = 0; i < len; i++) {
            if (!isspace((unsigned char)line[i])) {
                is_blank = false;
                break;
            }
        }
        if (is_blank)
            continue;

        // If we haven't yet captured the EXTINF line, look for it.
        if (!got_extinf) {
            // Accept the EXTINF line (it should start with "#EXTINF:")
            if (strncmp(line, "#EXTINF:", 8) == 0) {
                strncpy(extinf_line, line, extinf_len - 1);
                extinf_line[extinf_len - 1] = '\0';
                got_extinf = true;
            }
            // Otherwise skip lines before EXTINF.
            continue;
        }
        else {
            // We already got an EXTINF line, now we need a valid URL.
            // Skip any extraneous comment lines (like #EXTVLCOPT: or other commented URLs)
            if (line[0] == '#')
                continue;
            // Otherwise, treat this line as the URL.
            strncpy(url_line, line, url_len - 1);
            url_line[url_len - 1] = '\0';
            return true; // Successfully read one complete entry.
        }
    }
    return false;  // EOF reached or no complete entry found.
}



// Extracts channel name from a line containing "#EXTINF".
// The function removes leading/trailing spaces and parts in parentheses.
void extract_channel_name(const char* extinf_line, char* out_name, size_t name_size) {
    // Channel name always begin after last comma
    const char* comma = strrchr(extinf_line, ',');
    if (!comma) {
        strncpy(out_name, "Active Channel", name_size - 1);
        out_name[name_size - 1] = '\0';
        return;
    }
    // Skip the comma
    comma++;

	// Copy name after the last comma to out_name
    strncpy(out_name, comma, name_size - 1);
    out_name[name_size - 1] = '\0';

	// Remove any leading spaces
    char* paren = strchr(out_name, '(');
    if (paren) {
        *paren = '\0';
    }

    // Trim leading spaces
    while (*out_name && isspace((unsigned char)*out_name)) {
        memmove(out_name, out_name + 1, strlen(out_name));
    }
    // Trim trailing spaces
    size_t len = strlen(out_name);
    while (len > 0 && isspace((unsigned char)out_name[len - 1])) {
        out_name[len - 1] = '\0';
        len--;
    }
}

// Helper function to convert a resolution string ("WIDTHxHEIGHT") into its area.
int getResolutionArea(const char* resolution) {
    int width = 0, height = 0;
    if (sscanf(resolution, "%dx%d", &width, &height) != 2)
        return 0;
    return width * height;
}

// Function to remove duplicate channels from the output file
void Remove_Duplicates(const char* output) {
    FILE* fo = fopen(output, "r");
    if (fo == NULL) {
        printf(RED "[ERROR]" RESET " Unable to open output file: %s\n", output);
        return;
    }

    FILE* temp = fopen("temp.m3u", "w");
    if (temp == NULL) {
        printf(RED "[ERROR]" RESET " Unable to create temporary file.\n");
        fclose(fo);
        return;
    }

    /* Print header banner for duplicate removal process */
    printf("\n" BLUE "============================================================\n" RESET);
    printf(CYAN "|           Duplicate Channel Removal Process              |\n" RESET);
    printf(BLUE "============================================================\n" RESET);

    char last_extinf[1024] = "";
    char last_url[1024] = "";
    char curr_extinf[1024] = "";
    char curr_url[1024] = "";
    int has_last = 0;
    int duplicate_count = 0;

    while (fgets(curr_extinf, sizeof(curr_extinf), fo)) {
        /* If not an "#EXTINF" line, copy it directly */
        if (strncmp(curr_extinf, "#EXTINF", 7) != 0) {
            fputs(curr_extinf, temp);
            continue;
        }
        /* Next line is expected to be the channel URL */
        if (!fgets(curr_url, sizeof(curr_url), fo))
            break;

        if (has_last && strcmp(curr_url, last_url) == 0) {
            /* Extract channel names from the "#EXTINF" lines */
            char* curr_name = strchr(curr_extinf, ',');
            char* last_name = strchr(last_extinf, ',');
            char curr_name_buf[256] = { 0 };
            char last_name_buf[256] = { 0 };
            if (curr_name) {
                strncpy(curr_name_buf, curr_name + 1, sizeof(curr_name_buf) - 1);
                size_t len = strlen(curr_name_buf);
                if (len > 0 && curr_name_buf[len - 1] == '\n')
                    curr_name_buf[len - 1] = '\0';
            }
            if (last_name) {
                strncpy(last_name_buf, last_name + 1, sizeof(last_name_buf) - 1);
                size_t len = strlen(last_name_buf);
                if (len > 0 && last_name_buf[len - 1] == '\n')
                    last_name_buf[len - 1] = '\0';
            }
            /* Report duplicate found by displaying channel names only */
            printf(YELLOW "[Duplicate]" RESET " \"%s\" <=> \"%s\"\n", last_name_buf, curr_name_buf);
            if (duplicate_count < 5) {
                printf("    " GREEN "Keeping the higher quality version." RESET "\n");
            }
            else if (duplicate_count == 5) {
                printf("    " CYAN "(More duplicates found, suppressing additional details...)" RESET "\n");
            }
            duplicate_count++;

            /* Compare quality values if available (assumed in the format " | <quality>") */
            int curr_quality = 0, last_quality = 0;
            char* curr_ptr = strstr(curr_extinf, " | ");
            char* last_ptr = strstr(last_extinf, " | ");
            if (curr_ptr) {
                curr_ptr += 3;  // Skip " | "
                curr_quality = atoi(curr_ptr);
            }
            if (last_ptr) {
                last_ptr += 3;
                last_quality = atoi(last_ptr);
            }
            if (curr_quality > last_quality) {
                strcpy(last_extinf, curr_extinf);
                strcpy(last_url, curr_url);
            }
        }
        else {
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

    /* Print summary information */
    printf(BLUE "------------------------------------------------------------\n" RESET);
    if (duplicate_count > 0) {
        printf(GREEN "[SUMMARY]" RESET " " YELLOW "%d" RESET " duplicate channel(s) removed based on identical URLs.\n", duplicate_count);
        printf(CYAN "[RESULT]" RESET " Output file cleaned and optimized for unique channels.\n");
    }
    else {
        printf(GREEN "[SUMMARY]" RESET " No duplicate channels found. Output file is already clean.\n");
    }
    printf(BLUE "============================================================\n\n" RESET);
}


int test_channel_stream(char* url, char* resolved_url, size_t resolved_size,
    char* fetched_resolution, size_t reso_size,
    int* is_timeout, int* is_ts)
{
    // Variables and flags
    int is_active = 0;
    int status_code = 0;
    int new_status_code = 0;
    int got_response = 0;
    int is_download = 0; // Assume not downloading unless set elsewhere
    char command[1024] = { 0 };
    char resp[256] = { 0 };

    // Initialize output flags.
    *is_timeout = 0;
    *is_ts = 0;

    // Process only if URL begins with http:// or https://
    if (strstr(url, "http://") || strstr(url, "https://"))
    {
        // --- Step 1: Get HTTP code for the first URL ---
        // Command: curl -I -s -o /dev/null -w "%{http_code}\n" <first URL>
        snprintf(command, sizeof(command),
            "curl -I -s -o /dev/null -w \"%%{http_code}\\n\" \"%s\"", url);
        FILE* fp = _popen(command, "r");
        if (fp)
        {
            if (fgets(resp, sizeof(resp), fp))
            {
                status_code = atoi(resp);
                got_response = 1;
            }
            _pclose(fp);
        }

        // --- Step 2: If response code is 302, follow redirect ---
        if (status_code == 302)
        {
            char newUrl[1024] = { 0 };
            int found_location = 0;

            // First, try to get the "Location:" header using:
            // curl -I -s "<URL>" | findstr -i "Location:"
            snprintf(command, sizeof(command),
                "curl -I -s \"%s\" | findstr -i \"Location:\"",
                url);
            fp = _popen(command, "r");
            if (fp)
            {
                if (fgets(newUrl, sizeof(newUrl), fp))
                {
                    // The output might be a line like "Location: http://example.com/redirect"
                    char* loc = strstr(newUrl, "Location:");
                    if (loc != NULL)
                    {
                        loc += strlen("Location:");
                        // Skip any leading whitespace.
                        while (*loc == ' ' || *loc == '\t')
                            loc++;
                        // Copy the remainder of the line into newUrl.
                        strncpy(newUrl, loc, sizeof(newUrl) - 1);
                        newUrl[sizeof(newUrl) - 1] = '\0';
                        // Trim any trailing newline or carriage return.
                        newUrl[strcspn(newUrl, "\r\n")] = '\0';
                        found_location = 1;
                    }
                }
                _pclose(fp);
            }

            // If no valid "Location:" was found, fallback to use the effective URL method.
            if (!found_location || strlen(newUrl) == 0)
            {
                // Get effective URL:
                // Command: curl -L -s -o /dev/null -w "%{url_effective}\n" <first URL>
                snprintf(command, sizeof(command),
                    "curl -L -s -o /dev/null -w \"%%{url_effective}\\n\" \"%s\"", url);
                fp = _popen(command, "r");
                if (fp)
                {
                    if (fgets(newUrl, sizeof(newUrl), fp))
                    {
                        newUrl[strcspn(newUrl, "\r\n")] = '\0';
                    }
                    _pclose(fp);
                }
            }

            // Check new URL response code:
            // Command: curl -I -s -o /dev/null -w "%{http_code}\n" <newUrl>
            snprintf(command, sizeof(command),
                "curl -I -s -o /dev/null -w \"%%{http_code}\\n\" \"%s\"", newUrl);
            fp = _popen(command, "r");
            if (fp)
            {
                char newResp[256] = { 0 };
                if (fgets(newResp, sizeof(newResp), fp))
                    new_status_code = atoi(newResp);
                _pclose(fp);
            }

            // If new response code is in the 4XX/5XX range (except 405), mark as inactive.
            if ((new_status_code >= 400 && new_status_code < 600) && (new_status_code != 405))
            {
                is_active = 0;
                return is_active;
            }
            else
            {
                // Otherwise, update resolved_url to newUrl, set is_ts flag, and mark active.
                strncpy(resolved_url, newUrl, resolved_size - 1);
                resolved_url[resolved_size - 1] = '\0';
                strncpy(url, newUrl, strlen(newUrl) + 1);
                *is_ts = 1;
                is_active = 1;
                return is_active;
            }
        }

        // --- Step 3: If first URL returned a valid code (1XX-3XX or 405) ---
        else if (got_response && ((status_code >= 100 && status_code < 500)))
        {
            // Process the playlist to fetch available resolutions and candidate stream URLs.
            StreamInfo streams[100] = { 0 };
            int stream_count = 0;
            int pass = 0;
            char playlist_cmd[1024] = { 0 };
            // Command: curl -L -s -- "<first URL>" to retrieve the playlist.
            snprintf(playlist_cmd, sizeof(playlist_cmd), "curl -L -s -- \"%s\"", url);
            FILE* fp_playlist = _popen(playlist_cmd, "r");
            if (fp_playlist == NULL)
            {
                perror("Failed to open pipe for playlist");
                return 1;
            }
            char playlist_line[1024] = { 0 };
            char last_resolution[64] = { 0 };
            while (fgets(playlist_line, sizeof(playlist_line), fp_playlist) != NULL)
            {
                pass = 1;
                // Remove trailing newline characters.
                playlist_line[strcspn(playlist_line, "\r\n")] = '\0';

                // Look for resolution lines.
                char* res_ptr = strstr(playlist_line, "RESOLUTION=");
                if (res_ptr)
                {
                    int w = 0, h = 0;
                    res_ptr += strlen("RESOLUTION=");
                    if (sscanf(res_ptr, "%dx%d", &w, &h) == 2)
                    {
                        snprintf(last_resolution, sizeof(last_resolution), "%dx%d", w, h);
                    }
                    else
                    {
                        last_resolution[0] = '\0';
                    }
                }
                // When a media URL is encountered immediately after a resolution line.
                else if (strlen(last_resolution) > 0)
                {
                    if (stream_count < 100)
                    {
                        // Copy the resolution value.
                        strncpy(streams[stream_count].resolution, last_resolution, sizeof(streams[stream_count].resolution) - 1);
                        streams[stream_count].resolution[sizeof(streams[stream_count].resolution) - 1] = '\0';

                        // Use snprintf to safely copy playlist_line (the URL) into stream_url.
                        snprintf(streams[stream_count].stream_url, sizeof(streams[stream_count].stream_url),
                            "%s", playlist_line);

                        int w = 0, h = 0;
                        sscanf(last_resolution, "%dx%d", &w, &h);
                        streams[stream_count].area = w * h;
                        stream_count++;
                    }
                    last_resolution[0] = '\0';
                }
            }
            _pclose(fp_playlist);
            _pclose(fp_playlist);

            // If no candidate stream was found but playlist was processed, perform an extra HEAD check.
            if (stream_count == 0 && pass)
            {
                char head_cmd[512] = { 0 };
                char head_resp[256] = { 0 };
                int final_code = 0;
                snprintf(head_cmd, sizeof(head_cmd),
                    "curl -I -s -o /dev/null -w \"%%{http_code}\\n\" \"%s\"", url);
                FILE* head_fp = _popen(head_cmd, "r");
                if (head_fp)
                {
                    if (fgets(head_resp, sizeof(head_resp), head_fp))
                    {
                        final_code = atoi(head_resp);
                    }
                    _pclose(head_fp);
                }
                is_active = (final_code == 200) ? 1 : 0;
            }
            else
            {
                // Evaluate candidate streams in descending order of resolution (highest first)
                qsort(streams, stream_count, sizeof(StreamInfo), cmp_streams);
                int candidate_found = 0;
                for (int i = 0; i < stream_count && !candidate_found; i++)
                {
                    char candidate_url[1024] = { 0 };
                    // If candidate URL is relative, prepend the base URL from the first URL.
                    if (strncmp(streams[i].stream_url, "http", 4) != 0)
                    {
                        char base_url[1024] = { 0 };
                        char* last_slash = strrchr(url, '/');
                        if (last_slash)
                        {
                            int len = last_slash - url + 1;
                            strncpy(base_url, url, len);
                            base_url[len] = '\0';
                        }
                        snprintf(candidate_url, sizeof(candidate_url), "%s%s", base_url, streams[i].stream_url);
                    }
                    else
                    {
                        strncpy(candidate_url, streams[i].stream_url, sizeof(candidate_url) - 1);
                        candidate_url[sizeof(candidate_url) - 1] = '\0';
                    }

                    // Check candidate URL using:
                    // curl -L -s -o /dev/null -w "%{http_code}\n" <candidate URL>
                    char test_cmd[1024] = { 0 };
                    char test_resp[256] = { 0 };
                    int candidate_active = 0;
                    snprintf(test_cmd, sizeof(test_cmd),
                        "curl -L -s -o /dev/null -w \"%%{http_code}\\n\" \"%s\"", candidate_url);
                    FILE* fp_test = _popen(test_cmd, "r");
                    if (fp_test)
                    {
                        if (fgets(test_resp, sizeof(test_resp), fp_test))
                        {
                            int code = atoi(test_resp);
                            if (code >= 200 && code < 400)
                                candidate_active = 1;
                        }
                        _pclose(fp_test);
                    }
                    // If candidate is active, update fetched_resolution and URL.
                    if (candidate_active)
                    {
                        strcpy(fetched_resolution, streams[i].resolution);
                        strcpy(url, candidate_url);
                        candidate_found = 1;
                    }
                }
                is_active = candidate_found ? 1 : 0;
            }
        }
        else
        {
            is_active = 0;
        }
    }
    else if (!(*is_ts))
    {
        is_active = 0;
    }

    return is_active;
}

void store_channel_status(ChannelStatus* status,
    const char* name,
    const char* url,
    const char* resolution,
    int active,
    int timeout) {
    strncpy(status->name, name, sizeof(status->name) - 1);
    status->name[sizeof(status->name) - 1] = '\0';
    strncpy(status->url, url, sizeof(status->url) - 1);
    status->url[sizeof(status->url) - 1] = '\0';
    if (resolution)
        strncpy(status->resolution, resolution, sizeof(status->resolution) - 1);
    else
        status->resolution[0] = '\0';
    status->active = active;
    status->timeout = timeout;
}

void Check_Active(const char* input, const char* output) {
    // Reset the log file at the beginning.
    initialize_log_file("log.txt");

    FILE* fi = fopen(input, "r");
    if (!fi) {
        printf("Error opening input file.\n");
        return;
    }
    FILE* fo = fopen(output, "w");
    if (!fo) {
        printf("Error opening output file.\n");
        fclose(fi);
        return;
    }

    clock_t start_time = clock();

    // Count total channels in the file.
    int total_channels = count_total_channels(fi);

    ChannelStatus* status_list = (ChannelStatus*)malloc(total_channels * sizeof(ChannelStatus));
    if (!status_list) {
        printf("Memory allocation failed.\n");
        fclose(fi);
        fclose(fo);
        return;
    }

    // Initialize spinner and progress bar variables.
    char scan_chars[] = "|/-\\";
    int scan_idx = 0;
    int processed = 0;

    // Temporary buffers for channel entry.
    char extinf_line[1024];
    char url_line[1024];
    char channel_name[256];
    char final_url[1024];      // This will be updated by test_channel_stream.
    char original_url[1024];   // Preserve the original URL from the file.
    char fetched_resolution[128] = { 0 };

    clock_t start_clock = clock();
    int eta_h = 0, eta_m = 0, eta_s = 0;

    while (read_channel_entry(fi, extinf_line, sizeof(extinf_line), url_line, sizeof(url_line))) {
        // Extract channel name from #EXTINF line.
        extract_channel_name(extinf_line, channel_name, sizeof(channel_name));

        // Save the original URL from the file.
        strncpy(original_url, url_line, sizeof(original_url) - 1);
        original_url[sizeof(original_url) - 1] = '\0';

        // Copy the original URL into final_url (this may be updated by test_channel_stream).
        strncpy(final_url, url_line, sizeof(final_url) - 1);
        final_url[sizeof(final_url) - 1] = '\0';

        // Flags for timeouts and stream type.
        int is_timeout = 0;
        int is_ts = 0;

        // Update progress bar and spinner before testing each channel.
        processed++;
        update_progress_bar(processed, total_channels, channel_name, scan_chars[scan_idx], eta_h, eta_m, eta_s);
        scan_idx = (scan_idx + 1) % 4;

        // Test the channel stream (this function may update final_url and fetched_resolution).
        int active = test_channel_stream(final_url, final_url, sizeof(final_url),
            fetched_resolution, sizeof(fetched_resolution),
            &is_timeout, &is_ts);

        // ---- ETA Calculations ----
        double elapsed_sec = (double)(clock() - start_clock) / CLOCKS_PER_SEC;
        double avg_per_channel = elapsed_sec / processed;
        double remaining_sec = avg_per_channel * (total_channels - processed);
        eta_h = (int)(remaining_sec / 3600);
        eta_m = ((int)remaining_sec % 3600) / 60;
        eta_s = (int)remaining_sec % 60;

        // -----------------------------------------------------------------------
        // Simulate response codes for demonstration.
        // In a refined version, you would extract real response codes.
        int initial_response = 200;   // Default simulated response.
        int final_response = active ? 200 : 404;
        if (is_ts) {
            // If test_channel_stream flagged a redirect (is_ts is TRUE),
            // simulate an initial redirection (302) that resolved to 200.
            initial_response = 302;
            final_response = 200;
        }

        // Build a string for the Respond Code.
        char respond_code[32];
        if (initial_response == 302)
            snprintf(respond_code, sizeof(respond_code), "302 -> 200");
        else
            snprintf(respond_code, sizeof(respond_code), "%d", initial_response);

        // Determine the Status string for logging.
        const char* status_str = NULL;
        if (initial_response == 302)
            status_str = "Redirected";
        else if (active)
            status_str = "Live";
        else
            status_str = "Inactive";

        // Rollback if there's any '/../' present.
        rollback_to_original_if_needed(original_url, final_url, sizeof(final_url));


        // ---- Logging Integration ----
        // The logging function is defined as:
        // write_log_line(const char *log_filename,
        //                const char *channel_name,
        //                const char *status,
        //                const char *respond_code,
        //                const char *url,
        //                const char *fixed_url);
        write_log_line("log.txt",
            /* channel_name */ channel_name,  // e.g., "CBS News 24/7"
            /* status       */ status_str,    // "Redirected", "Live", or "Inactive"
            /* resolution  */ fetched_resolution,  // e.g., "1920 x 1080" ...
            /* url          */ original_url,  // Original URL from the playlist/file
            /* fixed_url    */ final_url);    // Updated URL after processing.
        // ---- End Logging Integration ----

        // Save channel status for later processing (summary & duplicate removal).
        store_channel_status(&status_list[processed - 1], channel_name, final_url,
            fetched_resolution, active, is_timeout);
    }

    // Sorting and writing summary after all channels are processed.
    sortStatusList(status_list, processed);

    clock_t end_time = clock();
    double elapsed_total = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    print_scan_summary(status_list, processed, elapsed_total);

    // Write the active channels to the output file.
    fprintf(fo, "#EXTM3U\n");
    for (int i = 0; i < processed; i++) {
        if (status_list[i].active) {
            if (strlen(status_list[i].resolution) > 0)
                fprintf(fo, "#EXTINF:-1,%s (%s)\n", status_list[i].name, status_list[i].resolution);
            else
                fprintf(fo, "#EXTINF:-1,%s\n", status_list[i].name);
            fprintf(fo, "%s\n", status_list[i].url);
        }
    }

    free(status_list);
    fclose(fi);
    fclose(fo);
}
