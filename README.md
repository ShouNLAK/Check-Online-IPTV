# 🌟 Check-Online-IPTV - Version 2.0 (Enterprise GUI)

Welcome to the source code of **Version 2.0**, the next-generation architecture for the Check-Online-IPTV project. 

This directory contains the **C# .NET MAUI** implementation of the scanner, bringing a massive upgrade in terms of User Experience (UX), Performance, and Absolute Accuracy compared to the classic C console versions.

---

## 🛡️ 100% Security & Safety Guarantee (4-Layer Defense)

In this Version 2.0 architecture, we have implemented ironclad security measures to ensure that your system remains 100% safe while scanning completely unknown, unverified, and potentially malicious URLs from the internet:

1. **Uri Parser & SSRF Protection:** Using native C# `Uri` validation, the engine rejects malformed links and blocks `localhost`/`127.0.0.1` (loopback) to prevent Server-Side Request Forgery (SSRF) attacks targeting your own machine.
2. **Anti Port-Scanning:** Hackers cannot use your machine as a proxy to attack others. Sensitive internal service ports (e.g., 21, 22, 23, 25, 3389, 3306, 1433) are strictly blacklisted.
3. **Malware Extension Radar:** By analyzing the exact `AbsolutePath`, the scanner blocks execution-prone file extensions (`.exe`, `.bat`, `.cmd`, `.msi`, `.sh`, `.vbs`, `.apk`, `.bin`, `.scr`, `.js`, `.ps1`, `.jar`, `.dll`, `.sys`, `.elf`, `.cpl`, `.wsf`) while safely permitting dynamic PHP portals and media files.
4. **Zero-Execution Media Probing:** We enforce `HttpCompletionOption.ResponseHeadersRead` to never download payloads to memory. Metadata is parsed through a sandboxed `ffprobe` instance that reads the multimedia container and terminates immediately.

You can scan 10,000 unknown URLs without any fear of malware, ransomware, or executable injection.

---

## 🎯 What's New in Version 2.0?

### 1. 🖥️ The Graphical User Interface (GUI)
Built on **.NET MAUI / XAML**, this version provides a stunning, desktop-grade interface with:
- **Real-Time DataGrid:** Columns for `Channel Name`, `Status`, `Response Code`, `Latency (ms)`, `Direct URL`, `Resolution`, `Video Codec`, and `Audio Codec`.
- **Integrated Live Terminal:** A beautifully embedded log console tracking background tasks in real time.
- **Dynamic Thread Control:** Start and Stop buttons instantly control the Task Cancellation Tokens without freezing the UI.

### 2. 🔍 Deep FFprobe Integration
HTTP `200 OK` is no longer enough to trick the scanner. V2.0 pipes every "Live" HTTP link directly into FFprobe. It extracts:
- **Video Codec:** `h264`, `hevc`, `vp9`, etc.
- **Audio Codec:** `aac`, `mp3`, etc.
- **Resolution Matrix:** `1920x1080`, `1280x720`, etc.
If the link responds `200 OK` but lacks a media stream, V2.0 flags it as **Dead**. This achieves **100% false-positive immunity**.

### 3. 🛡️ Smart 3-Try Loop (Anti-Congestion)
IPTV servers are notoriously unstable. V2.0 introduces an advanced **Retry Mechanism**:
- If a channel times out or drops the connection, the engine waits 1 second and retries.
- It attempts this up to **3 times**.
- If *any* of the 3 attempts succeeds, the channel is rescued and marked Live.

### 4. ⏱️ Millisecond Connection Latency
Using `System.Diagnostics.Stopwatch`, every HTTP request is timed to the exact millisecond (`ConnectionTimeMs`). You can now sort your final playlists to keep only the fastest, buffer-free servers.

### 5. 🔄 Auto-Redirect Tracer
Tokens and dynamic hostings often use HTTP 301/302 redirects. V2.0 automatically unwraps these and displays the final, direct streaming URL in **Blue** on the DataGrid.

### 6. 🎬 1-Click VLC Invocation
Upon finishing the scan, the app saves a `*_checked.m3u` file and automatically searches your Windows Environment for `vlc.exe`. With one click, your validated playlist begins playing instantly.

---

## 🏗️ Architecture & Source Code Layout

This project follows a strict **MVVM (Model-View-ViewModel)** Clean Architecture pattern to ensure future scalability:

- `Check-Online-IPTV.sln` — The main Visual Studio Solution file.
- **`/Models/`**
  - `ChannelItem.cs` — The core entity representing a single IPTV channel. Implements `INotifyPropertyChanged` for real-time DataGrid UI updates.
- **`/ViewModels/`**
  - `ScannerViewModel.cs` — The brain of the application. Orchestrates the file parsing, thread dispatching, and state management without ever blocking the UI thread.
- **`/Services/`**
  - `M3uParser.cs` — Parses raw `TXT` and `M3U` files, and generates cleaned `M3U` outputs.
  - `StreamChecker.cs` — The heavy-lifter. Contains the 3-Try Loop, `HttpClient` logic, and `ffprobe` process invocation.
  - `NetworkService.cs` — Handles global connectivity and speed-test diagnostics prior to scanning.
- **`/MainPage.xaml`** — The XAML layout defining the modern UI grid, colors, and layout elements.

---

## 💻 Developer Instructions

### Prerequisites
- **Visual Studio 2022** (v17.3 or newer)
- **.NET Multi-platform App UI development** workload installed.
- **FFprobe** must be installed on your machine and present in the system `PATH`.

### Compiling & Running
1. Open `Check-Online-IPTV.sln` in Visual Studio.
2. Select `Windows Machine` as your target.
3. Hit **F5** to build and run in Debug mode.

### Packaging (Release Build)
To compile a standalone `.exe` without requiring users to install .NET SDKs, run this command in your terminal at the project root:
```bash
dotnet publish -f net10.0-windows10.0.19041.0 -c Release -p:RuntimeIdentifierOverride=win10-x64
```
The packaged build will be available in the `bin/Release/.../publish/` folder.

---
*Maintained by ShouNLAK. Built with C# & .NET MAUI.*
