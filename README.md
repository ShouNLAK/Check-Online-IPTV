# Online IPTV Channel Scanner – Update 1.2 (C17 Standard)

**A powerful IPTV channel scanner built in Standard C for Windows. Now featuring advanced connectivity checks, smart quality selection, geo-block detection, auto-redirect optimization, intelligent duplicate filtering, and an enhanced UI with dynamic progress and ETA indicators.**

---

## Table of Contents

- [Introduction](#introduction)
- [Warning & Prerequisites](#warning--prerequisites)
- [How It Works](#how-it-works)
- [New & Enhanced Features](#new--enhanced-features)
- [Optional Features](#optional-features)
- [Frequently Asked Questions](#frequently-asked-questions)
- [Demo & UI Preview](#demo--ui-preview)
- [License & Credits](#license--credits)

---

## Introduction

This tool scans IPTV channel lists by verifying and testing URLs, measuring your internet speed, and filtering out inactive or duplicate entries—all while providing an engaging user experience. Update 1.2 (C17 Standard) introduces comprehensive improvements such as geo-restriction detection (via country code lookup), smart quality selection with automated resolution retrieval, an auto-redirect mechanism, duplicate filtering with high-quality preference, and a dynamic UI with real-time progress and ETA indicators. Active channels are exported into an optimized M3U playlist ready for immediate use (e.g., with VLC).

---

## Warning & Prerequisites

- **Platform:**  
  Built exclusively for Windows, this tool leverages Command Prompt utilities (using `curl`, `findstr`, and Windows-specific sleep functions) to execute its tasks.
  
- **Input File Format:**  
  - **Channel Name:** Extracted from the content following the final comma in each `#EXTINF` line.  
  - **Resolution/Quality (Optional):** If provided, this information should be enclosed within the last pair of parentheses `()`.
  
- **Output File:**  
  The scanner produces an `output.m3u` file that contains only active channels—with the direct best-quality URL and any available resolution/quality details.

- **Disclaimer:**  
  This project is a student endeavor. Although fully functional, it may not match the efficiency of professional-grade IPTV tools. Use it for educational and personal projects only.

---

## How It Works

1. **Connectivity & Internet Speed Testing:**  
   - **Speed Test:** Before beginning the channel scan, the tool downloads a 1 MB file using `curl` to measure your download speed in Mbps.  
   - **Geo-Detection:** Your country code is retrieved through an IP lookup, assisting in the identification of geo-restricted channels that might otherwise be blocked in your region.
   
2. **Input File Processing:**  
   The input file (typically `input.txt`) is parsed to count and extract channel entries. Each entry comprises an `#EXTINF` line for metadata—where the channel name and (optional) resolution are derived—and a URL line.

3. **URL Scanning, Auto-Redirect & Retry Mechanism:**  
   - **HTTP Status Check:** For each channel, the scanner issues a `curl` command to check the URL’s HTTP status. A response in the 2XX/3XX range marks the channel as **ACTIVE**.  
   - **Auto-Redirect:** If a URL returns a redirection (HTTP 302), the tool automatically scans the header for the new location and updates the URL to point to the best quality stream.  
   - **Retry on Failure:** Should a channel fail due to a poor connection, the system triggers a retry mechanism before finalizing its status.
   
4. **Resolution Conversion & Duplicate Filtering:**  
   - **Resolution Conversion & Quality Retrieval:**  
     - **Version 1.1:** Previously, the tool could only convert resolution strings provided manually in the channel name (e.g., "Channel Name (720x480)" converting to "480p").  
     - **Version 1.2:** The scanner now queries each channel's URL directly to detect the best available quality. Raw resolution values (e.g., "720x480") are retrieved and automatically converted into user-friendly labels (like "480p"). This approach removes the need for manually inputting resolution details, ensuring that the displayed quality is both accurate and up-to-date.
     
   - **Duplicate Handling:** Duplicates are detected based on identical URLs. If duplicates exist, the scanner compares available quality details and retains the entry with the highest available stream.

5. **Real-Time Progress & ETA Indicators:**  
   A dynamic progress bar with a spinner, current channel count, and estimated time remaining (ETA) provide continuous feedback during the scan.

6. **Output Compilation:**  
   Only channels verified as active—and filtered to include the best quality version in case of duplicates—are written into the final `output.m3u` file.

7. **Optional Cleanup & Launch:**  
   After processing, additional optional features allow the user to launch the output file in VLC immediately and view a final summary of the scan.

---

## New & Enhanced Features

- **Internet Speed & Geo-Detection:**  
  - Measures your download speed to help diagnose network performance.  
  - Retrieves and displays your country code to help identify geo-restricted channels.

- **Smart Quality Selection, Auto-Redirect & Resolution Conversion:**  
  - Automatically chooses the best quality available for each channel by querying the channel’s URL directly.  
  - Redirects provider URLs to the direct best-quality stream and retries connections automatically if the initial attempt fails.  
  - Converts raw resolution values into standardized, user-friendly labels (e.g., "720x480" becomes "480p"), even if the input lacks manual resolution metadata.

- **Duplicate Filtering:**  
  Ensures that duplicate channels (determined by identical URLs) are consolidated by retaining the entry with the most complete or highest quality information.

- **Enhanced User Interface & ETA Calculation:**  
  - A modern UI featuring dynamic progress indicators, spinners, and real-time ETA estimates.  
  - Detailed scan summaries display totals for active, inactive, and timed-out channels.

---

## Optional Features

- **VLC Launch Prompt:**  
  Once scanning is complete, the tool offers the option to immediately launch the generated `output.m3u` playlist in VLC Media Player.

- **Customizable Timeout Configuration:**  
  You can adjust the response timeout for each URL (up to a maximum of 30 seconds) according to your network conditions.

- **Detailed Performance Metrics:**  
  Displays overall processing time alongside a comprehensive breakdown of channel statuses.

---

## Frequently Asked Questions

### Version Comparison

<table style="border-collapse: collapse; width: 100%; font-family: Arial, sans-serif;">
  <thead>
    <tr>
      <th style="background-color: #4CAF50; color: white; text-align: center; padding: 8px;">Feature</th>
      <th style="background-color: #4CAF50; color: white; text-align: center; padding: 8px;">Version 1.0</th>
      <th style="background-color: #4CAF50; color: white; text-align: center; padding: 8px;">Version 1.1</th>
      <th style="background-color: #4CAF50; color: white; text-align: center; padding: 8px;">Version 1.2</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td style="padding: 8px;"><strong>Initial URL Check</strong></td>
      <td style="padding: 8px;">Basic HTTP check</td>
      <td style="padding: 8px;">Added redirection handling</td>
      <td style="padding: 8px;">Auto-redirect to best-quality URLs</td>
    </tr>
    <tr>
      <td style="padding: 8px;"><strong>Retry Mechanism</strong></td>
      <td style="padding: 8px;">None</td>
      <td style="padding: 8px;">Minimal retry on failure</td>
      <td style="padding: 8px;">Automatic retry on poor connections</td>
    </tr>
    <tr>
      <td style="padding: 8px;"><strong>Duplicate Filtering</strong></td>
      <td style="padding: 8px;">Not available</td>
      <td style="padding: 8px;">Basic duplicate removal</td>
      <td style="padding: 8px;">Intelligent duplicate filtering with quality check</td>
    </tr>
    <tr>
      <td style="padding: 8px;"><strong>Internet Connectivity Check</strong></td>
      <td style="padding: 8px;">Single check at start</td>
      <td style="padding: 8px;">Improved connectivity re-check (1 sec)</td>
      <td style="padding: 8px;">Continuous connectivity &amp; speed test + geo-detection</td>
    </tr>
    <tr>
      <td style="padding: 8px;"><strong>Resolution Conversion</strong></td>
      <td style="padding: 8px;">Not present</td>
      <td style="padding: 8px;">Converts manually entered resolution info</td>
      <td style="padding: 8px;">Dynamically retrieves and converts best-quality resolution from URL</td>
    </tr>
    <tr>
      <td style="padding: 8px;"><strong>UI &amp; ETA</strong></td>
      <td style="padding: 8px;">Basic output</td>
      <td style="padding: 8px;">Enhanced progress reporting</td>
      <td style="padding: 8px;">Advanced UI with dynamic progress bar, spinner, &amp; ETA</td>
    </tr>
    <tr>
      <td style="padding: 8px;"><strong>VLC Launch Option</strong></td>
      <td style="padding: 8px;">Not available</td>
      <td style="padding: 8px;">Basic prompt</td>
      <td style="padding: 8px;">Integrated prompt to launch VLC automatically</td>
    </tr>
  </tbody>
</table>

### Additional Q&A

**Q: What improvements do I see in Update 1.2?**  
**A:**  
- The tool now retries channel tests if a poor connection causes an initial failure.  
- It auto-redirects from provider URLs to direct best-quality streams and fetches the best resolution data in real time—converting raw resolution values like "720x480" to "480p".  
- The UI now provides detailed progress indicators and ETA estimates, offering a smoother and more informative scanning experience.

**Q: How does the duplicate filtering work?**  
**A:**  
Duplicates are detected by comparing channel URLs. When duplicates are found, the tool compares available quality information and retains only the entry with the highest stream quality, ensuring a cleaner final playlist.

**Q: What measures are taken for geo-blocked channels?**  
**A:**  
The tool performs an IP lookup to retrieve your country code. This is displayed during the scanning process to help you identify and troubleshoot potential geo-restriction issues.

**Q: How is a channel evaluated as active?**  
**A:**  
Each channel’s URL is tested with `curl`. A channel is marked as active if it returns a valid HTTP 2XX/3XX status code or triggers a download. Channels that fail are retried before being marked inactive or timed out.

**Q: Where can I download an IPTV channel list for testing?**  
**A:**  
A good starting point is the [IPTV-org repository on GitHub](https://github.com/iptv-org/iptv), which offers an extensive collection of channel lists.

**Q: What if I encounter issues or have suggestions for improvements?**  
**A:**  
Feel free to post issues or submit pull requests on the GitHub repository. Contributions and constructive feedback are highly appreciated as this is a continually evolving project.

---

## Demo & UI Preview

Below are example screenshots that highlight the tool’s new features and enhanced UI:

- **Internet Speed & Geo-Detection:**  
  ![No Internet Connection](https://github.com/user-attachments/assets/f975af81-0905-4c7f-a4ef-4d5643ebb3cb)

- **Scanning Process & Speed Test:**  
  ![Scanning Progress](https://github.com/user-attachments/assets/544d36c1-e48b-4619-b871-4afc7a7ad85c)

- **Scan Summary with ETA & Conversion of Resolution:**  
  ![Scan Summary](https://github.com/user-attachments/assets/4d3e856d-4f11-40cf-946e-1b426e855b29)  
  ![Processing Time](https://github.com/user-attachments/assets/a2c9bfc6-b7ba-43c8-8519-7ee2f2693ca8)

- **VLC Launch Prompt:**  
  ![VLC Launch](https://github.com/user-attachments/assets/9be94a7d-379d-4310-9fdd-07866636b897)

---

## License & Credits

Developed by **ShouNLAK**  
This tool is provided as-is for educational and personal use. Contributions, suggestions, and improvements are welcome via GitHub issues or pull requests.

---
