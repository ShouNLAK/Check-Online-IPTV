# Online IPTV Channel Scanner – Version 1.1

**An enhanced IPTV channel scanner built in Standard C for Windows.**  
This release verifies each stream URL in an M3U playlist, measures internet speed, displays a colorized UI with timestamps and countdowns, and automatically removes duplicate channels based on quality.

> The code is available in the **Version** folder | Released on 09/06/2025

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

Version 1.1 represents the next step in the evolution of the IPTV Scanner. In addition to the basic URL verification found in Version 1.0, this release features:

- **Colorized, interactive UI:** Real-time progress bars, countdowns, and timestamps.  
- **Internet Speed Measurement:** Uses a 1 MB test file to report approximate Mbps.  
- **Duplicate Removal:** Compares channels with identical URLs and retains only the one with higher quality.  
- **Improved Connectivity Checks & Error Handling:** Detailed messages and retry timers if the internet connection is lost.

These enhancements deliver a more robust, user-friendly experience while preserving the core simplicity of the original release.

---

## Prerequisites

- **Operating System:** Windows 10 or later  
- **Required Tools:**  
  - `curl` (for HTTP requests and speed tests)  
  - `ping` (for connectivity checks)  
  - An M3U file named `input.txt` (see format below)  
  - **Optional:** VLC Media Player (to launch the output playlist automatically)  

- **Input File Format:**  
  Each channel block in `input.txt` must follow this structure:
  ```m3u
  #EXTINF:-1,Channel Name (HD)
  https://stream.example.com/…
  ```

---

## How It Works

1. **Connectivity & Terminal Check**  
   Pings `google.com` to verify internet access and ensures the terminal window meets a minimum size requirement.

2. **Internet Speed Measurement**  
   Downloads a 1 MB file via `curl` to estimate download speed in Mbps.

3. **Playlist Parsing & Channel Scanning**  
   - Opens `input.txt`, skips the first line (`#EXTM3U`).  
   - Extracts each `#EXTINF` entry’s channel name, optional resolution/quality, and URL.  
   - Issues the following command for each URL:
     ```bat
     curl -I -L -s --max-time 15 "<URL>"
     ```
     A response code of 200–399, or a `Content-Disposition: attachment` header, marks the stream as **active**.

4. **Duplicate Removal**  
   After scanning, the generated `output.m3u` is re‐parsed. Channels sharing identical URLs are deduplicated—keeping only the entry with the higher quality value.

5. **Output Generation & VLC Launch**  
   - Writes all **active** channels (including resolution info) to `output.m3u`.  
   - Prompts to open `output.m3u` in VLC.

---

## New & Enhanced Features

- **Colorful, Informative UI:** Real-time progress bar, spinner, countdowns, and timestamped logs.  
- **Internet Speed Test:** Displays approximate download speed after a 1 MB file download.  
- **Automatic Duplicate Removal:** Identifies and removes duplicate URLs, retaining only the highest-quality version.  
- **Interactive Connectivity Check:** Retries internet verification with clear error guidance (e.g., “disable VPN”).  
- **Optional VLC Launch:** Colorized prompt to open the cleaned playlist in VLC.

---

## Version Comparison

| Feature                 | Version 1.0                                      | Version 1.1                                                                                              |
|-------------------------|--------------------------------------------------|----------------------------------------------------------------------------------------------------------|
| Connectivity Check      | Single ping at startup                           | Interactive re-check loop with detailed errors and countdown                                             |
| Progress UI             | Plain ASCII bar                                  | Colorized UI with timestamp, spinner, and countdown                                                      |
| Duplicate Handling      | Not available                                    | Automatic removal by URL, keeping higher-quality streams                                                 |
| Resolution Parsing      | Extracts resolution from channel name            | Same parsing with enhanced handling and integrated deduplication                                         |
| Speed Measurement       | Not available                                    | Measures download speed using a 1 MB file and reports Mbps                                              |
| VLC Launch Option       | Prompt at end                                    | Retained with improved, colorized messaging                                                               |
| Terminal Window Check   | Not implemented                                  | Warns if terminal window is too small for full UI                                                        |

---

## Clone from GitHub

To grab the v1.1 scanner and run it locally:

```bash
# 1. Clone the repository
git clone https://github.com/ShouNLAK/Check-Online-IPTV.git

# 2. Change into the v1.1 source folder
cd Check-Online-IPTV/Version/"Check Online IPTV (Ver 1.1)"

# 3. Compile the scanner
gcc scanner.c -o IPTVScanner.exe

# 4. Place your M3U playlist in this directory as input.txt
#    (e.g. copy or rename your file accordingly)

# 5. Run the scanner
./IPTVScanner.exe
```

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

**Q: What improvements does Version 1.1 offer over Version 1.0?**  
**A:** Colorized UI, speed measurement, and automatic duplicate removal while retaining core scanning functionality.

**Q: How does duplicate removal work?**  
**A:** After scanning, the output file is re-parsed. If two entries share the same URL, only the one with the higher quality value remains.

**Q: Can the URL timeout be adjusted?**  
**A:** The timeout is fixed at 15 seconds per URL in both Version 1.0 and Version 1.1 (up to 30 seconds if modified in source).

**Q: What happens if the terminal window is too small?**  
**A:** A warning appears, and the process pauses until the window size is adjusted for full UI rendering.

---

## License & Credits

Developed by **ShouNLAK**
Provided as-is for educational and personal use.
Contributions, issue reports, and pull requests are welcome via GitHub.
