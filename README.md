<HTML><HEAD></HEAD><BODY><!--StartFragment --><h1>Online IPTV Channel Scanner – Version 1.3</h1>
<p><strong>Release Date:</strong> June 15, 2025<br />
<strong>Project Type:</strong> Student self-made project</p>
<p>Experience the fastest, most accurate IPTV channel scanner yet—with a polished UTF-8 UI, blazing scan speeds, and two editions tailored for end-users and researchers alike.</p>
<hr />
<h2>Table of Contents</h2>
<ul>
<li><a href="#introduction">Introduction</a></li>
<li><a href="#what’s-new-in-1.3">What’s New in 1.3</a></li>
<li><a href="#editions-overview">Editions Overview</a></li>
<li><a href="#version-comparison-12-vs-13">Version Comparison: 1.2 vs 1.3</a></li>
<li><a href="#optional-features--enhancements">Optional Features &amp; Enhancements</a></li>
<li><a href="#upgrade-instructions">Upgrade Instructions</a></li>
<li><a href="#research-edition-file-structure">Research Edition File Structure</a></li>
<li><a href="#license--credits">License &amp; Credits</a></li>
</ul>
<hr />
<h2>Introduction</h2>
<p>Version 1.3 arrives today with major performance boosts—channel checks complete in just 2–3 seconds, accuracy climbs to 98.5% (99% in Research Edition), and full UTF-8 support makes non-English channel names crystal clear. Choose between a one-file executable or dive into a modular, instrumented codebase for academic study.</p>
<hr />
<h2>What’s New in 1.3</h2>
<ul>
<li><p><strong>Blazing Scan Speed &amp; Stellar Accuracy</strong><br />
• Redirects resolved directly in 2–3 s (vs. up to 22 s).<br />
• Normal Edition: ~98.5% success rate.<br />
• Research Edition: ~99% success (~1% of unsuccess because of the hosts URL respond with empty content).</p>
</li>
<li><p><strong>Full UTF-8 Display &amp; Alignment</strong><br />
• Perfect rendering of any script or special character.<br />
• Fixed-width, neatly aligned columns in all summaries.</p>
</li>
<li><p><strong>Enhanced Redirect &amp; Resolution Handling</strong><br />
• More robust auto-redirects to fetch the absolute best stream.<br />
• Real-time resolution detection with standardized labels (“480p”, “1080p”, “4K”, etc.).</p>
</li>
<li><p><strong>Vibrant, User-Friendly Interface</strong><br />
• Bright colored banners, dynamic progress bars, live countdown timers.<br />
• Timestamped, color-coded status and error messages.</p>
</li>
<li><p><strong>Built-In Logging (Research Edition)</strong><br />
• Automatic generation of <code>scan.log</code> detailing per-URL timings, HTTP codes, retries, and failures.<br />
• Ideal for performance audits, instrumentation, and teaching.</p>
</li>
</ul>
<hr />
<h2>Editions Overview</h2>
<p>Both editions released today—pick the one that fits your workflow:</p>

Feature | Normal Edition | Research Edition
-- | -- | --
One-file executable | ✔️ | ✔️
UTF-8 UI & banners | ✔️ | ✔️
Direct redirect resolution | ✔️ | ✔️
Built-in log file | ❌ | 📄 scan.log
Modular source breakdown | ❌ | ✔️ (7 layers, < 200 lines each)
Academic annotations & guides | ❌ | 📝 Extensive comments & README
Instrumentation hooks (timers) | ❌ | 🔌 Ready-to-use timing & mock APIs
Success rate target | ~98.5% | ~99%


<hr />
<h2>Optional Features &amp; Enhancements</h2>
<ul>
<li><strong>Custom Timeouts:</strong> Adjust per-URL timeout (up to 30 s).</li>
<li><strong>VLC Auto-Launch:</strong> Prompt to open <code>output.m3u</code> in VLC post-scan.</li>
<li><strong>Detailed Metrics:</strong> Breakdown of active/inactive, retries, host-empty failures.</li>
<li><strong>Theming:</strong> Light/dark schemes and customizable banners.</li>
<li><strong>Academic Hooks (Research):</strong> Built-in mocks and timers for experiments and lectures.</li>
</ul>
<hr />
<h2>Upgrade Instructions</h2>
<ol>
<li>Visit the <a href="https://github.com/iptv-org/iptv/releases/tag/v1.3">GitHub Releases page</a>.</li>
<li>Download the <strong>Normal Edition (Using .exe file after this README or found in Version folder) </strong> or <strong>Research Edition (Check.IPTV.Online.Ver.1.3.-.Research.Edition.zip)</strong></li>
<li>Replace your existing executable or source folder with the new files.</li>
<li>Run <code>Check.Online.IPTV.exe</code> (or compile the research sources) to enjoy the speed and accuracy improvements.</li>
<li>Research Edition users: inspect <code>scan.log</code> and the modular code to explore instrumentation hooks.</li>
</ol>
<hr />
<h2>Research Edition File Structure</h2>
<pre><code class="language-text">Research Edition File Structure
├── References/                  
├── External Dependencies/       
├── Header Files/               # Public APIs &amp; shared data structures
│   ├── All.h                   # Central header including them all
│   ├── Channel_Scan.h          # Scanning algorithms &amp; retry logic
│   ├── Header.h                # M3U tag parsing &amp; metadata extraction
│   ├── Log.h                   # log() declarations, levels &amp; formats
│   ├── Network.h               # HTTP requests, redirects &amp; geo-detect
│   ├── Sorting.h               # Duplicate detection &amp; quality compare
│   ├── System.h                # Sleep, timers, speed test harness
│   ├── UI.h                    # Progress bar, banners &amp; UTF-8 formatting
│   └── World.h                 # Global state: For Country Code
├── Resource Files/             # Icons, color schemes, templates
├── Source Files/               # Core implementations (&lt; 200 lines each)
│   ├── Main.cpp                # Entry point, edition selector, orchestrator
│   ├── Channel_Scan.cpp        # Implements Channel_Scan.h
│   ├── Network.cpp             # Implements Network.h
│   ├── Sorting.cpp             # Implements Sorting.h
│   ├── System.cpp              # Implements System.h
│   ├── UI.cpp                  # Implements UI.h
│   ├── World.cpp               # Implements World.h
│   └── log.cpp                 # Implements Log.h
└── Other Files/                
    ├── input.txt               # Sample list for testing
    ├── scan.log                # Auto-generated full log output
    ├── output.m3u              # Final playlist
    └── README.md               # This document
</code></pre>
<h3>File Responsibilities</h3>
<ul>
<li><strong>Main.cpp</strong>: Bootstraps Normal vs. Research edition, initializes globals, parses flags, and launches the scan loop.</li>
<li><strong>Channel_Scan.{h,cpp}</strong>: URL health checks, retries, auto-redirect, and quality detection.</li>
<li><strong>Network.{h,cpp}</strong>: <code>curl</code> wrappers for HTTP HEAD/GET, geo-lookup, and raw resolution fetch.</li>
<li><strong>Sorting.{h,cpp}</strong>: Duplicate URL hashing, best-quality retention, and resolution-to-label conversion.</li>
<li><strong>System.{h,cpp}</strong>: Download-speed test harness, cross-platform sleep, and timer utilities.</li>
<li><strong>UI.{h,cpp}</strong>: UTF-8 console renderer (banners, spinners, tables) and color-coded logs.</li>
<li><strong>World.{h,cpp}</strong>: Houses all counters, configuration settings, and central ETA calculations.</li>
<li><strong>Log.{h,cpp}</strong>: Writes <code>scan.log</code> with per-URL timings, HTTP codes, retries, and adjustable verbosity.</li>
</ul>
<hr />
## Test Demo

**Input:** Vietnamese IPTV list from IPTV-org  
**Workflow Preview:**

1. Internet measurement + Country detection + Scanning process  
   ![Internet measurement, country detection, and scanning process](https://github.com/user-attachments/assets/db99b502-15bf-41d3-b7a3-41050a834220)

2. Scanning summary + Duplicate removal + VLC launch prompt (optional)  
   ![Scan summary with duplicate removal](https://github.com/user-attachments/assets/9f8b971d-c228-452c-a288-fb6e9075ceea)  
   ![Asking for VLC launch](https://github.com/user-attachments/assets/ef66dd9a-3cc8-4b64-a16d-fdcb50924f29)
   ![VLC launch prompt](https://github.com/user-attachments/assets/fc29f37b-9980-43df-81c4-0c139020a2cf)

3. `scan.log` output (Research Edition)  
   ![Log file example](https://github.com/user-attachments/assets/3898d5db-963f-4b64-8294-9db27adfdfbb)

<h2>License &amp; Credits</h2>
<p><strong>Developed by ShouNLAK</strong><br />
Open-source project · Educational &amp; personal use only.<br />
Contributions, bug reports, and pull requests if you have any question or discussing about the project</a>.</p>
<!--EndFragment -->
