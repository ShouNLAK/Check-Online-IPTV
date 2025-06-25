#include "Network.h"

// check_internet: Checks if there is an active internet connection by pinging google.com.
// On Windows, the command is "ping -n 1 google.com >nul".
// On Unix-like systems, the command is "ping -c 1 google.com >/dev/null 2>&1".
int check_internet()
{
#ifdef _WIN32
    return system("ping -n 1 google.com >nul") == 0;
#else
    return system("ping -c 1 google.com >/dev/null 2>&1") == 0;
#endif
}

void Country_Find() {
    // Run the curl command to fetch the country code
    FILE* pipe = POPEN("curl -s ipinfo.io/country", "r");
    if (!pipe) {
        fprintf(stderr, RED "[ERROR]" RESET " Could not lookup your country.\n\n");
        return;
    }

    char country_code[4] = { 0 };  // 2-letter code + newline + nul
    if (!fgets(country_code, sizeof(country_code), pipe)) {
        fprintf(stderr, RED "[ERROR]" RESET " Failed reading country code.\n\n");
        PCLOSE(pipe);
        return;
    }
    PCLOSE(pipe);

    // Strip newline/carriage returns
    country_code[strcspn(country_code, "\r\n")] = '\0';

    // Lookup in our worldCountries array
    int numCountries = worldCountryCount;
    const char* country_name = NULL;
    for (int i = 0; i < numCountries; i++) {
        if (strcmp(worldCountries[i].code, country_code) == 0) {
            country_name = worldCountries[i].name;
            break;
        }
    }

    // Print the result
    if (country_name) {
        printf(CYAN "[LOCATION]" RESET " Detected country: " GREEN "%s" RESET " (%s)\n\n",
            country_name, country_code);
    }
    else {
        printf(YELLOW "[LOCATION]" RESET " Country code “%s” not recognized in database.\n\n",
            country_code);
    }
}


void measure_speed() {
    // Display a banner for the speed test.
    print_banner("Speed Test", BLUE);
    printf(CYAN "[INFO]" RESET " Speedtest will be starting soon\n");
    printf(YELLOW "[INFO]" RESET " Please wait while we measure your connection speed...\n");

    // Start clock and execute a curl download (1MB file) with timeout options.
    clock_t start = clock();

    // Note: We add timeout options so that if the connection is slow or the server is unresponsive,
    // the curl command doesn't hang forever. Adjust the timeouts as needed.
    int ret = system("curl -s --connect-timeout 10 --max-time 30 -o speedtest_tmp.bin http://speedtest.tele2.net/1MB.zip");

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    // Remove the two status lines printed during the curl execution.
    printf("\033[F\033[K");
    printf("\033[F\033[K");

    if (ret == 0 && elapsed > 0.0) {
        // Calculate download speed in Mbps: (1MB = 8 megabits).
        double mbps = 8.0 / elapsed;
        printf(GREEN "[RESULT]" RESET " Approximate download speed: " MAGENTA "%.2f Mbps" RESET "\n", mbps);
    }
    else {
        printf(RED "[ERROR]" RESET " Speed test failed. Verify your internet connection and 'curl' availability.\n");
    }

    // Call Country_Find to also display the country info.
    Country_Find();

    // Clean up temporary file.
    remove("speedtest_tmp.bin");
}
