
# Online IPTV Channel Scanner (Standard C) - Windows

**A powerful IPTV channel scanner built in Standard C for Windows.**  
This project validates IPTV channel lists by testing stream URLs, measuring network performance, and filtering out inactive or duplicate channels—all while delivering a visually appealing, dynamic user interface.

> **Latest : Version 1.2.0 | Release on 10/06/2025** 

---

## Table of Contents

- [Introduction](#introduction)
- [Prerequisites](#prerequisites)
- [How It Works](#how-it-works)
- [Features](#features)
- [Optional Features](#optional-features)
- [Frequently Asked Questions](#frequently-asked-questions)
- [Demo & UI Preview](#demo--ui-preview)
- [License & Credits](#license--credits)

---

## Introduction

The Online IPTV Channel Scanner is a student-developed tool designed to help you quickly verify and optimize IPTV channel lists. It processes an M3U playlist by performing several key tasks:
- Verifying the connectivity and measuring internet speed.
- Parsing channel information from the playlist.
- Testing each channel URL (following redirects to the best quality stream when necessary).
- Converting raw resolution data into clear, user-friendly labels.
- Automatically filtering out duplicate or inactive channels.
- Presenting a polished, colorized user interface with real-time progress updates and estimated time remaining (ETA).

After scanning, only active channels are compiled into a clean "output.m3u" playlist—ready for immediate use with media players like VLC.

---

## Prerequisites

- **Operating System:** Windows 10 or later
- **Required Tools:**  
  - `curl` (used for HTTP requests, speed tests, and URL redirection handling)
  - `ping` (for checking internet connectivity)
  - An M3U file named `input.txt` (formatted as described below)
  - **Optional:** VLC Media Player – to automatically launch the output playlist

- **Input File Format:**  
  Each channel block in your `input.txt` should follow this structure:
  ```m3u
  #EXTM3U
  #EXTINF:-1,Channel Name (HD)
  https://stream.example.com/…
  ```
  The channel name is extracted from the text following the final comma, and any resolution or quality information (if available) should be enclosed in the last pair of parentheses.

---

## How It Works

1. **Connectivity & Internet Speed Testing:**  
   Before scanning begins, the tool pings `google.com` to ensure an active internet connection. It then downloads a 1 MB file using `curl` to estimate your download speed in Mbps. An IP lookup is also performed to detect your country code, which can help diagnose geo-restriction issues.

2. **Input File Processing:**  
   The scanner opens “input.txt”, skips the header line, and counts each channel entry marked by the `#EXTINF` tag. It then processes each entry to extract the channel’s name and any optional resolution or quality details, followed by its corresponding URL.

3. **URL Scanning & Auto-Redirect Handling:**  
   For every URL that begins with “http://” or “https://”, the tool issues a `curl` command with a fixed timeout to test HTTP status.  
   - If the URL returns a 2XX/3XX response or a downloadable attachment header, the channel is marked as **active**.  
   - When a channel URL responds with an HTTP 302 (redirection), the tool automatically retrieves the new URL to point to the best quality stream—ensuring that delays due to redirects are minimized.
   - In cases of poor connectivity, the tool employs a retry mechanism to improve accuracy.

4. **Resolution Conversion & Duplicate Filtering:**  
   The scanner dynamically retrieves raw resolution data (for example, "720x480") from channel responses and converts it into standard labels such as "480p," "1080p," or "2160p – 4K."  
   It then examines channels with identical URLs and intelligently filters out duplicates—retaining only the entry with the highest quality or most complete resolution information.

5. **Real-Time Progress & ETA Indicators:**  
   Throughout the scan, the UI displays a dynamic progress bar with a spinner, current channel count, and an estimated time remaining, all rendered in vibrant colors for clarity.

6. **Output Compilation & Optional VLC Launch:**  
   Only channels verified as active are written to “output.m3u” with clean metadata including resolution labels. Upon completion, the tool offers an optional prompt to immediately open the playlist in VLC Media Player.

---

## Features

- **Advanced Connectivity & Speed Testing:**  
  Verifies internet access, measures download speed, and performs geo-detection to help pinpoint potential geo-restriction issues.

- **Smart URL Scanning with Auto-Redirect:**  
  Automatically follows HTTP 302 redirects to retrieve the best available stream quality while minimizing delay.

- **Dynamic Resolution Conversion:**  
  Converts raw resolution data into standardized, user-friendly labels for clarity in the final playlist.

- **Intelligent Duplicate Filtering:**  
  Consolidates duplicate channel entries by evaluating quality details to retain only the optimal stream.

- **Enhanced User Interface:**  
  Offers a vivid, colorized display with real-time progress bars, countdown timers, ETA estimates, and detailed status messages for a polished scanning experience.

---

## Optional Features

- **VLC Launch Prompt:**  
  At the end of the scan, an optional prompt allows you to automatically open the optimized “output.m3u” playlist in VLC Media Player.

- **Customizable Timeout Settings:**  
  The response timeout for each URL can be adjusted (up to a maximum of 30 seconds) to suit your network conditions.

- **Performance Metrics:**  
  Displays overall processing time along with a comprehensive summary of active, inactive, and timed-out channels.

---

## Frequently Asked Questions

**Q: Can I change the file names for input and output?**  
A: In the current version, the scanner always reads from `input.txt` and writes to `output.m3u`. You can modify these within the source code if needed.

**Q: How does the auto-redirect mechanism work?**  
A: When encountering an HTTP 302 status code, the scanner automatically retrieves the “Location:” header and updates the URL to point to the direct stream, improving both speed and accuracy.

**Q: What if my internet connection is poor?**  
A: The tool first checks connectivity and measures your download speed. If a poor connection is detected, it will retry URL checks before marking a channel as inactive.

**Q: How is duplicate filtering handled?**  
A: After scanning, duplicate channels (those with identical URLs) are compared based on available quality and resolution data; only the highest quality entry is retained in the final playlist.

**Q: Where can I use the generated playlist?**  
A: The final `output.m3u` file is compatible with popular media players like VLC, Kodi, and Plex, making it easy to enjoy your verified channels immediately.

---

## Demo & UI Preview

Below are example screenshots that illustrate the tool’s modern, dynamic interface:

- **Dynamic Progress & Countdown:**  
  A vibrant progress bar with a spinner and real-time countdown helps you follow the scanning process.

- **Scan Summary:**  
  An organized, colorized table shows the status of each channel, including active, inactive, and timed-out entries, along with resolution details.

- **VLC Launch Prompt:**  
  After scanning, a clear prompt allows you to automatically open the optimized playlist in VLC Media Player.

> *Note: New release has been confirmed, the media will be updated as soon as possible, right after the release*

---

## License & Credits

Developed by **ShouNLAK**  
This tool is provided as-is for educational and personal use.

Contributions, suggestions, and improvements are welcome via GitHub issues or pull requests.
