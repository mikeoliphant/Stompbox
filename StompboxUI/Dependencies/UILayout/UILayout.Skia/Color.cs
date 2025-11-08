using SkiaSharp;

namespace UILayout
{
    public partial struct UIColor
    {
        public SKColor NativeColor { get { return new SKColor(R, G, B, A); } }
    }
}
