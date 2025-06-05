# Check Online for IPTV
Scanning and sorting IPTV List that available on your region (Windows-user only)

# Table of content
- [WARNING BEFORE USE](#WARNING_BEFORE_USE)
- [How does it work?](#How_does_it_work?)
- [Question](#Question)


## WARNING BEFORE USE
This code is build by student, so the effiency is not good as other but the result is acceptable for user to enjoying IPTV

This code also only used by Windows-user (Due to Command Prompt is being used)

Input file will take Channel Name (after last comma) and Resolution or Quality ( Last "(" , ")" ) if available

Output will only contain Channel Name, URL, (Resolution - Quality if available)

## How does it work?
1. Check user if they have internet accessibility
2. Open file
3. Scanning how many channel are there in input.txt
4. Begin the search with the URL
5. Running CMD with curl command, to check if the URL respond back in 15 seconds. If yes, the URL is ACTIVE, if not then its will be considered INACTIVE / TIMEOUT.
6. Save the infomation of each channel for showing all channel scan summary for user to know
7. Write all ACTIVE channel to output.m3u

--- OPTIONAL ---
   1. Asking user to open output file by VLC after finished the scanning process
   2. Calculate time for whole process
   3. Show processbar while processing
   4. User can change the 15s of responding time max to 30s

### Question
Q: What is the conditions for channel for active or not active?

A: Channel will active when the respond code is 2XX or 3XX, or requesting for an download.

Q: Many things in the code is very unnecessary?

A: Yes, many of them is, just making the UI or more information for user.

Q: Where can i get the IPTV List?

A: If you're new, " https://github.com/iptv-org/iptv " is recommended for IPTV
