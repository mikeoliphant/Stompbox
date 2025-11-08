using System.Globalization;
using System.Numerics;
using System.Runtime.InteropServices;

namespace UILayout
{
    [StructLayout(LayoutKind.Sequential, Size = 4)]
    public partial struct UIColor
    {
        public static UIColor Transparent { get => new UIColor(0, 0, 0, 0); }
        public static UIColor White { get => new UIColor(255, 255, 255); }
        public static UIColor Black { get => new UIColor(0, 0, 0); }
        public static UIColor Red { get => new UIColor(255, 0, 0); }
        public static UIColor Green { get => new UIColor(0, 255, 0); }
        public static UIColor Blue { get => new UIColor(0, 0, 255); }
        public static UIColor Yellow { get => new UIColor(255, 255, 0); }
        public static UIColor Cyan { get => new UIColor(0, 255, 255); }
        public static UIColor Magenta { get => new UIColor(255, 0, 255); }
        public static UIColor Orange { get => new UIColor(255, 165, 0); }

        public byte R;
        public byte G;
        public byte B;
        public byte A;

        public UIColor(byte r, byte g, byte b, byte a)
        {
            R = r;
            G = g;
            B = b;
            A = a;
        }

        public UIColor(byte r, byte g, byte b)
        {
            R = r;
            G = g;
            B = b;
            A = 255;
        }

        public UIColor(int r, int g, int b)
        {
            this.R = (byte)r;
            this.G = (byte)g;
            this.B = (byte)b;
            this.A = (byte)255;
        }

        public UIColor(int r, int g, int b, int a)
        {
            this.R = (byte)r;
            this.G = (byte)g;
            this.B = (byte)b;
            this.A = (byte)a;
        }

        public UIColor(float r, float g, float b)
        {
            this.R = (byte)(r * byte.MaxValue);
            this.G = (byte)(g * byte.MaxValue);
            this.B = (byte)(b * byte.MaxValue);
            this.A = (byte)255;
        }

        public UIColor(float r, float g, float b, float a)
        {
            this.R = (byte)(r * byte.MaxValue);
            this.G = (byte)(g * byte.MaxValue);
            this.B = (byte)(b * byte.MaxValue);
            this.A = (byte)(a * byte.MaxValue);
        }

        public UIColor(in Vector3 colorVec)
        {
            R = (byte)(colorVec.X * 255);
            G = (byte)(colorVec.Y * 255);
            B = (byte)(colorVec.Z * 255);
            A = (byte)255;
        }

        public static UIColor operator *(UIColor c, float value)
        {
            return new UIColor((byte)(((float)c.R * value)), (byte)(((float)c.G * value)), (byte)(((float)c.B * value)), c.A);
        }

        public static UIColor operator *(UIColor c1, UIColor c2)
        {
            return new UIColor((c1.R * c2.R) / 256, (c1.G * c2.G) / 256, (c1.B * c2.B) / 256, (c1.A * c2.A) / 256);
        }

        public static bool operator ==(UIColor c1, UIColor c2)
        {
            return (c1.R == c2.R) && (c1.G == c2.G) && (c1.B == c2.B) && (c1.A == c2.A);
        }

        public static bool operator !=(UIColor c1, UIColor c2)
        {
            return !(c1 == c2);
        }

        public Vector3 ToVector3()
        {
            return new Vector3((float)R / 255.0f, (float)G / 255.0f, (float)B / 255.0f);
        }

        public static UIColor FromHex(string hex)
        {
            if (!string.IsNullOrEmpty(hex))
            {
                string colorStr = hex.TrimStart('#');

                if (colorStr.Length == 6)
                {
                    return new UIColor(int.Parse(colorStr.Substring(0, 2), NumberStyles.HexNumber),
                        int.Parse(colorStr.Substring(2, 2), NumberStyles.HexNumber),
                        int.Parse(colorStr.Substring(4, 2), NumberStyles.HexNumber));
                }
            }

            return UIColor.Transparent;
        }

        public static UIColor Lerp(UIColor color1, UIColor color2, float lerp)
        {
            return new UIColor((int)MathUtil.Lerp(color1.R, color2.R, lerp),
                (int)MathUtil.Lerp(color1.G, color2.G, lerp),
                (int)MathUtil.Lerp(color1.B, color2.B, lerp),
                (int)MathUtil.Lerp(color1.A, color2.A, lerp));
        }

        public UIColor MultiplyAlpha(float alpha)
        {
            return new UIColor(R, G, B, (byte)(A * alpha));
        }

        public static UIColor Blend(UIColor srcColor, UIColor destColor)
        {
            float srcA = (float)srcColor.A / 255.0f;
            float oneMinusSrcA = 1 - srcA;

            return new UIColor((byte)(((float)srcColor.R * srcA) + ((float)destColor.R * oneMinusSrcA)),
                 (byte)(((float)srcColor.G * srcA) + ((float)destColor.G * oneMinusSrcA)),
                 (byte)(((float)srcColor.B * srcA) + ((float)destColor.B * oneMinusSrcA)),
                 (byte)(((float)srcColor.A) + ((float)destColor.A * oneMinusSrcA)));
        }
    }
}
