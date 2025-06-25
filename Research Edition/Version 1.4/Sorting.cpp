#include "Sorting.h"
#include "Header.h"
#include "System.h"
#include "Channel_Scan.h"

// Sort the status list based on channel names and resolution
void sortStatusList(ChannelStatus* status_list, int processed) {
    for (int i = 0; i < processed - 1; ++i) {
        for (int j = i + 1; j < processed; ++j) {
            // Compare channel names in a case-insensitive manner.
            int cmp = strcasecmp(status_list[i].name, status_list[j].name);
            // If names differ, sort alphabetically.
            // If the names match (cmp == 0), compare resolution by its area (low to high).
            if (cmp > 0 || (cmp == 0 &&
                getResolutionArea(status_list[i].resolution) > getResolutionArea(status_list[j].resolution)))
            {
                // Swap if out of order.
                ChannelStatus temp = status_list[i];
                status_list[i] = status_list[j];
                status_list[j] = temp;
            }
        }
    }
}

// Comparison function for qsort that sorts resolutions by their pixel area (width * height) in descending order.
int compare_resolutions(const void* p, const void* q)
{
    // p and q point to each resolution string
    const char* res_a = (const char*)p;
    const char* res_b = (const char*)q;

    int w1 = 0, h1 = 0, w2 = 0, h2 = 0;
    // Parse resolution strings (format "WIDTHxHEIGHT")
    sscanf(res_a, "%dx%d", &w1, &h1);
    sscanf(res_b, "%dx%d", &w2, &h2);

    int area1 = w1 * h1;
    int area2 = w2 * h2;
    return area2 - area1;  // sort from high to low area
}

int cmp_streams(const void* a, const void* b)
{
    const StreamInfo* s1 = (const StreamInfo*)a;
    const StreamInfo* s2 = (const StreamInfo*)b;
    return s2->area - s1->area;
}

// Helper function: Converts a resolution string ("WxH") into a standard label.
void convertResolutionToLabel(const char* resolution, char* label, size_t label_size) {
    int width = 0, height = 0;

    // Attempt to parse the input string as "WIDTHxHEIGHT"
    if (sscanf(resolution, "%dx%d", &width, &height) != 2) {
        // On parse failure, simply return the input string.
        snprintf(label, label_size, "%s", resolution);
        return;
    }

    // 480-line resolution cases:
    if (height == 480) {
        if (width == 720) {
            snprintf(label, label_size, "480p");
            return;
        }
        else if (width == 352 || width == 480 || width == 528 ||
            width == 544 || width == 640 || width == 704) {
            snprintf(label, label_size, "480i");
            return;
        }
    }

    // 576-line resolution cases:
    if (height == 576) {
        if (width == 720) {
            snprintf(label, label_size, "576p");
            return;
        }
        else if (width == 480 || width == 544 || width == 704) {
            snprintf(label, label_size, "576i");
            return;
        }
    }

    // 720-line resolution cases:
    if (height == 720) {
        if (width == 1280) {
            snprintf(label, label_size, "720p");
            return;
        }
    }

    // 1080-line resolution cases:
    if (height == 1080) {
        if (width == 1920) {
            snprintf(label, label_size, "1080p");
            return;
        }
        else if (width == 1280 || width == 1440) {
            snprintf(label, label_size, "1080i");
            return;
        }
    }

    // 1440-line resolution cases:
    if (height == 1440) {
        if (width == 2560) {
            snprintf(label, label_size, "1440p - 2K");
            return;
        }
    }

    // UHD resolutions:
    if (width == 3840 && height == 2160) {
        snprintf(label, label_size, "2160p - 4K");
        return;
    }
    if (width == 7680 && height == 4320) {
        snprintf(label, label_size, "4320p - 8K");
        return;
    }

    // Fallback: if no condition matches, do not convert.
    snprintf(label, label_size, "%s", resolution);
}