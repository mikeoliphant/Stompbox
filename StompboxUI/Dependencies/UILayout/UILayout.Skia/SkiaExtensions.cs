using SkiaSharp;

namespace UILayout
{
    public static class SkiaExtensions
    {
        public static SKRect ToSKRect(this RectF rectangleF)
        {
            return new SKRect(rectangleF.Left, rectangleF.Top, rectangleF.Right, rectangleF.Bottom);
        }
    }
}
