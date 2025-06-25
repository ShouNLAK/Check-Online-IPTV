#include "All.h"

void main()
{
    int First = 1;
    const char* Input = "input.txt";
    const char* Output = "output.m3u";
    int concurrency = 4; // default

    // 1) Initial terminal-size warning
    printf(BRIGHT_WHITE "-------------------------------------------------------------------------------------------------------------------------" RESET "\n");
    printf(RED "[CAUTION]" RESET " The dashed line above represents the minimum terminal window dimensions required for proper UI functionality.\n");
    fflush(stdout);
#ifdef _WIN32
    Sleep(10000);
#else
    sleep(10);
#endif
    printf("\033[2K\r");  // clear that line
    fflush(stdout);
    // 2) Wait for internet
    while (!check_internet()) {
        time_t now = time(NULL);
        char ts[64];
        strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", localtime(&now));
        if (First) {
            printf(BLUE "=====================================================================================\n" RESET);
            printf(RED "[ERROR]" RESET " No internet connection detected. Please check your network and try again.\n");
            printf(YELLOW "[Re-checking]" RESET " Ensure your Wi-Fi/Ethernet is active and, if using a VPN, consider disabling it.\n");
            printf(BLUE "=====================================================================================\n" RESET);
            First = 0;
        }
        printf(CYAN "[Re-Checking]" RESET " Last checked at: %s", ts);
#ifdef _WIN32
        Sleep(1000);
#else
        sleep(10);
#endif
        printf("\033[2K\r");
    }

    // 3) Clean up warning lines
    for (int i = 0; i < 6; i++) {
        printf("\033[F\033[K");
    }

    // 4) Speed test + country lookup
    if (check_internet()) {
        measure_speed();      // prints speed AND calls Country_Find()

        // Ask user for concurrency (UI)
        int max_threads = get_number_of_processors();
        concurrency = ask_concurrency(max_threads);

        // Ask user if they want live log (Yes/No only, retry if invalid)
        char live_log_choice[8];
        int enable_live_log = 0;
        printf(BLUE "=====================================================================================\n" RESET);
        printf(CYAN "|                                Live Log Option                                    |\n" RESET);
        printf(CYAN "|   Live Log writes progress to log.txt, which you can open during scanning.        |\n" RESET);
        printf(BLUE "=====================================================================================\n" RESET);
        printf("Enable Live Log? (Yes/No): ");
        while (1) {
            fflush(stdout);
            char input[32];
            if (!fgets(input, sizeof(input), stdin)) {
            // Input error, try again
            printf("\033[F\033[K");
            printf(RED "[ERROR]" RESET " Please answer Yes or No: ");
            continue;
            }
            // Remove trailing newline
            size_t len = strlen(input);
            if (len > 0 && input[len - 1] == '\n') input[len - 1] = '\0';
            // Check if input is empty (user pressed Enter)
            if (input[0] == '\0') {
            printf("\033[F\033[K");
            printf(CYAN "Please answer Yes or No: ");
            continue;
            }
            // Convert to lowercase for comparison
            for (char* p = input; *p; ++p) *p = (char)tolower(*p);
            if (strcmp(input, "yes") == 0) {
            enable_live_log = 1;
            break;
            } else if (strcmp(input, "no") == 0) {
            enable_live_log = 0;
            break;
            } else {
            printf("\033[F\033[K");
            printf(RED "[ERROR]" RESET " Please answer Yes or No: ");
            }
        }
        printf("\033[F\033[K");
        if (enable_live_log) {
            printf(GREEN "[INFO]" RESET " Live Log : " GREEN "enabled.\n\n"); 
        } else {
            printf(YELLOW "[INFO]" RESET " Live Log : " RED "disabled.\n\n");
        }

        if (enable_live_log) {
        initialize_log_txt("log.txt");
        #ifdef _WIN32
        // Open a new CMD window at 250x30 for live log display
        system("start powershell -Command Get-Content log.txt -Wait\"");
    #else
        system("xdg-open log.txt &");
    #endif
    }

        // 5) Welcome banner
        printf(BLUE "=====================================================================================\n" RESET);
        printf(CYAN "|                          Online IPTV Channel Scanner                              |\n" RESET);
        printf(CYAN "|                         Version 1.3   |   C17 Standard                            |\n" RESET);
        printf(CYAN "|                            Developed by ShouNLAK                                  |\n" RESET);
        printf(BLUE "=====================================================================================\n" RESET);

        // 6) Countdown to scan
        printf("[COUNTDOWN] The scan will start in 5 seconds...");
        fflush(stdout);
#ifdef _WIN32
        Sleep(5000);
#else
        sleep(5);
#endif
        printf("\033[2K\r");
        fflush(stdout);

        // 7) Step 1: Channel scan
        printf("\n[STEP 1] Scanning channels for activity...\n");
        Check_Active(Input, Output, concurrency, enable_live_log);

        // 8) Step 2: Duplicate removal
        printf("\n[STEP 2] Removing duplicate channels from output file...\n");
        Remove_Duplicates(Output);

        // 9) Success & info
        printf(BLUE "=====================================================================================\n" RESET);
        printf(GREEN "[SUCCESS]" RESET " " CYAN "[INFO]" RESET " Scan complete! Active channels saved to %s\n", Output);
        printf(CYAN "[INFO]" RESET " You can now use this file with VLC or any media player supporting M3U playlists.\n");
        printf(BLUE "=====================================================================================\n" RESET);

        // 10) Optional: launch VLC
        char choice;
        printf("\n[OPTION] Open the output in VLC? (y/n): ");
        scanf(" %c", &choice);
        printf("\033[F\033[K");
        if (choice == 'y' || choice == 'Y') {
#ifdef _WIN32
            printf(GREEN "[INFO]" RESET " Launching VLC...\n");
            system("start vlc.exe output.m3u");
#else
            printf(GREEN "[INFO]" RESET " Launching VLC...\n");
            system("vlc output.m3u &");
#endif
        }
        else {
            printf(YELLOW "[INFO]" RESET " Exiting without launching VLC.\n");
        }

        // 11) Exit countdown
        printf("[COUNTDOWN] Exiting in 10 seconds...");
        fflush(stdout);
#ifdef _WIN32
        Sleep(5000);
#else
        sleep(10);
#endif
        printf("\033[2K\r");
        fflush(stdout);
    }

    // 12) Goodbye
    printf(BLUE "=====================================================================================\n" RESET);
    printf(CYAN "[INFO]" RESET " Thank you for using the Online IPTV Channel Scanner!\n");
    printf(CYAN "[INFO]" RESET " Developed by ShouNLAK. Goodbye!\n");
    printf(BLUE "=====================================================================================\n" RESET);
#ifdef _WIN32
    Sleep(5000);
#else
    sleep(5);
#endif
}