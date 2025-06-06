
---

# Check Online for IPTV (Using Standard C)

**Experience efficient IPTV scanning and sorting for your region (Windows users only).**

## Table of Contents
- [Warning Before Use](#warning-before-use)
- [How It Works](#how-it-works)
- [Optional Features](#optional-features)
- [Frequently Asked Questions](#frequently-asked-questions)
- [Demo & UI Preview](#demo--ui-preview)

---

## Warning Before Use

Please note that this tool is a student project. While it may not match the efficiency of some professional-grade solutions, it delivers acceptable performance for enjoying IPTV. Designed exclusively for Windows, it leverages Command Prompt functionality.

**Input File Requirements:**
- **Channel Name:** Located after the final comma.
- **Resolution/Quality (Optional):** Any details enclosed between the last pair of parentheses `()`.

**Output:**
- The generated `output.m3u` file will include only the Channel Name, URL, and, when available, the Resolution or Quality.

---

## How It Works

1. **Internet Connectivity Check:**  
   The tool first verifies that an active internet connection is available, continuously scanning every second until connectivity is confirmed.

2. **File Handling:**  
   It opens the designated input file and counts the number of IPTV channels listed.

3. **URL Scanning:**  
   The program examines each channel’s URL to determine its status.

4. **Curl Command Verification:**  
   By executing the `curl` command through CMD, each URL is given up to 15 seconds to respond. URLs returning a 2XX/3XX response or prompting a download are marked as **ACTIVE**; all others are designated as **INACTIVE/TIMEOUT**.

5. **Result Recording:**  
   Information for each channel is compiled to present a comprehensive scanning summary.

6. **Output Generation:**  
   Only channels confirmed as active are exported to the final `output.m3u` file.

7. **Duplicate Filtering:**  
   If duplicate channels (based on the same URL) are detected, the tool removes redundant entries, keeping only the channel with the most complete information.

---

## Optional Features

- **VLC Launch:**  
  Prompt the user to automatically launch the output file in VLC media player upon completion.

- **Processing Duration:**  
  Calculate and display the total time taken to complete the scan.

- **Visual Progress Indicator:**  
  A dynamic progress bar enhances the processing experience.

- **Customizable Timeout:**  
  Users can adjust the response timeout threshold (up to a maximum of 30 seconds) if needed.

---

## Frequently Asked Questions

**Q: What improvements have been made from version 1.0 to 1.1?**  
**A:**  
- **Enhanced Connectivity:** The tool now monitors for an active internet connection every second.  
- **Duplicate Removal:** It actively filters out channels with duplicate URLs to streamline the list.  
- **Improved UI:** Significant upgrades have been made to the user interface for a cleaner, more engaging experience.

**Q: How is a channel classified as active or inactive?**  
**A:**  
A channel is considered active if it responds with a 2XX/3XX response or prompts a download when tested via the `curl` command. Otherwise, it is marked as inactive or timed out.

**Q: Is all the code essential?**  
**A:**  
While some parts of the code primarily enhance the UI and provide additional information, they collectively contribute to a more robust and user-friendly experience.

**Q: Where can I obtain the IPTV list?**  
**A:**  
If you're new to this, an excellent starting point is the [IPTV-org repository on GitHub](https://github.com/iptv-org/iptv).

---

## Demo & UI Preview

Below are some preview screenshots demonstrating the tool in action:

- **User Interface During Processing:**
  ![image](https://github.com/user-attachments/assets/f975af81-0905-4c7f-a4ef-4d5643ebb3cb)
  *No Internet Connection*

  ![image](https://github.com/user-attachments/assets/544d36c1-e48b-4619-b871-4afc7a7ad85c)
  *Scanning process + Internet Speed Measurement*


- **Scan Summary & Processing Time:**  
  ![image](https://github.com/user-attachments/assets/4d3e856d-4f11-40cf-946e-1b426e855b29)
  ![image](https://github.com/user-attachments/assets/a2c9bfc6-b7ba-43c8-8519-7ee2f2693ca8)


- **VLC Launch Prompt:**  
  ![image](https://github.com/user-attachments/assets/9be94a7d-379d-4310-9fdd-07866636b897)


---

<div align="center">
  <strong>Developed by ShouNLAK</strong><br>
  If you have any questions, feel free to ask.
</div>
