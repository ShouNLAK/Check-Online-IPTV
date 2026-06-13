using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace Check_Online_IPTV.Models
{
    public class ChannelItem : INotifyPropertyChanged
    {
        private string _name = string.Empty;
        public string Name
        {
            get => _name;
            set { _name = value; OnPropertyChanged(); }
        }

        private string _url = string.Empty;
        public string Url
        {
            get => _url;
            set { _url = value; OnPropertyChanged(); }
        }

        private string _extInf = string.Empty;
        public string ExtInf
        {
            get => _extInf;
            set { _extInf = value; OnPropertyChanged(); }
        }

        private string _status = "Pending";
        public string Status
        {
            get => _status;
            set { _status = value; OnPropertyChanged(); }
        }

        private string _resolution = string.Empty;
        public string Resolution
        {
            get => _resolution;
            set { _resolution = value; OnPropertyChanged(); }
        }

        private string _videoCodec = string.Empty;
        public string VideoCodec
        {
            get => _videoCodec;
            set { _videoCodec = value; OnPropertyChanged(); }
        }

        private string _audioCodec = string.Empty;
        public string AudioCodec
        {
            get => _audioCodec;
            set { _audioCodec = value; OnPropertyChanged(); }
        }
        
        private string _directUrl = string.Empty;
        public string DirectUrl
        {
            get => _directUrl;
            set 
            { 
                _directUrl = value; 
                OnPropertyChanged(); 
                OnPropertyChanged(nameof(DisplayUrl));
            }
        }

        private bool _isLive;
        public bool IsLive
        {
            get => _isLive;
            set { _isLive = value; OnPropertyChanged(); }
        }

        private int _responseCode;
        public int ResponseCode
        {
            get => _responseCode;
            set { _responseCode = value; OnPropertyChanged(); }
        }

        private long _connectionTimeMs;
        public long ConnectionTimeMs
        {
            get => _connectionTimeMs;
            set { _connectionTimeMs = value; OnPropertyChanged(); }
        }

        public string DisplayUrl => string.IsNullOrEmpty(DirectUrl) ? Url : DirectUrl;
        
        public bool IsRedirected => !string.IsNullOrEmpty(DirectUrl) && DirectUrl != Url;
        
        public string UrlColor => IsRedirected ? "#007ACC" : "#CCCCCC";

        public void ResetState()
        {
            Status = "Pending";
            Resolution = string.Empty;
            VideoCodec = string.Empty;
            AudioCodec = string.Empty;
            DirectUrl = string.Empty;
            IsLive = false;
            ResponseCode = 0;
            ConnectionTimeMs = 0;
            // Notify derived properties explicitly
            OnPropertyChanged(nameof(DisplayUrl));
            OnPropertyChanged(nameof(IsRedirected));
            OnPropertyChanged(nameof(UrlColor));
        }

        public event PropertyChangedEventHandler? PropertyChanged;
        protected void OnPropertyChanged([CallerMemberName] string? propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
