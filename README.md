# Online IPTV Channel Scanner – Version 1.2

**An advanced IPTV scanner built in Standard C for Windows.**  
This release verifies each stream URL in an M3U playlist, measures internet speed with country lookup, auto-redirects channels to their best quality stream, displays a colorized UI with timestamps and countdowns, and automatically removes duplicate channels with enhanced resolution conversion.

> Released on 10/06/2025

---

## Table of Contents

- [Introduction](#introduction)  
- [Prerequisites](#prerequisites)  
- [How It Works](#how-it-works)  
- [New & Enhanced Features](#new--enhanced-features)  
- [Version Comparison](#version-comparison)  
- [Clone from GitHub](#clone-from-github)  
- [Demo & UI Preview](#demo--ui-preview)  
- [Frequently Asked Questions](#frequently-asked-questions)  
- [License & Credits](#license--credits)  

---

## Introduction

Version 1.2 further refines the IPTV Scanner by building on the features present in v1.1. In addition to basic channel scanning, this version now:

- **Measures Internet Speed & Detects Country:** Upon startup, the scanner downloads a 1 MB file and reports an approximate download speed in Mbps while displaying your detected country (via an IP lookup).  
- **Auto-Redirect Handling:** If a channel URL returns a redirection (HTTP status 302), the scanner automatically fetches the updated URL to ensure it points to the best available stream.  
- **Enhanced Resolution Conversion:** Converts raw resolution strings into common labels (e.g., “480p”, “1080p”, “2160p – 4K”) using defined mappings based on width and height.  
- **Improved Duplicate Removal:** Processes the output file to remove duplicate channels, keeping only the version with higher quality or better resolution when duplicates are detected.
- **Refined UI Experience:** Retains a colorized, interactive UI complete with real-time progress indicators, countdown timers, and timestamped messages.

These improvements provide a more comprehensive, reliable, and informative scanning experience.

---

## Prerequisites

- **Operating System:** Windows 10 or later  
- **Required Tools:**  
  - `curl` (for HTTP requests, redirection handling, and speed tests)  
  - `ping` (for connectivity checks)  
  - An M3U file named `input.txt` (see format below)  
  - **Optional:** VLC Media Player (to automatically launch the output playlist)

- **Input File Format:**  
  Each channel block in `input.txt` must follow this structure:
  ```m3u
  #EXTINF:-1,Channel Name (HD)
  https://stream.example.com/…
  ```

---

## How It Works

1. **Connectivity & Terminal Check**  
   The scanner begins by pinging `google.com` to confirm that an internet connection is available and ensures that the terminal window meets a minimum size requirement.

2. **Internet Speed & Country Lookup**  
   A 1 MB file is downloaded via `curl` to estimate download speed (in Mbps). Simultaneously, an IP lookup is performed (via `ipinfo.io/country`) to display the user’s country with a friendly banner.

3. **Playlist Parsing & Channel Scanning**  
   - Opens `input.txt`, skipping the header (`#EXTM3U`).  
   - Parses each channel entry to extract the channel name and optional resolution/quality information.  
   - For each URL starting with `http://` or `https://`, the scanner sends an HTTP request using:
     ```bat
     curl -I -L -s --max-time 15 "<URL>"
     ```
     A response code of 200–399 (or detection of a Content-Disposition header) marks a channel as **active**.  
   - If the HTTP status code is 302 (redirection), the scanner fetches the new URL from the “Location:” header and updates it automatically.

4. **Duplicate Removal**  
   Once scanning is complete, the generated `output.m3u` is re-parsed to remove duplicate channels. When identical URLs are detected, only the channel with the better (or higher) quality/resolution remains.

5. **Output Generation & VLC Launch**  
   - Writes all **active** channels (including enhanced resolution info) to `output.m3u`.  
   - Prompts the user to optionally open the output file in VLC.

---

## New & Enhanced Features

- **Internet Speed & Country Detection:**  
  Displays download speed in Mbps and shows the user’s country using an IP lookup API.

- **Auto-Redirect Mechanism:**  
  Detects HTTP redirections (302) and auto-fetches the redirected URL to ensure the best quality stream is selected.

- **Enhanced Resolution Conversion:**  
  Converts raw resolution strings (in the format "WIDTHxHEIGHT") into standard labels (e.g., “480p”, “1080p”, “2160p – 4K”) for user-friendly output.

- **Refined Duplicate Removal:**  
  Automatically identifies and consolidates duplicate channel entries based on URL and quality, keeping only the optimal version.

- **Advanced UI with Countdown & Timestamp:**  
  A color-coded, dynamic progress bar with spinner updates, countdown timers, and timestamp banners creates an engaging scanning experience.

---

## Version Comparison

| Feature                 | Version 1.1                                      | Version 1.2                                                                                             |
|-------------------------|--------------------------------------------------|---------------------------------------------------------------------------------------------------------|
| **Internet Speed Test** | Not available                                    | Measures download speed and displays Mbps along with country lookup information                         |
| **Auto-Redirect**       | Basic redirection handling                       | Improved handling of HTTP 302; automatically updates the channel URL                                  |
| **Resolution Conversion**| Standard parsing with basic quality mapping      | Enhanced conversion using resolution dimensions to generate common labels (e.g., "1080p", "4K")          |
| **UI Enhancements**     | Colorized progress bar, countdown, and timestamps | Refined UI with detailed progress indicators, improved countdown timers, and additional banners         |
| **Duplicate Removal**   | Automatic removal based on URL, retaining higher quality | Improved duplicate removal with integrated resolution comparison and quality-based selection           |
| **Terminal Window Check** | Not implemented                                | Warns users if the terminal window is too small for full UI rendering                                   |

---

## Clone from GitHub

To grab the v1.2 scanner and run it locally:

```bash
# 1. Clone the repository
git clone https://github.com/ShouNLAK/Check-Online-IPTV.git

# 2. Enter the Version 1.2 folder
cd Check-Online-IPTV/Version/"Check Online IPTV (Ver 1.2)"

# 3. Compile the scanner
gcc scanner.c -o IPTVScanner.exe

# 4. Place your M3U playlist in this directory as "input.txt"
#    (Ensure your file follows the correct M3U format)

# 5. Run the scanner
IPTVScanner.exe
```

*Note: On Windows CMD, simply run `IPTVScanner.exe` without the `./` prefix.*

---

## Demo & UI Preview

- **Scanning Progress & Countdown**  
  ![Scanning Progress](https://github.com/user-attachments/assets/544d36c1-e48b-4619-b871-4afc7a7ad85c)

- **Scan Summary Table**  
  ![Summary Output](https://github.com/user-attachments/assets/4d3e856d-4f11-40cf-946e-1b426e855b29)

- **Duplicate Removal Process**  
  ![Duplicate Removal](https://github.com/user-attachments/assets/9be94a7d-379d-4310-9fdd-07866636b897)

---

## Frequently Asked Questions

**Q: What new features does Version 1.2 include over Version 1.1?**  
**A:** Version 1.2 integrates an advanced speed test with country lookup, improved auto-redirect handling, enhanced resolution conversion, and refined duplicate removal.

**Q: How does the auto-redirect mechanism work?**  
**A:** If a channel URL responds with HTTP status 302, the scanner automatically retrieves the “Location:” header value and updates the URL to point to the best available stream.

**Q: What is the purpose of the country lookup?**  
**A:** The tool uses an IP lookup to determine your country code and display the corresponding country name, which is useful for diagnosing geo-restriction issues.

**Q: Can the URL timeout be modified?**  
**A:** The timeout is set to 15 seconds per URL by default but can be increased (up to 30 seconds) by modifying the source code.

**Q: What happens if my terminal window is too small?**  
**A:** The scanner warns you and pauses execution until the terminal window is resized for optimal UI rendering.

---

## License & Credits

Developed by **ShouNLAK**  
Provided as-is for educational and personal use. 
Contributions, issue reports, and pull requests are welcome via GitHub.

---
