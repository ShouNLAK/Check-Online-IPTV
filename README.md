# Online IPTV Channel Scanner (Standard C) – Windows

**A powerful IPTV channel scanner built in Standard C for Windows.**  
This student-built tool validates IPTV playlists by testing stream URLs, measuring network performance, and filtering out inactive or duplicate channels—all through a vibrant, dynamic console UI.

> **Latest:** Version 1.3.0 | Released on 15/06/2025

---

## Table of Contents

- [Introduction](#introduction)
- [How to run this code](#how-to-run-this-code)
- [What’s New in Version 1.3](#whats-new-in-version-13)  
- [Prerequisites](#prerequisites)  
- [How It Works](#how-it-works)  
- [Features](#features)  
- [Optional Features](#optional-features)  
- [Research Edition File Structure](#research-edition-file-structure)  
- [Version History & Improvements](#version-history--improvements)  
- [Frequently Asked Questions](#frequently-asked-questions)  
- [Demo & UI Preview](#demo--ui-preview)  
- [License & Credits](#license--credits)  

---

## Introduction

The Online IPTV Channel Scanner helps you quickly verify and optimize M3U playlists. It measures your internet speed, handles HTTP redirects to capture the best-quality stream, converts raw resolution data into clear labels, filters duplicates, and presents results in a colorized, real-time UI. After scanning, an `output.m3u` file is generated—ready to play in VLC, Kodi, or Plex.

---

## How to run this code
1. Open your Microsoft Visual Studio Code
2. Open terminal (Ctrl + Shift + `)
3. Type this command for specific version:
   ```
   Version 1.0 : git checkout Version-1.0
   Version 1.1 : git checkout Version-1.1
   Version 1.2 : git checkout Version-1.2
   Version 1.3 : git checkout Version-1.3
   Version 1.3 (Reseach Edition) : git checkout Version-1.3---Research-Edition
   ```
4. Compile the scanner
5. Make sure to have data in `input.txt`
6. Wait due scan (Or check Live Log - Only for Research Edition)
7. Result will be in `output.txt`

*   If you're using Microsoft Visual Studio 2022, you can download file in `Folder (Version)` (Version 1.0 -> 1.3).
    For Research Edition, please download whole `Folder (Research Edition)` then create the new in MSVC and add by existing items.

---

## What’s New in Version 1.3

- **Research Edition is now available!**  
  A fully modular, instrumented codebase (7 files, <200 lines each) with built-in `scan.log`, academic annotations, and hooks for performance or deep-learning experiments.

- **Blazing-Fast Auto-Redirects & Higher Accuracy**  
  Fetches final stream URLs in just 2–3 s (instead of 22 s for some providers), boosting Normal Edition accuracy to ~98.5% and Research Edition to ~99%.

- **Internet Speed Test**  
  Same as Version 1.2: downloads a 1 MB file to report Mbps, alongside continuous connectivity checks and geo-detection.

- **Full UTF-8 Support & Fixed-Width Alignment**  
  Perfect rendering of non-English and special characters; neatly aligned columns for all tables and logs.

- **Vibrant, User-Friendly Interface**  
  Bright banners, dynamic progress bars, live countdown timers, and timestamped, color-coded logs.

- **Enhanced Resolution & Duplicate Handling**  
  Real-time resolution detection with labels like “480p”, “1080p”, “4K”; intelligent duplicate filtering retains the highest-quality stream.

- **Built-In Logging (Research Edition)**  
  Generates `scan.log` with per-URL timings, HTTP codes, retries, and failures—ideal for profiling, instrumentation, and teaching.

---

## Prerequisites

- **OS:** Windows 10 or later  
- **Tools:**  
  - `curl` (HTTP requests, speed test, auto-redirect)  
  - `ping` (connectivity check)  
  - An M3U playlist named `input.txt`  
  - **Optional:** VLC Media Player (for auto-launch)

- **Input File Format (`input.txt`):**  
  ```m3u
  #EXTM3U
  #EXTINF:-1,Channel Name (HD)
  https://stream.example.com/…
  ```

---

## How It Works

1. **Connectivity & Speed Test**  
   - Pings `google.com` for internet access.  
   - Downloads a 1 MB file to measure download speed.  
   - Performs IP lookup to retrieve your country code for geo-restriction insight.

2. **Playlist Parsing**  
   - Reads `input.txt`, counting each `#EXTINF` entry.  
   - Extracts channel names and optional manual resolution tags.

3. **URL Scanning & Auto-Redirect**  
   - Issues `curl` HEAD/GET requests with a configurable timeout.  
   - Marks HTTP 2XX/3XX or downloadable headers as **active**.  
   - On HTTP 302, fetches the `Location:` header and updates to the final stream URL (2–3 s resolution).  
   - Retries poor connections automatically before marking failures.

4. **Resolution Conversion & Duplicate Filtering**  
   - Retrieves raw resolution (e.g., “720x480”) and converts to user-friendly labels.  
   - Detects identical URLs and retains the entry with the highest detected quality.

5. **Real-Time UI & ETA**  
   - Displays a color-coded progress bar, spinner, channel count, and ETA.

6. **Output & VLC Launch**  
   - Writes active channels to `output.m3u` with clean metadata.  
   - Optionally prompts to open the result in VLC Media Player.  
   ![VLC Launch Prompt](https://github.com/user-attachments/assets/ed21b368-da5c-4040-9ca0-30f4c6e80bc4)

---

## Features

- Advanced connectivity and speed diagnostics  
- Smart URL scanning with ultra-fast auto-redirect  
- Dynamic resolution detection and labeling  
- Intelligent duplicate consolidation  
- Colorized, UTF-8 console UI with ETA and logs  

---

## Optional Features

- **VLC Auto-Launch:** Prompt to open `output.m3u` in VLC after scanning  
- **Custom Timeouts:** Adjust per-URL timeout (up to 30 s)  
- **Performance Metrics:** Breakdown of active, inactive, timed-out, and host-empty failures  
- **Theming:** Light/dark color schemes and custom banners  
- **Academic Hooks (Research):** Built-in mocks and timers for instrumentation and deep-learning analysis  

---

## Research Edition File Structure

```text
Research Edition File Structure
├── References/                  
├── External Dependencies/       
├── Header Files/               # Public APIs & shared data structures
│   ├── All.h                   # Central include
│   ├── Channel_Scan.h          # Scan & retry logic
│   ├── Header.h                # M3U parsing
│   ├── Log.h                   # Logging API
│   ├── Network.h               # HTTP & geo-lookup
│   ├── Sorting.h               # Duplicate & quality logic
│   ├── System.h                # Speed test & timers
│   ├── UI.h                    # Console rendering & UTF-8
│   └── World.h                 # Global country code configure
├── Resource Files/             # Icons, color schemes
├── Source Files/               # <200 lines each
│   ├── Main.cpp                # Entry & orchestrator
│   ├── Channel_Scan.cpp        # Implements Channel_Scan.h
│   ├── Network.cpp             # Implements Network.h
│   ├── Sorting.cpp             # Implements Sorting.h
│   ├── System.cpp              # Implements System.h
│   ├── UI.cpp                  # Implements UI.h
│   ├── World.cpp               # Implements World.h
│   └── log.cpp                 # Implements Log.h
└── Other Files/                
    ├── input.txt               # Sample playlist
    ├── scan.log                # Generated by Research Edition
    ├── output.m3u              # Final playlist
    └── README.md               # This document
```

> **Research Edition** is perfect for anyone wanting to dive deep—whether that’s instrumenting performance tests, training deep-learning models on scan logs, or teaching students how each component works.

---

## Version History & Improvements

| Feature                  | Version 1.0                         | Version 1.1                                                        | Version 1.2                                                                                             | Version 1.3                                                                                                              |
|--------------------------|-------------------------------------|--------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------|
| Connectivity Check       | Single ping at startup              | Interactive re-check loop with detailed errors & countdown         | Continuous connectivity & 1 MB speed test + geo-detection                                               | Continuous check + geo-detection; auto-retry on failures, direct redirect resolution in 2–3 s                             |
| Internet Speed Test      | Not available                       | Measures download speed using 1 MB file (Mbps)                     | Measures download speed + displays country code                                                         | Same as v1.2 (1 MB test)                                                                                                 |
| Auto-Redirect            | Basic HTTP check                    | Basic redirection handling                                         | Improved HTTP 302 handling; updates channel URL to best stream                                          | Fetches final stream URLs in 2–3 s (vs. 22 s), boosting accuracy                                                         |
| Resolution Conversion    | Extracts resolution from channel name| Parses manual tags; converts to p-labels                           | Dynamic dimensions → “480p”, “1080p”, “4K”                                                               | Real-time detection + standardized labels with improved accuracy                                                         |
| Duplicate Handling       | Not available                       | Automatic removal by URL, keeping higher-quality streams           | Intelligent filtering: compares resolution & quality to retain best                                     | Same as v1.2, with integrated quality-based selection and `scan.log` details (Research Edition)                           |
| UI Enhancements          | Plain ASCII bar                     | Colorized UI with spinner, timestamps & countdown                  | Refined banners, detailed progress indicators, improved countdown timers                                | Vibrant banners, dynamic progress bars, live ETA, timestamped/color-coded logs                                           |
| VLC Launch Option        | Prompt at end                       | Retained with improved, colorized messaging                        | Integrated prompt to launch VLC                                                                         | Same as v1.2, with themed messages and confirmation logs                                                                  |
| Terminal Window Check    | Not implemented                     | Warns if window too small                                          | Warns if window too small for full UI                                                                    | Same as v1.2                                                                                                              |
| Built-In Logging         | Console only                        | Console only                                                       | Console + detailed summary                                                                              | `scan.log` in Research Edition with per-URL metrics, verbosity levels, retry/failure details                             |
| Research Edition         | –                                   | –                                                                  | –                                                                                                       | ✔️ Fully modular source breakdown, academic comments & instrumentation hooks                                              |

---

## Frequently Asked Questions

**Q: Can I rename the input/output files?**  
A: In v1.3, they’re fixed as `input.txt` and `output.m3u`. You can modify the source code to change this.

**Q: How are redirects handled?**  
A: On HTTP 302, the scanner parses the `Location:` header and follows it to the direct stream URL—resolving in 2–3 s instead of 22 s.

**Q: What if my connection is unstable?**  
A: The tool retries failed URL checks before marking a channel inactive.

**Q: How does duplicate filtering work?**  
A: Channels with identical URLs are compared by detected resolution; only the highest-quality entry is kept.

---

## Demo & UI Preview

**Test Demo (using Vietnamese IPTV list)**

1. **Internet Measurement + Country + Scanning**  
   ![Measurement & Scanning](https://github.com/user-attachments/assets/db99b502-15bf-41d3-b7a3-41050a834220)

2. **Scan Summary + Duplicate Removal**  
   ![Scan Summary](https://github.com/user-attachments/assets/9f8b971d-c228-452c-a288-fb6e9075ceea)
   ![Scan Summary + Asking for VLC](https://github.com/user-attachments/assets/3bd7038c-88d8-44df-964a-683ad622cb15)

3. **Asking to Open VLC (Optional)**  
   ![VLC Launch Prompt](https://github.com/user-attachments/assets/ed21b368-da5c-4040-9ca0-30f4c6e80bc4)

4. **`scan.log` Output (Research Edition)**  
   ![Log File](https://github.com/user-attachments/assets/3898d5db-963f-4b64-8294-9db27adfdfbb)

---

## License & Credits

Developed by **ShouNLAK**  
Open-source project · Educational & personal use only.  
Contributions, bug reports, and pull requests are welcome.
