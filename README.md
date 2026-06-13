# 🌟 Check-Online-IPTV Scanner

> **Author:** [ShouNLAK](https://github.com/ShouNLAK)  
> **License:** Open Source  
> **Current Flagship Version:** 2.0.0 (Enterprise GUI Edition)

![Demo CLI](Demo.gif)

---

## 🚀 Overview

**Check-Online-IPTV** is a comprehensive suite of tools built to rapidly validate, analyze, and optimize IPTV playlists (M3U / TXT). What started as a lightweight C-based terminal script has evolved into a robust, enterprise-grade Graphical User Interface (GUI) application.

This repository serves as the home for **all versions** of the IPTV Scanner. Whether you need a blazing-fast, lightweight CLI for Linux servers, or a visually stunning, multi-threaded GUI, you will find it here.

---

## 🌟 The Flagship: Version 2.0 (Enterprise GUI)

Version 2.0 represents a massive evolutionary leap. Built from the ground up using **C# and .NET MAUI**, it is a true **Cross-Platform** application supporting **Windows, macOS, iOS, and Android**. It introduces unparalleled accuracy by abandoning simple HTTP checks in favor of deep **FFprobe** Audio/Video stream decoding.

### ✨ Key Highlights of V2.0:
- **Cross-Platform Support:** Fully supports all major operating systems (Windows, macOS, iOS, Android).
- **Professional GUI & DataGrid:** Monitor connection latency (ms), HTTP codes, resolutions, and codecs in real-time.
- **Enterprise-Grade Security:** A 4-layer defense system (URI parsing, SSRF loopback protection, sensitive port blocking, and malware extension filtering) blocks malicious URLs before they are pinged.
- **Deep FFprobe Integration:** Detects fake 200 OK responses by verifying actual `h264`, `hevc`, and `aac` streams. **100% Accuracy**.
- **Robust 3-Try Loop:** Combats network congestion. If a stream times out, the engine intelligently re-queues it up to 3 times, preventing false negatives.
- **Auto-VLC Launch:** Cleans your playlist into a `*_checked.m3u` file and prompts you to instantly play the results in VLC Media Player.

### ⚙️ How It Works (Detailed Workflow):
1. **File Parsing:** The user loads an `.m3u` or `.txt` file into the app. The engine parses the raw text and extracts the stream URLs.
2. **State Initialization:** All channels are reset to their default state (clearing previous HTTP codes, latency, and codecs).
3. **Multi-Threaded HTTP Ping:** The scanner dispatches multiple asynchronous tasks (`async/await`) to send HTTP GET requests to the servers. It stops the stopwatch the moment the first byte of headers is received, recording the exact **Connection Latency (ms)**.
4. **Auto-Redirect Resolution:** If the server returns a `301/302 Redirect`, the engine captures the final `Direct URL` and updates the DataGrid in blue text.
5. **Deep AV Analysis (FFprobe):** If the HTTP status is `200 OK`, the app spawns a silent `ffprobe` process to dissect the stream's payload. It extracts the exact Video Codec, Audio Codec, and Resolution.
6. **Smart Validation:** If `ffprobe` times out or fails to find an AV stream, the app retries the entire process up to 3 times. Only streams that successfully return valid AV data at least once are marked as **Live**.
7. **Export & Play:** Valid channels are compiled and exported to a clean `*_checked.m3u` file. The app then optionally invokes VLC Media Player to test the playlist immediately.

### 📦 Packaged Releases
Version 2.0 is fully packaged as a standalone executable. You do not need to compile the code yourself. Simply go to the **Releases** tab, download the ZIP file for your operating system, extract it, and run the executable!

*(For developers: To compile C# source code, please check the source code inside the V2.0 project folder and run `dotnet publish`).*

---

## 🔒 Security & Privacy Guarantee

When scanning hundreds of unknown URLs from the internet, security is a paramount concern. **Check-Online-IPTV Version 2.0** guarantees 100% safety for your machine through a strict 4-Layer Defense mechanism:

1. **Anti-SSRF & Port Scanning Protection:** The engine blocks loopback addresses (like `localhost` or `127.0.0.1`) and sensitive network ports (e.g., `22 SSH`, `3306 MySQL`), preventing hackers from using malicious playlists to scan or attack your own local network.
2. **Zero Code Execution (No Malware):** The application strictly uses C# `Uri` parsing and `ResponseHeadersRead` to only fetch HTTP headers. It automatically intercepts and kills connections to files with malicious extensions (e.g., `.exe`, `.bat`, `.sh`, `.vbs`, `.apk`, `.ps1`). 
3. **Sandboxed Media Probing:** It spawns `ffprobe` exclusively to parse multimedia metadata (codecs/resolutions). Malware or malicious scripts embedded in media containers cannot be triggered.
4. **100% Local Processing:** Your playlists (`.m3u` / `.txt`) are processed entirely offline on your own machine. There is **zero telemetry**, no phoning home, and no data collection.

---

## 💻 The Classic Lineage: Versions 1.0 to 1.4 (CLI Editions)

For users who prefer the terminal, automated scripts, or need to run checks on a headless Linux server, the **Version 1.x series** remains fully available and functional. Written in standard **C**, these versions are lightweight and exceptionally fast.

### 📜 Classic Version History
- **Version 1.0:** Basic sequential scanner.
- **Version 1.1:** Added connectivity checks, speed test, and colored terminal UI.
- **Version 1.2:** Introduced geo-detection, redirect tracing, and duplicate filtering.
- **Version 1.3 (Research Edition):** Advanced diagnostics, ultra-fast redirects, and machine-readable `scan.log`.
- **Version 1.4 (Unified CLI):** The ultimate CLI build. Multi-threaded scanning, dual-format logging (`log.txt` & `log.csv`), and a highly modular C architecture.

*(To compile the CLI versions, simply navigate to the respective V1.x branch or folder and compile using `gcc` / `clang`.)*

---

## ⚡ Which Version Should I Use?

| Requirement | Recommended Version | Technology | OS Support |
|-------------|---------------------|------------|------------|
| I want a beautiful GUI, 100% accuracy, and cross-platform support. | **Version 2.0** | C# / .NET MAUI | Windows, macOS, iOS, Android |
| I want to run tests on a Linux VPS / Terminal. | **Version 1.4** | C (GCC) | Windows, Linux, macOS |
| I need to pipe outputs into bash scripts. | **Version 1.4** | C (GCC) | Windows, Linux, macOS |

---

## ⚠️ Disclaimer

This tool is strictly for **technical validation, network diagnostics, and educational purposes**.  
- **No copyrighted, premium, or unauthorized content is provided or endorsed.**
- Users are solely responsible for the legality of the playlists they scan.

---

## 📝 License & Credits

Developed and maintained by **ShouNLAK**.  
Open-source for educational, research, and personal use. Feel free to fork, contribute, or adapt into your own workflow!
