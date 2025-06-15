# Online IPTV Channel Scanner – Version 1.3 (Research Edition)

A refactored, modular release of the C17-standard IPTV scanner—ideal for code exploration, academic research, and rapid customization. Version 1.3 splits the code into focused units, shortens each source file, and makes the logic easier to trace.

---

## Table of Contents

- [Overview](#overview)  
- [What’s New in 1.3](#whats-new-in-13)  
- [Repository Structure](#repository-structure)  
- [Building & Running](#building--running)  
- [Key Modules & Flow](#key-modules--flow)  
- [Contributing & Research Tips](#contributing--research-tips)  
- [License](#license)

---

## Overview

While retaining all the features of the version 1.3, this update reorganizes the codebase into small, single-purpose files:

- **UI layer:** banners, progress bar, logging 
- **Network layer:** speed tests, HTTP checks, redirects  
- **Parser layer:** M3U reading, `#EXTINF` extraction  
- **Scanner layer:** channel testing, duplicate removal  
- **Data layer:** externalized country list and lookup  

This makes each component easy to read, modify, and test.

---

## What’s New in 1.3

- **Modular Codebase**  
  Split into `*.h`/`*.c` (or `.cpp`) files for UI, Network, Sorting, Channel Scan, System, and World data (For country code).  

- **Lean Source Files**  
  Each file is under ~200 lines—ideal for step-through debugging and academic study.  

- **Improved Documentation**  
  Inline comments and README pointers guide researchers through core algorithms.  

_All existing functionality remains unchanged:_  
• Internet speed & geo-lookup  
• HTTP/redirect handling & TS-stream detection  
• Real-time progress bar with ETA  
• Smart duplicate removal by URL + quality  
• Optional VLC launch

---

## Repository Structure

```  
/                 ← root  
├── include/      ← public headers  
│   ├── all.h		← all headers included here
│   ├── ui.h  
│   ├── network.h  
│   ├── system.h  
│   ├── channel_scan.h  
│   ├── sorting.h  
│   └── world.h      ← worldCountries declaration
│ 
├── src/          ← implementation files  
│   ├── main.c  ← main files
│   ├── ui.c  
│   ├── network.c  
│   ├── sorting.c  
│   ├── channel scan.c  
│   ├── system.c  
│   └── world.c      ← worldCountries definition    
├── input.txt      ← sample M3U input  
├── output.m3u     ← default output path  
└── README.md  
```

---

## Building & Running
1. **Install prerequisites:**  
   - Windows 10 + or Linux
   - GCC/Clang: standard C17 compiler 
2. **Clone the repo**  
 
To grab the v1.3 - reseach edition and run it locally:

```bash
# 1. Clone the repository
git clone https://github.com/ShouNLAK/Check-Online-IPTV.git

# 2. Enter the Version 1.2 folder
cd Check-Online-IPTV/Version/"Check Online IPTV (Ver 1.3 - Research edition)"

# 3. Compile the scanner
gcc scanner.c -o IPTVScanner.exe

# 4. Place your M3U playlist in this directory as "input.txt"
#    (Ensure your file follows the correct M3U format)

# 5. Run the scanner
IPTVScanner.exe
```

---

## Key Modules & Flow

1. **main.c**  
   - Internet check + speed/geo  
   - Welcome UI / countdown  
   - Calls `scan_channels()` → writes `output.m3u`  
   - Calls `remove_duplicates()`  
   - Final summary + optional VLC launch  

2. **parser.c/.h**  
   - `count_total_channels()`  
   - `read_channel_entry()`  
   - `extract_channel_name()`  

3. **network.c/.h**  
   - `test_channel_stream()` (HTTP status, 302 redirects, TS detection)  
   - `measure_speed()` (download test + country lookup)  

4. **scanner.c/.h**  
   - `Check_Active()` (drives channel testing and progress bar)  
   - `Remove_Duplicates()` (writes temp file + rename)  

5. **data.c/.h**  
   - `worldCountries[]` (ISO codes → country names)  
   - `Country_Find()` (resolves user’s country code)  

---

## Contributing & Research Tips

- **Browse by module** in `src/` for targeted study.  
- **Add instrumentation** (timers, counters) by modifying single files.  
- **Swap out `curl` calls** for mocks in `network.c` to simulate edge cases.  
- **Enhance duplicate logic** in `scanner.c` without touching core I/O.

Pull requests, issue reports, and academic citations are welcome!

---

## License

Developed by **ShouNLAK**, this code is released as open-source for educational and research purposes.

Feel free to fork, adapt, and cite in your research publications.
