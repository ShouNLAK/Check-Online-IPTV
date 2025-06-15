#include "All.h"

void main()
{
    int First = 1;
    const char* Input = "input.txt";
    const char* Output = "output.m3u";

    // 1) Initial terminal-size warning
    printf("-------------------------------------------------------------------------------------------------------------------------\n");
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
        printf(YELLOW "[Re-Checking]" RESET " Last checked at: %s", ts);
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

        // 5) Welcome banner
        printf(BLUE "=============================================\n" RESET);
        printf(BLUE "|         Online IPTV Channel Scanner        |\n" RESET);
        printf(BLUE "|         Version 1.3 - C17 Standard         |\n" RESET);
        printf(BLUE "|           Developed by ShouNLAK            |\n" RESET);
        printf(BLUE "=============================================\n" RESET);

        // 6) Countdown to scan
        printf(CYAN "[COUNTDOWN]" RESET " The scan will start in 5 seconds...");
        fflush(stdout);
#ifdef _WIN32
        Sleep(5000);
#else
        sleep(5);
#endif
        printf("\033[2K\r");
        fflush(stdout);

        // 7) Step 1: Channel scan
        printf("\n" CYAN "[STEP 1]" RESET " Scanning channels for activity...\n");
        Check_Active(Input, Output);

        // 8) Step 2: Duplicate removal
        printf("\n" CYAN "[STEP 2]" RESET " Removing duplicate channels from output file...\n");
        Remove_Duplicates(Output);

        // 9) Success & info
        printf("\n" BLUE "=====================================================================================\n" RESET);
        printf(GREEN "[SUCCESS]" RESET " " CYAN "Scan complete!" RESET " Active channels saved to " YELLOW "%s" RESET "\n", Output);
        printf(CYAN "[INFO]" RESET " You can now use this file with " GREEN "VLC" RESET " or any media player supporting " YELLOW "M3U" RESET " playlists.\n");
        printf(BLUE "=====================================================================================\n" RESET);

        // 10) Optional: launch VLC
        char choice;
        printf("\n" YELLOW "[OPTION]" RESET " Open the output in " GREEN "VLC" RESET "? (" GREEN "y" RESET "/" RED "n" RESET "): ");
        scanf(" %c", &choice);
        printf("\033[F\033[K");
        if (choice == 'y' || choice == 'Y') {
#ifdef _WIN32
            printf(BLUE "[INFO]" RESET " Launching VLC...\n");
            system("start vlc.exe output.m3u");
#else
            printf(BLUE "[INFO]" RESET " Launching VLC...\n");
            system("vlc output.m3u &");
#endif
        }
        else {
            printf(BLUE "[INFO]" RESET " Exiting without launching VLC.\n");
        }

        // 11) Exit countdown
        printf(CYAN "[COUNTDOWN]" RESET " Exiting in 10 seconds...");
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
    printf(BLUE "[INFO]" RESET " Thank you for using the Online IPTV Channel Scanner!\n");
    printf(BLUE "[INFO]" RESET " Developed by ShouNLAK. Goodbye!\n");
    printf(BLUE "=====================================================================================\n" RESET);
#ifdef _WIN32
    Sleep(5000);
#else
    sleep(5);
#endif
}
