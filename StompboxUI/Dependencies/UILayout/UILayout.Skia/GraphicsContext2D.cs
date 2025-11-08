using System;
using System.Drawing;
using System.Numerics;
using System.Text;
using SkiaSharp;

namespace UILayout
{
    public class GraphicsContext2D
    {
        public SKCanvas Canvas { get; set; }

        SKPaint colorPaint = new SKPaint()
        {           
        };

        SKPaint textPaint = new SKPaint
        {
            IsAntialias = true,
            Style = SKPaintStyle.StrokeAndFill,            
        };

        SKFont textFont = new SKFont()
        {
            
        };

        public void BeginDraw()
        {
        }

        public void EndDraw()
        {
        }

        public void DrawImage(UIImage image, float x, float y)
        {
            Canvas.DrawBitmap(image.Bitmap, new SKRect(image.XOffset, image.YOffset, image.Width, image.Height), new SKRect((int)x, (int)y, (int)x + image.Width, (int)y + image.Height));
        }

        public void DrawImage(UIImage image, float x, float y, in UIColor color)
        {
            colorPaint.Color = color.NativeColor;

            Canvas.DrawBitmap(image.Bitmap, new SKRect(image.XOffset, image.YOffset, image.Width, image.Height), new SKRect((int)x, (int)y, (int)x + image.Width, (int)y + image.Height), colorPaint);
        }

        public void DrawImage(UIImage image, float x, float y, in UIColor color, float scale)
        {
            colorPaint.Color = color.NativeColor;

            Canvas.DrawBitmap(image.Bitmap, new SKRect(image.XOffset, image.YOffset, image.Width, image.Height), new SKRect((int)x, (int)y, (int)x + image.Width, (int)y + image.Height), colorPaint);
        }

        public void DrawImage(UIImage image, float x, float y, in Rectangle srcRectangle)
        {
            Canvas.DrawBitmap(image.Bitmap, new SKRect(srcRectangle.X + image.XOffset, srcRectangle.Y + image.YOffset, srcRectangle.Right, srcRectangle.Bottom), new SKRect((int)x, (int)y, (int)x + image.Width, (int)y + image.Height));
        }

        public void DrawImage(UIImage image, float x, float y, in Rectangle srcRectangle, in UIColor color, float scale)
        {
            Canvas.DrawBitmap(image.Bitmap, new SKRect(srcRectangle.X + image.XOffset, srcRectangle.Y + image.YOffset, srcRectangle.Right, srcRectangle.Bottom), new SKRect((int)x, (int)y, (int)x + image.Width, (int)y + image.Height));
        }

        public void DrawImage(UIImage image, in Rectangle srcRectangle, in RectF destRectangle)
        {
            Canvas.DrawBitmap(image.Bitmap, new SKRect(srcRectangle.X + image.XOffset, srcRectangle.Y + image.YOffset, srcRectangle.Right, srcRectangle.Bottom), new SKRect((int)destRectangle.X, (int)destRectangle.Y, (int)destRectangle.Right, (int)destRectangle.Bottom));
        }

        public void DrawImage(UIImage image, in Rectangle srcRectangle, in RectF destRectangle, in UIColor color)
        {
            colorPaint.Color = color.NativeColor;

            Canvas.DrawBitmap(image.Bitmap, new SKRect(srcRectangle.X + image.XOffset, srcRectangle.Y + image.YOffset, srcRectangle.Right, srcRectangle.Bottom),
                new SKRect((int)destRectangle.X, (int)destRectangle.Y, (int)destRectangle.Right, (int)destRectangle.Bottom), colorPaint);
        }

        public void DrawImage(UIImage image, float x, float y, in UIColor color, float rotation, in Vector2 origin, float scale)
        {
            throw new NotImplementedException();
        }

        public void DrawRectangle(in RectF rectangle, in UIColor color)
        {
            colorPaint.Color = color.NativeColor;

            Canvas.DrawRect(new SKRect(rectangle.X, rectangle.Y, rectangle.Right, rectangle.Bottom), colorPaint);
        }

        public void DrawText(String text, UIFont font, float x, float y, in UIColor color)
        {
            textPaint.Color = color.NativeColor;
            textPaint.Typeface = font.Typeface;
            textPaint.TextSize = font.TextSize;

            Canvas.DrawText(text, x, y + font.TextHeight, textPaint);
        }

        public void DrawText(String text, UIFont font, float x, float y, in UIColor color, float scale)
        {
            textPaint.Color = color.NativeColor;
            textPaint.Typeface = font.Typeface;
            textPaint.TextSize = font.TextSize;

            Canvas.DrawText(text, x, y + font.TextHeight, textPaint);
        }

        public void DrawText(ReadOnlySpan<char> text, UIFont font, float x, float y, in UIColor color)
        {
            textFont.Typeface = font.Typeface;
            textPaint.Color = color.NativeColor;
            textPaint.Typeface = font.Typeface;
            textPaint.TextSize = font.TextSize;

            Canvas.DrawText(SKTextBlob.Create(text, textFont), x, y + font.TextHeight, textPaint);
        }

        public void DrawText(ReadOnlySpan<char> text, UIFont font, float x, float y, in UIColor color, float scale)
        {
            textPaint.Color = color.NativeColor;
            textPaint.Typeface = font.Typeface;
            textPaint.TextSize = font.TextSize;

            Canvas.DrawText(SKTextBlob.Create(text, textFont), x, y + font.TextHeight, textPaint);
        }

        public void DrawText(StringBuilder text, UIFont font, float x, float y, in UIColor color)
        {
            textPaint.Color = color.NativeColor;
            textPaint.Typeface = font.Typeface;
            textPaint.TextSize = font.TextSize;

            Canvas.DrawText(text.ToString(), x, y + font.TextHeight, textPaint);
        }

        public void DrawText(StringBuilder text, UIFont font, float x, float y, in UIColor color, float scale)
        {
            textPaint.Color = color.NativeColor;
            textPaint.Typeface = font.Typeface;
            textPaint.TextSize = font.TextSize;

            Canvas.DrawText(text.ToString(), x, y + font.TextHeight, textPaint);
        }
    }
}
