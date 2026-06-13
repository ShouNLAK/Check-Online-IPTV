using System;
using System.IO;
using System.Threading.Tasks;
using Check_Online_IPTV.Models;

namespace Check_Online_IPTV.Services
{
    public static class LoggerService
    {
        private static string _logFilePath = "log.csv";
        private static readonly object _lock = new object();

        public static void Initialize(string directory)
        {
            _logFilePath = Path.Combine(directory, "log.csv");
            try
            {
                File.WriteAllText(_logFilePath, "Time,Channel Name,Status,Resolution,Original URL,Fetched URL\n");
            }
            catch { }
        }

        public static void LogChannel(ChannelItem channel)
        {
            try
            {
                string timeStr = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss");
                
                // Tránh trường hợp có dấu phẩy hoặc nháy kép làm hỏng format CSV
                string SafeCsv(string? input)
                {
                    if (string.IsNullOrEmpty(input)) return "";
                    return "\"" + input.Replace("\"", "\"\"") + "\"";
                }

                string line = $"{SafeCsv(timeStr)},{SafeCsv(channel.Name)},{SafeCsv(channel.Status)},{SafeCsv(channel.Resolution)},{SafeCsv(channel.Url)},{SafeCsv(channel.DirectUrl)}\n";

                lock (_lock)
                {
                    File.AppendAllText(_logFilePath, line);
                }
            }
            catch { }
        }
    }
}
