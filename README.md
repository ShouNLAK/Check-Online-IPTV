# Online IPTV Channel Scanner – Version 1.4

A refactored, modular release of the C17-standard IPTV scanner—ideal for code exploration, academic research, and rapid customization. **Starting from Version 1.4, the Normal and Research Edition codebases are merged into a single, unified project.** This final version combines the best of both approaches, making the code easier to understand, maintain, and improve in the future.

---

## Table of Contents

- [Overview](#overview)  
- [What's New in 1.4](#whats-new-in-14)  
- [Improvements Table: 1.3 vs 1.4](#improvements-table-13-vs-14)  
- [Repository Structure](#repository-structure)  
- [Building & Running](#building--running)  
- [Key Modules & Flow](#key-modules--flow)  
- [Contributing & Research Tips](#contributing--research-tips)  
- [License](#license)

---

## Overview

Version 1.4 merges the Normal and Research Edition structures, providing a single, well-organized codebase. The project is now easier to read, modify, and extend, with all features and improvements from both editions included:

- **UI layer:** banners, progress bar, logging  
- **Network layer:** speed tests, HTTP checks, redirects  
- **Parser layer:** M3U reading, `#EXTINF` extraction  
- **Scanner layer:** channel testing, duplicate removal  
- **Data layer:** externalized country list and lookup  
- **Multi-threaded scanning:** supports scanning multiple channels in parallel for faster results

---

## What's New in 1.4

- **Unified Codebase**  
  The Normal and Research Edition are now merged, making it easier for users to understand and maintain the project.

- **Multi-threaded Scanning**  
  Now supports scanning multiple channels at the same time, significantly improving performance on modern CPUs.

- **Continued Research Edition Structure**  
  The modular, research-friendly layout is preserved for easier code reading and extension.

- **Further Modularization**  
  Additional separation of concerns and clearer interfaces between modules.

- **Enhanced Documentation**  
  More inline comments and improved README sections for even easier onboarding.

- **Better Maintainability**  
  Refined file structure and naming for faster navigation and updates.

_All existing functionality remains unchanged:_  
• Internet speed & geo-lookup  
• HTTP/redirect handling & TS-stream detection  
• Real-time progress bar with ETA  
• Smart duplicate removal by URL + quality  
• Optional VLC launch

---

## Improvements Table: 1.3 vs 1.4

| Feature/Aspect                | Version 1.3 (Research Edition)         | Version 1.4 (Unified Final)                |
|-------------------------------|----------------------------------------|--------------------------------------------|
| Codebase Structure            | Modular, research-focused              | Unified: Normal + Research Edition merged  |
| Multi-threaded Scanning       | No                                     | Yes (scan multiple channels in parallel)   |
| Documentation                 | Improved, research-oriented            | Further enhanced, easier onboarding        |
| Maintainability               | Good                                   | Even better, clearer file/module layout    |
| UI/UX                         | ASCII banners, progress bar            | Same, with more clarity and summary        |
| Duplicate Removal             | By URL + quality                       | Same                                       |
| Speed & Geo Lookup            | Yes                                    | Yes                                        |
| Platform Support              | Windows/Linux, C17                     | Same                                       |
| Customization                 | Easy                                   | Even easier, more modular                  |
| Final Output                  | output.m3u                             | output.m3u | log.txt (For Live) | log.csv  |

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

# 2. Enter the Version 1.4 folder
cd Check-Online-IPTV/Version/"Check Online IPTV (Ver 1.4 - Research edition)"

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

## Final Output

After running the scanner, you will get:

- **output.m3u**: The cleaned, active IPTV playlist in standard M3U format.
- **log.txt**: A detailed log file containing the scan results, including channel names, statuses (Live/Inactive/Redirected), resolutions, and URLs checked.

You can use `output.m3u` directly in VLC or any compatible media player.  
Check `log.txt` for troubleshooting, auditing, or research purposes.

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
