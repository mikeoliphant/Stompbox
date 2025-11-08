using System;
using System.Text;

namespace UILayout
{
    public static class StringBuilderExtensions
    {
        static char[] numberBuffer = new char[16];  // Not thread safe - should find a better way

        public static StringBuilder AppendNumber(this StringBuilder stringBuilder, int number)
        {
            return stringBuilder.AppendNumber(number, 0);
        }

        public static StringBuilder AppendNumber(this StringBuilder stringBuilder, int number, int minDigits)
        {
            if (number < 0)
            {
                stringBuilder.Append('-');
                number = -number;
            }

            int index = 0;

            do
            {
                int digit = number % 10;
                numberBuffer[index] = (char)('0' + digit);
                number /= 10;
                ++index;
            }
            while (number > 0 || index < minDigits);

            for (--index; index >= 0; --index)
            {
                stringBuilder.Append(numberBuffer[index]);
            }

            return stringBuilder;
        }
    }

    public class TextBlock : UIElement
    {
        public string Text { get; set; }

        public UIColor TextColor { get; set; }
        public UIFont TextFont { get; set; }

        public TextBlock()
        {
            TextFont = Layout.Current.DefaultFont;
            TextColor = Layout.Current.DefaultForegroundColor;
        }

        public TextBlock(string text)
            : this()
        {
            Text = text;
        }

        protected override void GetContentSize(out float width, out float height)
        {
            if (string.IsNullOrEmpty(Text))
            {
                width = 0;
                height = 0;
            }
            else
            {
                float textWidth = 0;
                float textHeight = 0;

                TextFont.MeasureString(Text, out textWidth, out textHeight);

                width = textWidth;
                height = textHeight;
            }
        }

        protected override void DrawContents()
        {
            base.DrawContents();

            Layout.Current.GraphicsContext.DrawText(Text, TextFont, ContentBounds.X, ContentBounds.Y, TextColor);
        }
    }

    public class StringBuilderTextBlock : UIElement
    {
        public StringBuilder StringBuilder { get; set; }

        public UIColor TextColor { get; set; }
        public UIFont TextFont { get; set; }

        public StringBuilderTextBlock()
        {
            TextFont = Layout.Current.DefaultFont;
            TextColor = Layout.Current.DefaultForegroundColor;

            StringBuilder = new StringBuilder();
        }

        public StringBuilderTextBlock(string text)
            : this()
        {
            StringBuilder = new StringBuilder(text);
        }

        protected override void GetContentSize(out float width, out float height)
        {
            if (StringBuilder.Length == 0)
            {
                width = 0;
                height = 0;
            }
            else
            {
                float textWidth = 0;
                float textHeight = 0;

                TextFont.MeasureString(StringBuilder, out textWidth, out textHeight);

                width = textWidth;
                height = textHeight;
            }
        }

        protected override void DrawContents()
        {
            base.DrawContents();

            Layout.Current.GraphicsContext.DrawText(StringBuilder, TextFont, ContentBounds.X, ContentBounds.Y, TextColor);
        }
    }
}
