# Online IPTV Channel Scanner (Windows - Standard C)

> **Version:** 1.4.0 (Unified)  
> **Release Date:** 25/06/2025  
> **Author:** [ShouNLAK](https://github.com/ShouNLAK)  
> **License:** Open Source (see below)

---

## 🚀 Overview

**Online IPTV Channel Scanner** is a high-performance, multi-threaded tool for validating and optimizing IPTV playlists (M3U format). Built in standard C for Windows and Linux, it rapidly checks stream URLs in parallel, measures network performance, and filters out inactive or duplicate channels—all with a modern, dynamic console UI.

**From v1.4 onward, all features from the Normal and Research Editions are unified in a single, maintainable codebase.**

---

## 📺 Live Demo

<!-- Replace the placeholder below with your GIF -->
<p align="center">
  <img src="demo.gif" alt="Live Demo of IPTV Channel Scanner" width="700"/>
  
  > *Reminder:* If the GIF not loaded, please watch it via Demo.gif
</p>

---

## 🗂️ Table of Contents

- [Overview](#-overview)
- [Live Demo](#-live-demo)
- [Features](#-features)
- [How It Works](#-how-it-works)
- [Quick Start](#-quick-start)
- [Building & Running](#-building--running)
- [Output Files](#-output-files)
- [Modules & Architecture](#-modules--architecture)
- [Repository Structure](#-repository-structure)
- [Version History](#-version-history)
- [FAQ](#-faq)
- [License & Credits](#-license--credits)

---

## ✨ Features

- **Ultra-fast Multi-threaded Scanning** — Leverages all CPU cores for parallel URL checks.
- **Modern Console UI** — Colorized, real-time progress bar, spinner, ETA, and status indicators.
- **Comprehensive Diagnostics** — Internet speed test, connectivity check, and geo-location.
- **Smart Duplicate Filtering** — Keeps only the highest-quality stream for each channel.
- **Dual-format Logging** — Human-readable `log.txt` and machine-friendly `log.csv`.
- **Auto-redirect Handling** — Follows HTTP redirects to resolve final stream URLs.
- **Ready-to-use Output** — Cleaned `output.m3u` playlist for any IPTV player.
- **Cross-platform** — Standard C, works on Windows and Linux.

---

## ⚙️ How It Works

1. **Connectivity & Speed Test:**  
   - Pings `google.com` to verify internet access.
   - Downloads a test file to measure download speed (Mbps).
   - Performs IP geo-lookup for country detection.

2. **Playlist Parsing:**  
   - Reads `input.txt`, extracts channel names and URLs.

3. **Concurrent Channel Scanning:**  
   - Launches multiple threads to test streams in parallel.
   - Marks channels as **Live** (HTTP 2XX/3XX) or **Inactive**.

4. **Duplicate & Quality Filtering:**  
   - Removes duplicate URLs, keeping the best quality stream.

5. **Logging & Output:**  
   - Generates `output.m3u`, `log.txt`, and `log.csv` with detailed results.

6. **Dynamic UI:**  
   - Displays a vibrant, color-coded progress bar, spinner, and live stats.

---

## ⚡ Quick Start

1. **Clone the repository:**
    ```bash
    git clone https://github.com/ShouNLAK/Check-Online-IPTV.git
    cd Check-Online-IPTV
    ```

2. **Place your M3U playlist as `input.txt` in the project root.**

3. **Build and run (see below for details).**

---

## 🛠️ Building & Running

### Method 1: Using Git Branches (Recommended)

1. **Install prerequisites:**  
   - Windows 10+ or Linux  
   - C17 compiler (GCC/Clang)  
   - `git`

2. **Checkout the desired version:**
    ```bash
    git checkout Version-1.4
    ```

3. **Compile:**
    ```bash
    gcc src/*.c -o IPTVScanner.exe -Iinclude
    ```

4. **Run:**
    ```bash
    ./IPTVScanner.exe
    ```

### Method 2: From Versioned Folders (Alternative)

- Navigate to the version folder and compile as above.

---

## 📄 Output Files

- **`output.m3u`** — Cleaned, validated playlist (for VLC, Kodi, etc.)
- **`log.txt`** — Human-readable scan log (status, resolution, URL)
- **`log.csv`** — Structured log for data analysis (Excel, scripts)

---

## 🧩 Modules & Architecture

| Module              | Responsibility                                      |
|---------------------|-----------------------------------------------------|
| `main.c`            | Orchestrates workflow, UI, and cleanup              |
| `system.c`          | Connectivity, speed test, geo-lookup                |
| `network.c`         | Network requests, HTTP/redirect handling            |
| `channel_scan.c`    | Core scan logic, threading, progress updates        |
| `sorting.c`         | Duplicate removal, quality selection                |
| `ui.c`              | Console UI, banners, color, progress bar            |
| `world.c`           | Country code lookup                                 |

---

## 📁 Repository Structure

```
/                  ← root
├── include/       ← Public headers
│   ├── all.h
│   ├── ui.h
│   ├── network.h
│   ├── system.h
│   ├── channel_scan.h
│   ├── sorting.h
│   └── world.h
│
├── src/           ← Implementation (.c files)
│   ├── main.c
│   ├── ui.c
│   ├── network.c
│   ├── system.c
│   ├── channel_scan.c
│   ├── sorting.c
│   └── world.c
│
├── input.txt      ← Input playlist
├── output.m3u     ← Cleaned output playlist
├── log.txt        ← Human-readable log
└── log.csv        ← Structured data log
```

---

## 🕒 Version History

| Version | Highlights                                                                                  |
|---------|--------------------------------------------------------------------------------------------|
| 1.0     | Basic sequential scan, console output                                                      |
| 1.1     | Interactive connectivity check, speed test, color UI                                       |
| 1.2     | Geo-detection, improved redirect, duplicate filtering by resolution                        |
| 1.3     | Research Edition: advanced UI, ultra-fast redirect, scan.log                               |
| 1.4     | **Unified codebase, multi-threaded scanning, dual-format logging, modular architecture**   |

---

## ❓ FAQ

**Q: Can I rename the input/output files?**  
A: Filenames are hardcoded for simplicity. You can change them in the source code.

**Q: How does multi-threading improve speed?**  
A: Multiple channels are checked in parallel, dramatically reducing total scan time.

**Q: What’s the difference between `log.txt` and `log.csv`?**  
A: `log.txt` is for human review; `log.csv` is for data analysis (Excel, scripts).

---

## ⚠️ Disclaimer

This tool is for **technical validation, education, and personal use only**.  
- **No copyrighted or unauthorized content is provided or endorsed.**
- Users are responsible for the legality of their playlists and streams.
- For legal, public IPTV playlists, see [IPTV-org on GitHub](https://github.com/iptv-org/iptv).

---

## 📝 License & Credits

Developed by **ShouNLAK**.  
Open-source for educational, research, and personal use.

Feel free to fork, adapt, and cite in your research.  
Pull requests, issues, and academic citations are welcome!
