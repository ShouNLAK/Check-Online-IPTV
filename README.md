# Online IPTV Channel Scanner – Version 1.0

**A simple IPTV playlist validator in C for Windows.**  
Reads an M3U file (`input.txt`), checks each stream URL’s HTTP status, and outputs a cleaned playlist (`output.m3u`) containing only the active channels.  
> The code is available in the **Version** folder | Released on 07/06/2025

---

## Table of Contents

- [Introduction](#introduction)  
- [Prerequisites](#prerequisites)  
- [How It Works](#how-it-works)  
- [Features](#features)  
- [Clone from GitHub](#clone-from-github)  
- [Frequently Asked Questions](#frequently-asked-questions)  
- [License & Credits](#license--credits)  

---

## Introduction

Version 1.0 is the inaugural release of the IPTV Channel Scanner. It performs a basic HTTP-status check on each URL in your M3U playlist and writes only streams that return a 2XX/3XX status (or a downloadable attachment) into `output.m3u`.

---

## Prerequisites

- **OS**: Windows 10 or later  
- **Required Tools** (must be in your `%PATH%`):  
  - `curl` (for HTTP requests)  
  - `ping` (for connectivity check)  
  - `vlc` (optional, for playback launch)  
- **Input file**:  
  A valid M3U playlist named `input.txt`, where each channel block looks like:
  ```m3u
  #EXTINF:-1,Channel Name (HD)
  https://stream.example.com/…
  ```

---

## How It Works

1. **Internet Connectivity Check**  
   On start, the program pings `google.com`. If unreachable, it aborts.

2. **Playlist Parsing**  
   - Opens `input.txt`, skips the first line (`#EXTM3U`).  
   - Counts `#EXTINF` entries to allocate storage.  
   - Rewinds and reads each `#EXTINF` line, extracting:  
     - **Channel name** (text after the last comma)  
     - **Resolution/quality** (optional text inside `()`, e.g. “1080p” or “HD”)  
   - Reads the following URL line for each entry.

3. **URL Verification**  
   For each URL containing `http://` or `https://`:
   ```bat
   curl -I -L -s --max-time 15 "<URL>"
   ```
   - **Active** if HTTP status 200–399 or if `Content-Disposition: attachment` is detected  
   - **Timeout** if “Operation timed out” appears  
   - **Inactive** otherwise

4. **Progress Indicator**  
   A “TV scan” style spinner and ASCII progress bar update in real time.

5. **Sorting & Summary**  
   - Channels are sorted alphabetically; ties broken by ascending quality value.  
   - Prints a colorized table of ACTIVE, INACTIVE, and TIMEOUT entries, plus total scan time (`hh:mm:ss`).

6. **Output Generation**  
   Only **ACTIVE** channels are written to `output.m3u`, preserving resolution and numeric quality in the `#EXTINF` tag.

7. **Optional VLC Launch**  
   After the scan, you’re prompted:
   ```
   Do you want to open the output file in VLC? (y/n):
   ```
   Enter **y** to launch `output.m3u` in VLC.

---

## Features

- One-time internet check via `ping`  
- Parses M3U `#EXTINF` tags for name & resolution  
- Follows HTTP redirects (`-L`) and handles attachments  
- Hard-coded 15 s timeout per URL  
- Interactive progress bar with spinner  
- Sorts channels by name & quality  
- Colorized console summary of Active / Inactive / Timeout  
- Writes a clean `output.m3u` of working streams  
- Prompt to launch VLC at the end  

---

## Clone from GitHub

To grab the v1.0 scanner and run it locally:

```bash
# 1. Clone the repository
git clone https://github.com/ShouNLAK/Check-Online-IPTV.git

# 2. Change into the v1.0 source folder
cd Check-Online-IPTV/Version/"Check Online IPTV (Ver 1.0)"

# 3. Compile the scanner
gcc scanner.c -o IPTVScanner.exe

# 4. Place your M3U playlist in this directory as input.txt
#    (e.g. copy or rename your file accordingly)

# 5. Run the scanner
./IPTVScanner.exe

```

---

## Frequently Asked Questions

**Q: Can I change the input/output filenames?**  
A: Not in v1.0. It always reads `input.txt` and writes `output.m3u`.

**Q: Can I adjust the timeout per URL?**  
A: The timeout is fixed at 15 seconds (`--max-time 15`). You may increase it up to 30 seconds by editing the source.

**Q: Does it follow HTTP redirects?**  
A: Yes—`curl` is run with `-L` to follow up to 50 redirects by default.

**Q: How are resolutions handled?**  
A: If you include `(720p)` or `(HD)` in the channel name, the tool parses it, maps it to a numeric quality, and carries that metadata into `output.m3u`.

**Q: Why does it ping `google.com`?**  
A: To confirm basic internet connectivity before performing a lengthy scan.

---

## License & Credits

**Developed by ShouNLAK**  
Provided as-is for educational and personal use.  
Contributions, issues, and pull requests are welcome on GitHub.  

