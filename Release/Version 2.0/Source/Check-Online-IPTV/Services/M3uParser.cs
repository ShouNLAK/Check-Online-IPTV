using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;
using Check_Online_IPTV.Models;

namespace Check_Online_IPTV.Services
{
    public static class M3uParser
    {
        public static async Task<List<ChannelItem>> ParseAsync(string filePath)
        {
            var channels = new List<ChannelItem>();
            if (!File.Exists(filePath)) return channels;

            string[] lines = await File.ReadAllLinesAsync(filePath);
            string currentExtInf = "";
            string currentName = "";
            int channelCounter = 1;

            foreach (var line in lines)
            {
                string tLine = line.Trim();
                if (string.IsNullOrEmpty(tLine)) continue;

                if (tLine.StartsWith("#EXTINF"))
                {
                    currentExtInf = tLine;
                    var parts = tLine.Split(',');
                    if (parts.Length > 1)
                    {
                        currentName = parts[1].Trim();
                    }
                    else
                    {
                        currentName = $"Channel {channelCounter}";
                    }
                }
                else if (tLine.StartsWith("http://") || tLine.StartsWith("https://"))
                {
                    if (string.IsNullOrEmpty(currentExtInf))
                    {
                        currentName = $"Channel {channelCounter}";
                        currentExtInf = $"#EXTINF:-1,{currentName}";
                    }

                    channels.Add(new ChannelItem
                    {
                        Name = currentName,
                        Url = tLine,
                        ExtInf = currentExtInf,
                        Status = "Pending"
                    });

                    currentExtInf = "";
                    currentName = "";
                    channelCounter++;
                }
            }

            return channels;
        }

        // Lấy diện tích độ phân giải để so sánh
        private static int GetResolutionArea(string? res)
        {
            if (string.IsNullOrEmpty(res)) return 0;
            var parts = res.Split('x');
            if (parts.Length == 2 && int.TryParse(parts[0], out int w) && int.TryParse(parts[1], out int h))
            {
                return w * h;
            }
            return 0;
        }

        public static async Task SaveAsync(string filePath, IEnumerable<ChannelItem> channels)
        {
            // 1. Chỉ lấy những kênh Live
            var liveChannels = channels.Where(c => c.IsLive).ToList();

            // 2. Gỡ bỏ trùng lặp (Dựa vào URL cuối cùng) - Giữ kênh có độ phân giải lớn nhất
            var uniqueChannels = liveChannels
                .GroupBy(c => string.IsNullOrEmpty(c.DirectUrl) ? c.Url : c.DirectUrl)
                .Select(g => g.OrderByDescending(c => GetResolutionArea(c.Resolution)).First())
                .ToList();

            // 3. Sắp xếp (Theo tên kênh, nếu trùng thì theo độ phân giải)
            uniqueChannels = uniqueChannels
                .OrderBy(c => c.Name)
                .ThenByDescending(c => GetResolutionArea(c.Resolution))
                .ToList();

            // 4. Ghi file
            using StreamWriter writer = new StreamWriter(filePath);
            await writer.WriteLineAsync("#EXTM3U");
            foreach (var channel in uniqueChannels)
            {
                string extInf = channel.ExtInf;
                if (!string.IsNullOrEmpty(channel.Resolution) && !extInf.Contains(channel.Resolution))
                {
                    extInf += $" ({channel.Resolution})";
                }
                await writer.WriteLineAsync(extInf);
                await writer.WriteLineAsync(string.IsNullOrEmpty(channel.DirectUrl) ? channel.Url : channel.DirectUrl);
            }
        }
    }
}
