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

//Colors for terminal output
#define RESET  "\033[0m"
#define BOLD   "\033[1m"
#define UNDERLINE "\033[4m"
#define ITALIC "\033[3m"
#define RED    "\033[1;31m"
#define GREEN  "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE   "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN   "\033[1;36m"
#define WHITE  "\033[1;37m"

// Channel information
typedef struct {
    char name[256];
    char url[1024];
    char resolution[128]; // resolution info if available
    int active;
    int timeout;
} ChannelStatus;


#ifdef _WIN32
    #define POPEN(cmd, mode) _popen(cmd, mode)
    #define PCLOSE(fp)       _pclose(fp)
#else
    #define POPEN(cmd, mode) popen(cmd, mode)
    #define PCLOSE(fp)       pclose(fp)
#endif

void msleep(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

void countdown(const char *message, int seconds) {
    for (int i = seconds; i > 0; i--) {
        printf("\r" CYAN "%s" RESET " %d second%s remaining...", message, i, (i == 1 ? "" : "s"));
        fflush(stdout);
        msleep(1000);
    }
    // Clear the countdown line
    printf("\r\033[2K\r");
}

/* ─── HELPERS ─────────────────────────────────────────────────────── */
static void print_banner(const char *title, const char *color) {
    const int width = 60; // adjust to fit your terminal width
    // Top border
    printf("%s+%.*s+%s\n", color, width, "------------------------------------------------------------", RESET);
    // Centered title
    int pad = (width - (int)strlen(title)) / 2;
    printf("%s|%*s%s%*s|%s\n", color, pad, "", title, width - pad - (int)strlen(title), "", RESET);
    // Bottom border
    printf("%s+%.*s+%s\n", color, width, "------------------------------------------------------------", RESET);
}


typedef struct {
    char code[3];   // 2 characters for code + null terminator
    char name[100];  // Sufficient space for most country names
} Country;
Country worldCountries[] = {
    {"AF", "Afghanistan"}, {"AX", "Åland Islands"}, {"AL", "Albania"}, {"DZ", "Algeria"},
    {"AS", "American Samoa"}, {"AD", "Andorra"}, {"AO", "Angola"}, {"AI", "Anguilla"},
    {"AQ", "Antarctica"}, {"AG", "Antigua and Barbuda"}, {"AR", "Argentina"}, {"AM", "Armenia"},
    {"AW", "Aruba"}, {"AU", "Australia"}, {"AT", "Austria"}, {"AZ", "Azerbaijan"},
    {"BS", "Bahamas"}, {"BH", "Bahrain"}, {"BD", "Bangladesh"}, {"BB", "Barbados"},
    {"BY", "Belarus"}, {"BE", "Belgium"}, {"BZ", "Belize"}, {"BJ", "Benin"},
    {"BM", "Bermuda"}, {"BT", "Bhutan"}, {"BO", "Bolivia (Plurinational State of)"},
    {"BQ", "Bonaire, Sint Eustatius and Saba"}, {"BA", "Bosnia and Herzegovina"}, {"BW", "Botswana"},
    {"BV", "Bouvet Island"}, {"BR", "Brazil"}, {"IO", "British Indian Ocean Territory"},
    {"BN", "Brunei Darussalam"}, {"BG", "Bulgaria"}, {"BF", "Burkina Faso"}, {"BI", "Burundi"},
    {"CV", "Cabo Verde"}, {"KH", "Cambodia"}, {"CM", "Cameroon"}, {"CA", "Canada"},
    {"KY", "Cayman Islands"}, {"CF", "Central African Republic"}, {"TD", "Chad"}, {"CL", "Chile"},
    {"CN", "China"}, {"CX", "Christmas Island"}, {"CC", "Cocos (Keeling) Islands"},
    {"CO", "Colombia"}, {"KM", "Comoros"}, {"CD", "Congo, Democratic Republic of the"},
    {"CG", "Congo"}, {"CK", "Cook Islands"}, {"CR", "Costa Rica"}, {"CI", "Côte d'Ivoire"},
    {"HR", "Croatia"}, {"CU", "Cuba"}, {"CW", "Curaçao"}, {"CY", "Cyprus"},
    {"CZ", "Czechia"}, {"DK", "Denmark"}, {"DJ", "Djibouti"}, {"DM", "Dominica"},
    {"DO", "Dominican Republic"}, {"EC", "Ecuador"}, {"EG", "Egypt"}, {"SV", "El Salvador"},
    {"GQ", "Equatorial Guinea"}, {"ER", "Eritrea"}, {"EE", "Estonia"}, {"SZ", "Eswatini"},
    {"ET", "Ethiopia"}, {"FK", "Falkland Islands (Malvinas)"}, {"FO", "Faroe Islands"},
    {"FJ", "Fiji"}, {"FI", "Finland"}, {"FR", "France"}, {"GF", "French Guiana"},
    {"PF", "French Polynesia"}, {"TF", "French Southern Territories"}, {"GA", "Gabon"},
    {"GM", "Gambia"}, {"GE", "Georgia"}, {"DE", "Germany"}, {"GH", "Ghana"},
    {"GI", "Gibraltar"}, {"GR", "Greece"}, {"GL", "Greenland"}, {"GD", "Grenada"},
    {"GP", "Guadeloupe"}, {"GU", "Guam"}, {"GT", "Guatemala"}, {"GG", "Guernsey"},
    {"GN", "Guinea"}, {"GW", "Guinea-Bissau"}, {"GY", "Guyana"}, {"HT", "Haiti"},
    {"HM", "Heard Island and McDonald Islands"}, {"VA", "Holy See"}, {"HN", "Honduras"},
    {"HK", "Hong Kong"}, {"HU", "Hungary"}, {"IS", "Iceland"}, {"IN", "India"},
    {"ID", "Indonesia"}, {"IR", "Iran (Islamic Republic of)"}, {"IQ", "Iraq"}, {"IE", "Ireland"},
    {"IM", "Isle of Man"}, {"IL", "Israel"}, {"IT", "Italy"}, {"JM", "Jamaica"},
    {"JP", "Japan"}, {"JE", "Jersey"}, {"JO", "Jordan"}, {"KZ", "Kazakhstan"},
    {"KE", "Kenya"}, {"KI", "Kiribati"}, {"KP", "Korea (Democratic People's Republic of)"},
    {"KR", "Korea (Republic of)"}, {"KW", "Kuwait"}, {"KG", "Kyrgyzstan"},
    {"LA", "Lao People's Democratic Republic"}, {"LV", "Latvia"}, {"LB", "Lebanon"},
    {"LS", "Lesotho"}, {"LR", "Liberia"}, {"LY", "Libya"}, {"LI", "Liechtenstein"},
    {"LT", "Lithuania"}, {"LU", "Luxembourg"}, {"MO", "Macao"}, {"MG", "Madagascar"},
    {"MW", "Malawi"}, {"MY", "Malaysia"}, {"MV", "Maldives"}, {"ML", "Mali"},
    {"MT", "Malta"}, {"MH", "Marshall Islands"}, {"MQ", "Martinique"}, {"MR", "Mauritania"},
    {"MU", "Mauritius"}, {"YT", "Mayotte"}, {"MX", "Mexico"},
    {"FM", "Micronesia (Federated States of)"}, {"MD", "Moldova (Republic of)"},
    {"MC", "Monaco"}, {"MN", "Mongolia"}, {"ME", "Montenegro"}, {"MS", "Montserrat"},
    {"MA", "Morocco"}, {"MZ", "Mozambique"}, {"MM", "Myanmar"}, {"NA", "Namibia"},
    {"NR", "Nauru"}, {"NP", "Nepal"}, {"NL", "Netherlands"}, {"NC", "New Caledonia"},
    {"NZ", "New Zealand"}, {"NI", "Nicaragua"}, {"NE", "Niger"}, {"NG", "Nigeria"},
    {"NU", "Niue"}, {"NF", "Norfolk Island"}, {"MP", "Northern Mariana Islands"},
    {"NO", "Norway"}, {"OM", "Oman"}, {"PK", "Pakistan"}, {"PW", "Palau"},
    {"PS", "Palestine, State of"}, {"PA", "Panama"}, {"PG", "Papua New Guinea"},
    {"PY", "Paraguay"}, {"PE", "Peru"}, {"PH", "Philippines"}, {"PN", "Pitcairn"},
    {"PL", "Poland"}, {"PT", "Portugal"}, {"PR", "Puerto Rico"}, {"QA", "Qatar"},
    {"MK", "Republic of North Macedonia"}, {"RE", "Réunion"}, {"RO", "Romania"},
    {"RU", "Russian Federation"}, {"RW", "Rwanda"}, {"BL", "Saint Barthélemy"},
    {"SH", "Saint Helena, Ascension and Tristan da Cunha"}, {"KN", "Saint Kitts and Nevis"},
    {"LC", "Saint Lucia"}, {"MF", "Saint Martin (French part)"}, {"PM", "Saint Pierre and Miquelon"},
    {"VC", "Saint Vincent and the Grenadines"}, {"WS", "Samoa"}, {"SM", "San Marino"},
    {"ST", "Sao Tome and Principe"}, {"SA", "Saudi Arabia"}, {"SN", "Senegal"},
    {"RS", "Serbia"}, {"SC", "Seychelles"}, {"SL", "Sierra Leone"}, {"SG", "Singapore"},
    {"SX", "Sint Maarten (Dutch part)"}, {"SK", "Slovakia"}, {"SI", "Slovenia"},
    {"SB", "Solomon Islands"}, {"SO", "Somalia"}, {"ZA", "South Africa"},
    {"GS", "South Georgia and the South Sandwich Islands"}, {"SS", "South Sudan"},
    {"ES", "Spain"}, {"LK", "Sri Lanka"}, {"SD", "Sudan"}, {"SR", "Suriname"},
    {"SJ", "Svalbard and Jan Mayen"}, {"SE", "Sweden"}, {"CH", "Switzerland"},
    {"SY", "Syrian Arab Republic"}, {"TW", "Taiwan, Province of China"},
    {"TJ", "Tajikistan"}, {"TZ", "Tanzania, United Republic of"}, {"TH", "Thailand"},
    {"TL", "Timor-Leste"}, {"TG", "Togo"}, {"TK", "Tokelau"}, {"TO", "Tonga"},
    {"TT", "Trinidad and Tobago"}, {"TN", "Tunisia"}, {"TR", "Turkey"},
    {"TM", "Turkmenistan"}, {"TC", "Turks and Caicos Islands"}, {"TV", "Tuvalu"},
    {"UG", "Uganda"}, {"UA", "Ukraine"}, {"AE", "United Arab Emirates"},
    {"GB", "United Kingdom of Great Britain and Northern Ireland"},
    {"US", "United States of America"}, {"UM", "United States Minor Outlying Islands"},
    {"UY", "Uruguay"}, {"UZ", "Uzbekistan"}, {"VU", "Vanuatu"},
    {"VE", "Venezuela (Bolivarian Republic of)"}, {"VN", "Viet Nam"},
    {"VG", "Virgin Islands (British)"}, {"VI", "Virgin Islands (U.S.)"},
    {"WF", "Wallis and Futuna"}, {"EH", "Western Sahara"}, {"YE", "Yemen"},
    {"ZM", "Zambia"}, {"ZW", "Zimbabwe"}
};

void Country_Find(void) {
    // Run the curl command to fetch the country code
    FILE *pipe = POPEN("curl -s ipinfo.io/country", "r");
    if (!pipe) {
        fprintf(stderr, RED "[ERROR]" RESET " Could not lookup your country.\n\n");
        return;
    }
    
    char country_code[4] = {0};  // Expecting a 2-character country code + null terminator
    if (!fgets(country_code, sizeof(country_code), pipe)) {
        fprintf(stderr, RED "[ERROR]" RESET " Failed reading country code.\n\n");
        PCLOSE(pipe);
        return;
    }
    PCLOSE(pipe);
    // Remove any newline or carriage return characters
    country_code[strcspn(country_code, "\r\n")] = '\0';

    // Search for this country code in the worldCountries array.
    int numCountries = sizeof(worldCountries) / sizeof(worldCountries[0]);
    const char* country_name = NULL;
    int found = 0;
    for (int i = 0; i < numCountries; i++) {
        if (strcmp(worldCountries[i].code, country_code) == 0) {
            country_name = worldCountries[i].name;
            found = 1;
            break;
        }
    }
    
    // Display the result in a banner
    print_banner("Country Lookup", MAGENTA);
    if (found) {
        printf(GREEN "[INFO]" RESET " Country code: " CYAN "%s" RESET "  |  Country name: " YELLOW "%s" RESET "\n\n",
               country_code, country_name);
    } else {
        printf(GREEN "[INFO]" RESET " Your country code is: " CYAN "%s" RESET "\n\n", country_code);
    }
}

// Function to check internet connectivity
int check_internet() {
#ifdef _WIN32
    return system("ping -n 1 google.com >nul") == 0;
#else
    return system("ping -c 1 google.com >/dev/null 2>&1") == 0;
#endif
}

void measure_speed(void) {
    // Display a banner for the speed test
    print_banner("Speed Test", BLUE);
    printf(CYAN "[INFO]" RESET " Speedtest will be starting soon\n");
    printf(YELLOW "[INFO]" RESET " Please wait while we measure your connection speed...\n");
    fflush(stdout);

    // Start clock and execute a curl download (1MB file)
    clock_t start = clock();
    int ret = system("curl -s -o speedtest_tmp.bin http://speedtest.tele2.net/1MB.zip");
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("\033[F\033[K");printf("\033[F\033[K");

    if (ret == 0 && elapsed > 0.0) {
        // Calculate download speed in Mbps (1MB = 8 megabits)
        double mbps = 8.0 / elapsed;
        printf("" GREEN "[RESULT]" RESET " Approximate download speed: " MAGENTA "%.2f Mbps" RESET "\n\n", mbps);
    } else {
        printf("" RED "[ERROR]" RESET " Speed test failed. Verify your internet connection and 'curl' availability.\n\n");
    }

    // Call Country_Find to also display the country info
    Country_Find();
    // Clean up temporary file
    remove("speedtest_tmp.bin");
}

// Helper function to convert a resolution string ("WIDTHxHEIGHT") into its area.
int getResolutionArea(const char *resolution) {
    int width = 0, height = 0;
    if (sscanf(resolution, "%dx%d", &width, &height) != 2)
        return 0;
    return width * height;
}

void sortStatusList(ChannelStatus *status_list, int processed) {
    for (int i = 0; i < processed - 1; ++i) {
        for (int j = i + 1; j < processed; ++j) {
            // Compare channel names in a case-insensitive manner.
            int cmp = strcasecmp(status_list[i].name, status_list[j].name);
            // If names differ, sort alphabetically.
            // If the names match (cmp == 0), compare resolution by its area (low to high).
            if (cmp > 0 || (cmp == 0 && 
                getResolutionArea(status_list[i].resolution) > getResolutionArea(status_list[j].resolution)))
            {
                // Swap if out of order.
                ChannelStatus temp = status_list[i];
                status_list[i] = status_list[j];
                status_list[j] = temp;
            }
        }
    }
}

// Helper function: Converts a resolution string ("WxH") into a standard label.
void convertResolutionToLabel(const char *resolution, char *label, size_t label_size) {
    int width = 0, height = 0;

    // Attempt to parse the input string as "WIDTHxHEIGHT"
    if (sscanf(resolution, "%dx%d", &width, &height) != 2) {
        // On parse failure, simply return the input string.
        snprintf(label, label_size, "%s", resolution);
        return;
    }
    
    // 480-line resolution cases:
    if (height == 480) {
        if (width == 720) {
            snprintf(label, label_size, "480p");
            return;
        }
        else if (width == 352 || width == 480 || width == 528 ||
                 width == 544 || width == 640 || width == 704) {
            snprintf(label, label_size, "480i");
            return;
        }
    }
    
    // 576-line resolution cases:
    if (height == 576) {
        if (width == 720) {
            snprintf(label, label_size, "576p");
            return;
        }
        else if (width == 480 || width == 544 || width == 704) {
            snprintf(label, label_size, "576i");
            return;
        }
    }
    
    // 720-line resolution cases:
    if (height == 720) {
        if (width == 1280) {
            snprintf(label, label_size, "720p");
            return;
        }
    }
    
    // 1080-line resolution cases:
    if (height == 1080) {
        if (width == 1920) {
            snprintf(label, label_size, "1080p");
            return;
        }
        else if (width == 1280 || width == 1440) {
            snprintf(label, label_size, "1080i");
            return;
        }
    }

    // 1440-line resolution cases:
    if (height == 1440) {
        if (width == 2560) {
            snprintf(label, label_size, "1440p - 2K");
            return;
        }
    }
    
    // UHD resolutions:
    if (width == 3840 && height == 2160) {
        snprintf(label, label_size, "2160p - 4K");
        return;
    }
    if (width == 7680 && height == 4320) {
        snprintf(label, label_size, "4320p - 8K");
        return;
    }
    
    // Fallback: if no condition matches, do not convert.
    snprintf(label, label_size, "%s", resolution);
}

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

    // Variables for ETA calculation
    time_t est_Begin, est_Finish;
    double runtime = 0, total_runtime = 0, avg_time = 0, remaining = 0;
    int eta_seconds = 0, eta_hours = 0, eta_minutes = 0, eta_secs = 0;

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
    printf("%s==============================================%s\n", BLUE, RESET);
    printf("%s          TV Channel Active Scanner           %s\n", CYAN, RESET);
    printf("%s==============================================%s\n", BLUE, RESET);

    printf("%s[INFO]%s Started at: %s%s%s\n", YELLOW, RESET, WHITE, timebuf, RESET);
        printf("%s[INFO]%s Scanning %s%d%s channels. Please wait...\n\n", CYAN, RESET, GREEN, total_channels, RESET);

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
        // Remove resolution info if present (Beginning with '(' and ending with ')')
        char* res_start = strchr(channel_name, '(');
        if (res_start) {
            *res_start = '\0'; // Truncate at '('
        }
        // Remove space if begin with space
        while (*channel_name == ' ' || *channel_name == '\t') {
            memmove(channel_name, channel_name + 1, strlen(channel_name));
        } 
        // Remove leading/trailing whitespace
        char* start = channel_name;
        while (*start == ' ' || *start == '\t') start++;
        char* end = channel_name + strlen(channel_name) - 1;
        while (end > start && (*end == ' ' || *end == '\t')) end--;
        end[1] = '\0'; // Null-terminate the string after trimming

        char resolution[128] = {0};
        // TV scan style progress
        // Clear line with ANSI escape code, then print
        int barWidth = 30;
        double progress = (double)processed / total_channels;
        int pos = (int)(barWidth * progress);
        est_Begin = time(NULL);
        processed++;
        // Clear current line and print the progress bar with spinner, progress percentage, and ETA (without newline)
        printf("\033[2K\r" CYAN "Scanning " RESET "[" YELLOW "%c" RESET "] " 
            BLUE "%3d" RESET "/" BLUE "%d" RESET ": " WHITE "%-30.30s" RESET " | [", 
            scan_chars[scan_idx], processed, total_channels, channel_name);

        for (int i = 0; i < barWidth; ++i) {
            if (i < pos)
                printf(GREEN "=" RESET);
            else if (i == pos)
                printf(RED ">" RESET);
            else
                printf(" ");
        }

        printf("]" CYAN "%3d%%" RESET " | " MAGENTA "ETA: %02d:%02d:%02d" RESET,
            (int)(progress * 100), eta_hours, eta_minutes, eta_secs);
        fflush(stdout);
        scan_idx = (scan_idx + 1) % 4;

        // Read next line (should be URL)
        if (!fgets(url, sizeof(url), fi))
            break;
        url[strcspn(url, "\n")] = '\0';
        int is_active = 0;
        int is_timeout = 0;
        boolean Test = FALSE;
        boolean firstTry = TRUE; // Flag to track if this is the first attempt for the channel
        char fetched_resolution[128] = {0}; 
        do {
            if (Test)
                firstTry = 0;  // if Test is true, we’re no longer on our first try

            // Reset variables for each iteration
            is_timeout = 0;
            int status_code = 0;
            int got_response = 0;
            int is_download = 0;
            char resp[256];

            // Check if URL contains "http://" or "https://"
            if (strstr(url, "http://") || strstr(url, "https://")) {
                // First check for HTTP status code using max-time 23s
                snprintf(command, sizeof(command),
                        "curl -I -s --max-time 23 \"%s\" | find \"HTTP/\" 2>&1",
                        url);

                FILE* fp = _popen(command, "r");
                if (fp) {
                    while (fgets(resp, sizeof(resp), fp)) {
                        if (strncmp(resp, "HTTP/", 5) == 0) {
                            char* token = strtok(resp, " ");
                            token = strtok(NULL, " ");
                            if (token)
                                status_code = atoi(token);
                            got_response = 1;
                        }
                    }
                    _pclose(fp);
                }

                // If status code is 302, find "Location:" and update the URL
                if (status_code == 302) {
                    char newUrl[512] = {0};
                    snprintf(command, sizeof(command),
                            "curl -I -s \"%s\" | find \"Location:\" 2>&1",
                            url);
                    fp = _popen(command, "r");
                    if (fp) {
                        while (fgets(resp, sizeof(resp), fp)) {
                            if (strstr(resp, "Location:")) {
                                // Extract the redirected URL after "Location:"
                                char* loc = resp + 9;
                                while (*loc == ' ' || *loc == '\t')
                                    loc++;
                                strncpy(newUrl, loc, sizeof(newUrl) - 1);
                                newUrl[strcspn(newUrl, "\r\n")] = '\0';
                                break;
                            }
                        }
                        _pclose(fp);
                    }
                    // Replace old URL with the new one
                    strncpy(url, newUrl, strlen(newUrl) + 1);
                }

                // Determine if URL is active based on response codes or download header
                if (is_timeout) {
                    is_active = 0;
                    // Do not update counters here.
                    // We'll update timeout_count later after the loop.
                } else if ((got_response && status_code >= 200 && status_code < 400) || is_download) {
                    {
                        {
                            char res_cmd[1024] = {0};
                            // Build the command with the current URL.
                            snprintf(res_cmd, sizeof(res_cmd),
                                    "curl -s \"%s\" | findstr /i \"RESOLUTION\" | powershell -Command \"$input | ForEach-Object { if ($_ -match 'RESOLUTION=([0-9]+x[0-9]+)') { $matches[1] } } | Select-Object -Last 1\"",
                                    url);
                            FILE* fp_res = _popen(res_cmd, "r");
                            if (fp_res) {
                                char local_buf[128] = {0};
                                if (fgets(local_buf, sizeof(local_buf), fp_res) != NULL) {
                                    local_buf[strcspn(local_buf, "\r\n")] = '\0';
                                    // Copy the result into our persistent variable.
                                    strncpy(fetched_resolution, local_buf, sizeof(fetched_resolution) - 1);
                                }
                                _pclose(fp_res);
                            }
                        }
                    }
                    is_active = 1;
                } else {
                    is_active = 0;
                }
            }
            else {
                is_active = 0;
            }
            Test = 1; // Mark that we've completed an attempt for this channel
            // Continue looping only if it's the first try and the channel is not yet active.
        } while (firstTry && !is_active);

        // --- Now update counters only once for this channel ---
        if (is_active) {
            active_count++;
        } else {
            inactive_count++;
            if (is_timeout)
                timeout_count++;
        }

        //Save info to the status list
        strncpy(status_list[processed - 1].name, channel_name, sizeof(status_list[processed - 1].name) - 1);
        status_list[processed - 1].name[sizeof(status_list[processed - 1].name) - 1] = '\0';
        strncpy(status_list[processed - 1].url, url, sizeof(status_list[processed - 1].url) - 1);
        status_list[processed - 1].url[sizeof(status_list[processed - 1].url) - 1] = '\0';
        status_list[processed - 1].active = is_active;
        status_list[processed - 1].timeout = is_timeout;

        // If we got a resolution, store it
        if (strlen(fetched_resolution) > 0) {
            strncpy(status_list[processed - 1].resolution, fetched_resolution, sizeof(status_list[processed - 1].resolution) - 1);
            status_list[processed - 1].resolution[sizeof(status_list[processed - 1].resolution) - 1] = '\0';
        } else {
            status_list[processed - 1].resolution[0] = '\0'; // No resolution info
        }

        // After the scan is done, clear the progress line so it's blank
        printf("\33[2K\r");
        fflush(stdout);

        // Update ETA calculations
        est_Finish = time(NULL);
        runtime = difftime(est_Finish, est_Begin);
        total_runtime += runtime;
        avg_time = total_runtime / processed; // Average time per channel
        remaining = avg_time * (total_channels - processed); // Remaining time in seconds
        // Calculate ETA
        eta_seconds = (int)remaining;
        eta_hours   = eta_seconds / 3600;
        eta_minutes = (eta_seconds % 3600) / 60;
        eta_secs    = eta_seconds % 60;
    }

    //Sort the status list by channel name (In alphabetical order) (if  matching, sort by resolution from low to high)
    sortStatusList(status_list, processed);

    // Clear progress line and print done message
    printf("\033[2K\r" CYAN "Scanning [" GREEN "✓" CYAN "] Complete! " YELLOW "%d/%d" CYAN " channels processed." RESET "\n", processed, total_channels);
    clock_t end_time = clock();
    double elapsed_sec = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // Calculate elapsed time in hours, minutes, seconds
    int elapsed_sec_int = (int)elapsed_sec;
    int hours = elapsed_sec_int / 3600;
    int minutes = (elapsed_sec_int % 3600) / 60;
    int seconds = elapsed_sec_int % 60;

    // Print summary table header (ASCII only)
    printf("\n" BLUE "=====================================================================================" RESET "\n");
    printf("| " WHITE "%-4s" RESET " | " WHITE "%-40s" RESET " | " WHITE "%-20s" RESET " | " WHITE "%-10s" RESET " |\n", "No.", "Channel Name", "Resolution", "Status");
    printf(BLUE "-------------------------------------------------------------------------------------" RESET "\n");

    int idx = 1;
    // Print ACTIVE channels
    for (int i = 0; i < processed; ++i) {
        if (status_list[i].active) {
            char res_label[32] = "";
            if (strlen(status_list[i].resolution) > 0) {
                convertResolutionToLabel(status_list[i].resolution, res_label, sizeof(res_label));
            }
            // Active channels: channel name in MAGENTA, status in GREEN
            printf("| " CYAN "%-4d" RESET " | " WHITE "%-40.40s" RESET " | " WHITE "%-20.20s" RESET " | " GREEN "%-10s" RESET " |\n",
                idx++, status_list[i].name, res_label, "ACTIVE");
        }
    }
    printf(BLUE "-------------------------------------------------------------------------------------" RESET "\n");

    idx = 1;
    // Print INACTIVE channels (including those with timeout)
    for (int i = 0; i < processed; ++i) {
        if (!status_list[i].active) {
            char res_label[32] = "";
            if (strlen(status_list[i].resolution) > 0) {
                convertResolutionToLabel(status_list[i].resolution, res_label, sizeof(res_label));
            }
            if (status_list[i].timeout) {
                // Timeout channels: show status in YELLOW
                printf("| " CYAN "%-4d" RESET " | " WHITE "%-40.40s" RESET " | " WHITE "%-20.20s" RESET " | " YELLOW "%-10s" RESET " |\n",
                    idx++, status_list[i].name, res_label, "TIMEOUT");
                timeout_count++;
            } else {
                // Inactive channels: show status in RED
                printf("| " CYAN "%-4d" RESET " | " WHITE "%-40.40s" RESET " | " WHITE "%-20.20s" RESET " | " RED "%-10s" RESET " |\n",
                    idx++, status_list[i].name, res_label, "INACTIVE");
            }
        }
    }
    printf(BLUE "=====================================================================================" RESET "\n");

    // Print the summary statistics with color for clarity
    printf("\nTotal channels: " WHITE "%d" RESET " | Active: " GREEN "%d" RESET " | Inactive: " RED "%d" RESET " | Timeout: " YELLOW "%d" RESET "\n",
        processed, active_count, inactive_count, timeout_count);
    printf("Total scan time: " CYAN "%02d:%02d:%02d" RESET " (hh:mm:ss)\n", hours, minutes, seconds);

    // Write only ACTIVE channels to the output file.
    if (fo) {
        for (int i = 0; i < processed; ++i) {
            if (status_list[i].active) {
                if (strlen(status_list[i].resolution) > 0) {
                    char res_label[32] = "";
                    convertResolutionToLabel(status_list[i].resolution, res_label, sizeof(res_label));
                    fprintf(fo, "#EXTINF:-1,%s (%s)\n", status_list[i].name, res_label);
                } else {
                    fprintf(fo, "#EXTINF:-1,%s\n", status_list[i].name);
                }
                fprintf(fo, "%s\n", status_list[i].url);
            }
        }
    }

    // Close files and free memory
    if (status_list) free(status_list);
    fclose(fi);
    fclose(fo);
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
            char curr_name_buf[256] = {0};
            char last_name_buf[256] = {0};
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
            } else if (duplicate_count == 5) {
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

    /* Print summary information */
    printf(BLUE "------------------------------------------------------------\n" RESET);
    if (duplicate_count > 0) {
        printf(GREEN "[SUMMARY]" RESET " " YELLOW "%d" RESET " duplicate channel(s) removed based on identical URLs.\n", duplicate_count);
        printf(CYAN "[RESULT]" RESET " Output file cleaned and optimized for unique channels.\n");
    } else {
        printf(GREEN "[SUMMARY]" RESET " No duplicate channels found. Output file is already clean.\n");
    }
    printf(BLUE "============================================================\n\n" RESET);
}


void main()
{
    int First = 1;  // Flag to ensure error message prints only once
    const char *Input  = "input.txt";
    const char *Output = "output.m3u";

    printf("-------------------------------------------------------------------------------------------------------------------------\n");
    printf(RED "[CAUTION]" RESET " The dashed line above represents the minimum terminal window dimensions required for proper UI functionality.\n");
    fflush(stdout);
    #ifdef _WIN32
    Sleep(10000);
    #else
    sleep(10);
    #endif
    printf("\033[2K\r");  // Clear the warning line
    fflush(stdout);

    while (!check_internet()) {
        time_t now = time(NULL);
        char timecheck[64];
        strftime(timecheck, sizeof(timecheck), "%Y-%m-%d %H:%M:%S", localtime(&now));
        if (First) {
            printf(BLUE "=====================================================================================\n" RESET);
            printf(RED "[ERROR]" RESET " No internet connection detected. Please check your network and try again.\n");
            printf(YELLOW "[Re-checking]" RESET " Ensure your Wi-Fi/Ethernet is active and, if using a VPN, consider disabling it.\n");
            printf(BLUE "=====================================================================================\n" RESET);
            First = 0;
        }
        printf(YELLOW "[Re-Checking]" RESET " Last checked at: %s", timecheck);
        #ifdef _WIN32
        Sleep(1000);
        #else
        sleep(10);
        #endif
        printf("\033[2K\r");
    }
    
    /* Tidy up the screen by clearing the last few lines */
    for (int i = 0; i < 6; i++) {
        printf("\033[F\033[K");  // Moves cursor up one line and clears it
    }
    
    if (check_internet()) {
        measure_speed();
        
        // Welcome banner
        printf(BLUE "=============================================\n" RESET);
        printf(BLUE "|         Online IPTV Channel Scanner        |\n" RESET);
        printf(BLUE "|         Version 1.2 - C17 Standard         |\n" RESET);
        printf(BLUE "|           Developed by ShouNLAK            |\n" RESET);
        printf(BLUE "=============================================\n" RESET);
        
        // Countdown before scan starts
        printf(CYAN "[COUNTDOWN]" RESET " The scan will start in 5 seconds...");
        fflush(stdout);
        #ifdef _WIN32
        Sleep(5000);
        #else
        sleep(5);
        #endif
        printf("\033[2K\r");
        fflush(stdout);
        
        /* ----------------------------------------------------------------
           Step 1: Scan Channels
           ---------------------------------------------------------------- */
        printf("\n" CYAN "[STEP 1]" RESET " Scanning channels for activity...\n");
        Check_Active(Input, Output);
        
        /* ----------------------------------------------------------------
           Step 2: Remove Duplicates
           ---------------------------------------------------------------- */
        printf("\n" CYAN "[STEP 2]" RESET " Removing duplicate channels from output file...\n");
        Remove_Duplicates(Output);
        
        /* ----------------------------------------------------------------
           Final Success Message:
           Inform the user of the successful scan and the output location.
        ---------------------------------------------------------------- */
        printf("\n" BLUE "=====================================================================================\n" RESET);
        printf(GREEN "[SUCCESS]" RESET " " CYAN "Scan complete!" RESET " Active channels saved to " YELLOW "%s" RESET "\n", Output);
        printf(CYAN "[INFO]" RESET " You can now use this file with " GREEN "VLC" RESET " or any media player that supports " YELLOW "M3U" RESET " playlists.\n");
        printf(BLUE "=====================================================================================\n" RESET);
        
        /* ----------------------------------------------------------------
           Optional: Open Output in VLC
        ---------------------------------------------------------------- */
        char choice;
        printf("\n" YELLOW "[OPTION]" RESET " Do you want to open the output file in " GREEN "VLC" RESET "? (" GREEN "y" RESET "/" RED "n" RESET "): ");
        scanf(" %c", &choice);
        printf("\033[F\033[K");  // Clear the option input line
        if (choice == 'y' || choice == 'Y') {
        #ifdef _WIN32
            printf(BLUE "[INFO]" RESET " Launching " GREEN "VLC" RESET "...\n");
            system("start vlc.exe output.m3u");
        #else
            printf(BLUE "[INFO]" RESET " Launching " GREEN "VLC" RESET "...\n");
            system("vlc output.m3u &");
        #endif
        } else {
            printf(BLUE "[INFO]" RESET " Exiting without opening VLC.\n");
            printf(BLUE "[INFO]" RESET " You can open the output file manually in VLC or another media player.\n");
        }
        
        // Final countdown before exit
        printf(CYAN "[COUNTDOWN]" RESET " Exiting in 10 seconds...");
        fflush(stdout);
        #ifdef _WIN32
                Sleep(5000);
        #else
                sleep(50);
        #endif
        printf("\033[2K\r"); fflush(stdout);
    }
    
    /* --------------------------------------------------------------------
       Goodbye Message
    -------------------------------------------------------------------- */
    printf(BLUE "=====================================================================================\n" RESET);
    printf(BLUE "[INFO]" RESET " Thank you for using the Online IPTV Channel Scanner!\n");
    printf(BLUE "[INFO]" RESET " Developed by ShouNLAK. Goodbye!\n");
    printf(BLUE "=====================================================================================\n" RESET);
    #ifdef _WIN32
        Sleep(5000);
    #else
        sleep(50);
    #endif
}
