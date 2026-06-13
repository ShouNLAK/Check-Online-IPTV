using System;
using System.Diagnostics;
using System.Net.Http;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;
using Check_Online_IPTV.Models;

namespace Check_Online_IPTV.Services
{
    public class StreamChecker
    {
        private readonly HttpClient _httpClient;
        private const string UserAgent = "VLC/3.0.16 LibVLC/3.0.16";

        public StreamChecker()
        {
            var handler = new HttpClientHandler
            {
                AllowAutoRedirect = true,
                MaxAutomaticRedirections = 5
            };
            _httpClient = new HttpClient(handler);
            _httpClient.DefaultRequestHeaders.Add("User-Agent", UserAgent);
            _httpClient.Timeout = TimeSpan.FromSeconds(15);
        }

        private bool IsSafeUrl(string url)
        {
            if (string.IsNullOrWhiteSpace(url)) return false;

            // Sử dụng thư viện Uri chuẩn quốc tế để phân tích, tránh mọi thủ thuật bypass bằng String Encoding
            if (!Uri.TryCreate(url, UriKind.Absolute, out var uri))
            {
                return false;
            }

            // 1. Chỉ cho phép giao thức mạng an toàn
            if (uri.Scheme != Uri.UriSchemeHttp && uri.Scheme != Uri.UriSchemeHttps)
            {
                return false;
            }

            // 2. Chống tấn công SSRF (Server-Side Request Forgery) - Chặn quét localhost/loopback
            if (uri.IsLoopback)
            {
                return false;
            }

            // 3. Chặn các cổng (Port) dịch vụ nội bộ nhạy cảm để chống Port-Scanning
            // Ví dụ: 21 (FTP), 22 (SSH), 23 (Telnet), 3389 (RDP), 3306 (MySQL)...
            int[] blacklistedPorts = { 21, 22, 23, 25, 53, 110, 135, 139, 445, 1433, 3306, 3389 };
            if (Array.Exists(blacklistedPorts, port => port == uri.Port))
            {
                return false;
            }

            // 4. Chặn các đuôi file có khả năng thực thi mã độc
            // AbsolutePath loại bỏ hoàn toàn các Query Parameters (?token=...) để lấy chính xác đuôi file
            string extension = System.IO.Path.GetExtension(uri.AbsolutePath).ToLowerInvariant();

            string[] dangerousExtensions = { 
                ".exe", ".bat", ".cmd", ".msi", ".sh", ".vbs", 
                ".apk", ".bin", ".scr", ".js", ".ps1", ".jar", 
                ".dll", ".sys", ".elf", ".cpl", ".wsf" 
            };

            foreach (var ext in dangerousExtensions)
            {
                if (extension == ext)
                {
                    return false;
                }
            }

            return true;
        }

        public async Task CheckStreamAsync(ChannelItem channel, CancellationToken cancellationToken)
        {
            channel.Status = "Checking...";
            
            // Bộ lọc an mật - Báo cáo link lạ/virus ngay từ đầu
            if (!IsSafeUrl(channel.Url))
            {
                channel.Status = "Suspicious Link";
                channel.IsLive = false;
                return;
            }

            int maxRetries = 3;
            bool success = false;

            for (int i = 0; i < maxRetries; i++)
            {
                if (cancellationToken.IsCancellationRequested) break;
                
                try
                {
                    var sw = Stopwatch.StartNew();
                    using var request = new HttpRequestMessage(HttpMethod.Get, channel.Url);
                    using var response = await _httpClient.SendAsync(request, HttpCompletionOption.ResponseHeadersRead, cancellationToken);
                    sw.Stop();

                    if (i == 0 || channel.ConnectionTimeMs == 0)
                    {
                        channel.ConnectionTimeMs = sw.ElapsedMilliseconds;
                    }
                    channel.ResponseCode = (int)response.StatusCode;

                    if (!response.IsSuccessStatusCode)
                    {
                        channel.Status = $"Error {(int)response.StatusCode}";
                        channel.IsLive = false;
                        await Task.Delay(1000, cancellationToken);
                        continue;
                    }

                    string finalUrl = response.RequestMessage?.RequestUri?.ToString() ?? channel.Url;
                    channel.DirectUrl = finalUrl;

                    bool isLive = await AnalyzeWithFfprobeAsync(channel, finalUrl, cancellationToken);
                    if (isLive)
                    {
                        success = true;
                        break;
                    }
                    else
                    {
                        await Task.Delay(1000, cancellationToken);
                    }
                }
                catch (TaskCanceledException)
                {
                    channel.Status = "Timeout/Canceled";
                    channel.IsLive = false;
                    break;
                }
                catch (Exception ex)
                {
                    channel.Status = "Error: " + ex.Message;
                    channel.IsLive = false;
                    await Task.Delay(1000, cancellationToken);
                }
            }

            if (!success)
            {
                channel.IsLive = false;
            }
        }

        private async Task<bool> AnalyzeWithFfprobeAsync(ChannelItem channel, string url, CancellationToken cancellationToken)
        {
            try
            {
                var processInfo = new ProcessStartInfo
                {
                    FileName = "ffprobe",
                    // Tối ưu args: giảm thời gian nạp buffer (probesize) để stream nhanh trả kết quả thay vì timeout
                    Arguments = $"-v warning -print_format json -show_streams -headers \"User-Agent: {UserAgent}\" -probesize 5000000 -analyzeduration 5000000 \"{url}\"",
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    CreateNoWindow = true
                };

                using var process = new Process { StartInfo = processInfo };
                process.Start();

                var outputTask = process.StandardOutput.ReadToEndAsync(cancellationToken);
                var errorTask = process.StandardError.ReadToEndAsync(cancellationToken);

                // Timeout cho ffprobe là 20s
                var timeoutTask = Task.Delay(20000, cancellationToken);
                
                var completedTask = await Task.WhenAny(Task.WhenAll(outputTask, errorTask), timeoutTask);

                if (completedTask == timeoutTask)
                {
                    process.Kill();
                    channel.Status = "FFprobe Timeout";
                    return false;
                }

                string output = await outputTask;
                string error = await errorTask;
                
                if (string.IsNullOrWhiteSpace(output))
                {
                    channel.Status = "No stream data";
                    return false;
                }

                using var document = JsonDocument.Parse(output);
                var root = document.RootElement;

                if (root.TryGetProperty("streams", out var streams))
                {
                    bool hasVideo = false;
                    bool hasAudio = false;

                    foreach (var stream in streams.EnumerateArray())
                    {
                        if (stream.TryGetProperty("codec_type", out var codecType))
                        {
                            if (codecType.GetString() == "video")
                            {
                                hasVideo = true;
                                channel.VideoCodec = stream.GetProperty("codec_name").GetString() ?? "";
                                if (stream.TryGetProperty("width", out var width) && stream.TryGetProperty("height", out var height))
                                {
                                    channel.Resolution = $"{width.GetInt32()}x{height.GetInt32()}";
                                }
                            }
                            else if (codecType.GetString() == "audio")
                            {
                                hasAudio = true;
                                channel.AudioCodec = stream.GetProperty("codec_name").GetString() ?? "";
                            }
                        }
                    }

                    if (hasVideo || hasAudio)
                    {
                        channel.Status = "Live";
                        channel.IsLive = true;
                        return true;
                    }
                    else
                    {
                        channel.Status = "No AV stream";
                        return false;
                    }
                }
                else
                {
                    channel.Status = "Invalid media format";
                    return false;
                }
            }
            catch (Exception ex)
            {
                channel.Status = "FFprobe error";
                return false;
            }
        }
    }
}
