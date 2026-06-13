using System;
using System.Globalization;
using Microsoft.Maui.Controls;
using Microsoft.Maui.Graphics;

namespace Check_Online_IPTV.Converters
{
    public class BoolToColorConverter : IValueConverter
    {
        public object Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
        {
            if (value is bool isLive)
            {
                // Xanh lá cho Live, Đỏ cho Dead/Error, Xám nhạt cho Pending
                return isLive ? Color.FromArgb("#00ADB5") : Color.FromArgb("#FF2E63");
            }
            return Color.FromArgb("#EEEEEE");
        }

        public object ConvertBack(object? value, Type targetType, object? parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
