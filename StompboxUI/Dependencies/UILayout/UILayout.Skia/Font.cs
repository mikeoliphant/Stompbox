using System;
using SkiaSharp;
using System.Text;

namespace UILayout
{
    public partial class UIFont
    {
        public SKTypeface Typeface { get; set; }
        public float TextSize { get; set; }

        SKPaint measurePaint = new SKPaint
        {
            IsAntialias = true,
            Style = SKPaintStyle.StrokeAndFill
        };

        SKRect bounds = SKRect.Empty;

        public float TextHeight
        {
            get { return ((float)TextSize * 3.0f) / 4.0f; }
        }

        static UIFont()
        {
        }

        public static UIFont FromSpriteFont(SpriteFontDefinition spriteFont)
        {
            return null;
        }

        public bool HasGlyph(char c)
        {
            return true;
        }

        public void MeasureString(string text, out float width, out float height)
        {
            measurePaint.Typeface = Typeface;
            measurePaint.TextSize = TextSize;

            measurePaint.MeasureText(text, ref bounds);

            width = bounds.Width;
            height = bounds.Height;
        }

        public void MeasureString(ReadOnlySpan<char> text, out float width, out float height)
        {
            measurePaint.Typeface = Typeface;
            measurePaint.TextSize = TextSize;

            measurePaint.MeasureText(text, ref bounds);

            width = bounds.Width;
            height = bounds.Height;
        }


        public void MeasureString(StringBuilder sb, out float width, out float height)
        {
            // Beter not to implement StringBuilder if it has to do a string copy
            throw new NotImplementedException();

            //measurePaint.Typeface = Typeface;
            //measurePaint.TextSize = TextSize;

            //measurePaint.MeasureText(sb.ToString(), ref bounds);

            //width = bounds.Width;
            //height = bounds.Height;
        }
    }
}
