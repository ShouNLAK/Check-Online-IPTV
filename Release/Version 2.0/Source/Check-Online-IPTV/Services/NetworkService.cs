using System;
using System.Diagnostics;
using System.Net.Http;
using System.Threading;
using System.Threading.Tasks;

namespace Check_Online_IPTV.Services
{
    public class NetworkInfo
    {
        public bool IsConnected { get; set; }
        public string CountryCode { get; set; } = "Unknown";
        public double DownloadSpeedMbps { get; set; } = 0;
    }

    public static class NetworkService
    {
        private static readonly HttpClient _httpClient = new HttpClient { Timeout = TimeSpan.FromSeconds(15) };

        public static async Task<NetworkInfo> RunDiagnosticsAsync()
        {
            var info = new NetworkInfo();
            info.IsConnected = await CheckInternetAsync();
            if (info.IsConnected)
            {
                info.CountryCode = await GetCountryCodeAsync();
                info.DownloadSpeedMbps = await MeasureSpeedAsync();
            }
            return info;
        }

        private static async Task<bool> CheckInternetAsync()
        {
            try
            {
                using var request = new HttpRequestMessage(HttpMethod.Head, "http://clients3.google.com/generate_204");
                using var response = await _httpClient.SendAsync(request);
                return response.IsSuccessStatusCode;
            }
            catch
            {
                return false;
            }
        }

        private static async Task<string> GetCountryCodeAsync()
        {
            try
            {
                return (await _httpClient.GetStringAsync("https://ipinfo.io/country")).Trim();
            }
            catch
            {
                return "Unknown";
            }
        }

        private static async Task<double> MeasureSpeedAsync()
        {
            try
            {
                // Download a 1MB test file to measure speed
                var stopwatch = Stopwatch.StartNew();
                // Cloudflare test block for stable speed check
                var request = new HttpRequestMessage(HttpMethod.Get, "https://speed.cloudflare.com/__down?bytes=1048576");
                using var response = await _httpClient.SendAsync(request, HttpCompletionOption.ResponseContentRead);
                stopwatch.Stop();

                double elapsedSeconds = stopwatch.Elapsed.TotalSeconds;
                if (elapsedSeconds > 0 && response.IsSuccessStatusCode)
                {
                    // 1 MB = 8 Megabits
                    return 8.0 / elapsedSeconds;
                }
            }
            catch
            {
                // Ignore error if speed test fails
            }
            return 0;
        }
    }
}
