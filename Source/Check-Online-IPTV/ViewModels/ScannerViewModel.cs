using System;
using System.Diagnostics;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Input;
using Check_Online_IPTV.Models;
using Check_Online_IPTV.Services;

namespace Check_Online_IPTV.ViewModels
{
    public class ScannerViewModel : INotifyPropertyChanged
    {
        private ObservableCollection<ChannelItem> _channels = new();
        public ObservableCollection<ChannelItem> Channels
        {
            get => _channels;
            set { _channels = value; OnPropertyChanged(); }
        }

        private bool _isScanning;
        public bool IsScanning
        {
            get => _isScanning;
            set { _isScanning = value; OnPropertyChanged(); }
        }

        private int _totalChannels;
        public int TotalChannels
        {
            get => _totalChannels;
            set { _totalChannels = value; OnPropertyChanged(); }
        }

        private int _scannedCount;
        public int ScannedCount
        {
            get => _scannedCount;
            set
            {
                _scannedCount = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(ProgressFraction));
            }
        }

        private int _liveCount;
        public int LiveCount
        {
            get => _liveCount;
            set { _liveCount = value; OnPropertyChanged(); }
        }

        public double ProgressFraction => TotalChannels == 0 ? 0 : (double)ScannedCount / TotalChannels;

        private string _networkStatus = "Checking...";
        public string NetworkStatus
        {
            get => _networkStatus;
            set { _networkStatus = value; OnPropertyChanged(); }
        }

        private string _selectedFilePath = "No file selected";
        public string SelectedFilePath
        {
            get => _selectedFilePath;
            set { _selectedFilePath = value; OnPropertyChanged(); }
        }

        public ObservableCollection<string> LiveLogs { get; } = new();

        public ICommand LoadFileCommand { get; }
        public ICommand StartScanCommand { get; }
        public ICommand StopScanCommand { get; }

        private CancellationTokenSource? _cancellationTokenSource;
        private string _currentFilePath = string.Empty;

        public ScannerViewModel()
        {
            LoadFileCommand = new Command(async () => await LoadFileAsync());
            StartScanCommand = new Command(async () => await StartScanAsync(), () => !IsScanning && Channels.Count > 0);
            StopScanCommand = new Command(StopScan, () => IsScanning);
        }

        private async Task LoadFileAsync()
        {
            try
            {
                var customFileType = new FilePickerFileType(new Dictionary<DevicePlatform, IEnumerable<string>>
                {
                    { DevicePlatform.WinUI, new[] { ".m3u", ".m3u8", ".txt" } },
                    { DevicePlatform.macOS, new[] { "m3u", "m3u8", "txt" } },
                });

                var result = await FilePicker.Default.PickAsync(new PickOptions
                {
                    PickerTitle = "Select IPTV Playlist or TXT",
                    FileTypes = customFileType,
                });

                if (result != null)
                {
                    _currentFilePath = result.FullPath;
                    SelectedFilePath = _currentFilePath;
                    var loadedChannels = await M3uParser.ParseAsync(_currentFilePath);
                    Channels = new ObservableCollection<ChannelItem>(loadedChannels);
                    TotalChannels = Channels.Count;
                    ScannedCount = 0;
                    LiveCount = 0;
                    ((Command)StartScanCommand).ChangeCanExecute();
                }
            }
            catch (Exception ex)
            {
                await Application.Current.MainPage.DisplayAlert("Error", ex.Message, "OK");
            }
        }

        private async Task StartScanAsync()
        {
            IsScanning = true;
            ((Command)StartScanCommand).ChangeCanExecute();
            ((Command)StopScanCommand).ChangeCanExecute();

            ScannedCount = 0;
            LiveCount = 0;
            LiveLogs.Clear();
            AddLog("Scan started...");

            foreach (var channel in Channels)
            {
                channel.ResetState();
            }

            // Kiểm tra mạng trước khi quét
            NetworkStatus = "Checking Network...";
            var netInfo = await NetworkService.RunDiagnosticsAsync();
            if (netInfo.IsConnected)
            {
                NetworkStatus = $"Online | {netInfo.CountryCode} | {netInfo.DownloadSpeedMbps:F1} Mbps";
            }
            else
            {
                NetworkStatus = "Offline - Please check your internet connection.";
                var retry = await Application.Current.MainPage.DisplayAlert("Network Error", "No Internet connection. Please check your network.", "Retry", "Cancel");
                if (!retry)
                {
                    IsScanning = false;
                    ((Command)StartScanCommand).ChangeCanExecute();
                    ((Command)StopScanCommand).ChangeCanExecute();
                    return;
                }
            }

            // Khởi tạo LoggerService ghi ra cùng thư mục
            if (!string.IsNullOrEmpty(_currentFilePath))
            {
                string dir = Path.GetDirectoryName(_currentFilePath) ?? "";
                LoggerService.Initialize(dir);
            }

            _cancellationTokenSource = new CancellationTokenSource();
            var token = _cancellationTokenSource.Token;

            var checker = new StreamChecker();
            
            // Giới hạn số luồng đồng thời (Concurrency) để không làm nghẽn máy
            int maxConcurrency = 20; 
            using var semaphore = new SemaphoreSlim(maxConcurrency);

            var tasks = Channels.Select(async channel =>
            {
                await semaphore.WaitAsync(token);
                try
                {
                    await checker.CheckStreamAsync(channel, token);
                    if (channel.IsLive)
                    {
                        Interlocked.Increment(ref _liveCount);
                        OnPropertyChanged(nameof(LiveCount));
                    }
                    LoggerService.LogChannel(channel);

                    // Add to Live Log safely
                    string time = DateTime.Now.ToString("HH:mm:ss");
                    string statusStr = channel.IsLive ? "Live" : "Dead";
                    string redirectInfo = !string.IsNullOrEmpty(channel.DirectUrl) && channel.DirectUrl != channel.Url 
                        ? $" -> {channel.DirectUrl}" : "";
                    string resolutionStr = string.IsNullOrEmpty(channel.Resolution) ? "" : $" ({channel.Resolution})";
                    string logLine = $"[{time}] [{statusStr}] {channel.Name}{redirectInfo}{resolutionStr}";
                    
                    MainThread.BeginInvokeOnMainThread(() =>
                    {
                        LiveLogs.Add(logLine);
                        // Giữ tối đa 500 dòng log để không làm chậm UI
                        if (LiveLogs.Count > 500)
                        {
                            LiveLogs.RemoveAt(0);
                        }
                    });
                }
                finally
                {
                    Interlocked.Increment(ref _scannedCount);
                    OnPropertyChanged(nameof(ScannedCount));
                    OnPropertyChanged(nameof(ProgressFraction));
                    semaphore.Release();
                }
            });

            try
            {
                await Task.WhenAll(tasks);
            }
            catch (OperationCanceledException)
            {
                // Scan was stopped
            }
            finally
            {
                IsScanning = false;
                ((Command)StartScanCommand).ChangeCanExecute();
                ((Command)StopScanCommand).ChangeCanExecute();

                // Lưu file kết quả
                if (!string.IsNullOrEmpty(_currentFilePath) && LiveCount > 0 && !_cancellationTokenSource.IsCancellationRequested)
                {
                    string dir = Path.GetDirectoryName(_currentFilePath) ?? "";
                    string fileName = Path.GetFileNameWithoutExtension(_currentFilePath);
                    string outPath = Path.Combine(dir, $"{fileName}_checked.m3u");
                    
                    await M3uParser.SaveAsync(outPath, Channels);
                    
                    var openVlc = await Application.Current.MainPage.DisplayAlert("Complete", $"Scanned {TotalChannels} channels. {LiveCount} are live.\nResult: {outPath}\n\nDo you want to open the playlist with VLC?", "Open VLC", "No");
                    if (openVlc)
                    {
                        try 
                        {
                            string vlcPath1 = @"C:\Program Files\VideoLAN\VLC\vlc.exe";
                            string vlcPath2 = @"C:\Program Files (x86)\VideoLAN\VLC\vlc.exe";
                            string vlcTarget = "vlc"; // Default to PATH
                            
                            if (File.Exists(vlcPath1)) vlcTarget = vlcPath1;
                            else if (File.Exists(vlcPath2)) vlcTarget = vlcPath2;

                            Process.Start(new ProcessStartInfo { FileName = vlcTarget, Arguments = $"\"{outPath}\"", UseShellExecute = true });
                        } 
                        catch
                        {
                            try 
                            {
                                // Fallback
                                Process.Start(new ProcessStartInfo { FileName = outPath, UseShellExecute = true });
                            }
                            catch (Exception ex)
                            {
                                await Application.Current.MainPage.DisplayAlert("Error", "Could not automatically open VLC. You can open the file manually.", "OK");
                            }
                        }
                    }
                }
            }
        }

        private void StopScan()
        {
            _cancellationTokenSource?.Cancel();
            AddLog("Scan stopped by user.");
        }

        private void AddLog(string message)
        {
            string time = DateTime.Now.ToString("HH:mm:ss");
            MainThread.BeginInvokeOnMainThread(() =>
            {
                LiveLogs.Add($"[{time}] {message}");
            });
        }

        public event PropertyChangedEventHandler? PropertyChanged;
        protected void OnPropertyChanged([CallerMemberName] string? propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
