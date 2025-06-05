# Check Online for IPTV (Using Standard C)

Scanning and sorting the IPTV list available in your region (Windows users only)

# Table of Contents
- [WARNING BEFORE USE](#warning-before-use)
- [How Does It Work?](#how-does-it-work)
- [Questions](#questions)

---

## WARNING BEFORE USE

This code is built by a student, so its efficiency is not as good as other tools, but the results are acceptable for users to enjoy IPTV.

This code is intended for Windows users only (because it uses the Command Prompt).

The input file should include the Channel Name (after the last comma) and the Resolution or Quality (between the last "(" and ")" if available).

The output will contain only the Channel Name, URL, and, if available, the Resolution or Quality.

---

## How Does It Work?

1. **Check Internet Access:** Verify that the user has an active internet connection.
2. **Open the File:** Open the input file.
3. **Count Channels:** Determine how many channels are listed in `input.txt`.
4. **URL Search:** Begin checking each URL.
5. **Curl Check:** Run CMD with the `curl` command to check if the URL responds within 15 seconds. If it does, the URL is marked as **ACTIVE**; if not, it is considered **INACTIVE/TIMEOUT**.
6. **Save Information:** Record the information for each channel to display a scan summary.
7. **Write Output:** Write all **ACTIVE** channels to `output.m3u`.

---

### OPTIONAL

1. Prompt the user to open the output file in VLC once the scanning process is finished.
2. Calculate the total duration of the process.
3. Display a progress bar during processing.
4. Allow the user to change the response timeout from 15 seconds to a maximum of 30 seconds.

---

## Questions

**Q: What are the conditions for a channel to be considered active or inactive?**  
**A:** A channel is considered active if the response code is in the 2XX or 3XX range, or if it prompts for a download.

**Q: Is much of the code unnecessary?**  
**A:** Yes, a lot of it is superfluous; it primarily exists to enhance the user interface and provide additional information.

**Q: Where can I get the IPTV list?**  
**A:** If you're new, the IPTV list available at [https://github.com/iptv-org/iptv](https://github.com/iptv-org/iptv) is recommended.

---

## Test Run
**Input is Japanese IPTV (6/6/2025) from iptv-org**

*UI Processing*
![image](https://github.com/user-attachments/assets/03086949-8bea-478f-b72e-6d4a098de755)

*Scan summary result + Time processing*
![image](https://github.com/user-attachments/assets/1819ffc1-55bb-4fa0-a2b0-338ea5ac90f5)

*(Optional) Open VLC*
![image](https://github.com/user-attachments/assets/a35145b8-f9f0-4370-b5b0-27f07ed29a71)

