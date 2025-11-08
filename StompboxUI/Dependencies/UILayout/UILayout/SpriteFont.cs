using System;
using System.Collections.Generic;
using System.Text;
using System.Xml.Serialization;

namespace UILayout
{
    public struct SpriteFontGlyph
    {
        public UInt16 Character;
        public int X;
        public int Y;
        public int Width;
        public int Height;
    }

    public struct SpriteFontKernPair
    {
        public UInt16 Ch1;
        public UInt16 Ch2;
        public int Kern;
    }

    public class SpriteFontDefinition
    {
        public string Name { get; set; }
        public bool IsFixedWidth { get; set; }
        public int GlyphWidth { get; set; }
        public int GlyphHeight { get; set; }
        public int GlyphsPerRow { get; set; }
        public int LineHeight { get; set; }
        public string GlyphString { get; set; }
        public SpriteFontGlyph[] Glyphs { get; set; }
        public List<SpriteFontKernPair> KernPairs { get; set; }
    }

    public class SpriteFont
    {
        public float Spacing { get; set; }
        public float LineSpacing { get; set; }
        public float LineHeight { get; set; }
        public float GlyphHeight { get; set; }

        int numGlyphs = (127 - 32);

        protected UIImage fontImage;
        protected Dictionary<int, SpriteFontGlyph> glyphs = new Dictionary<int, SpriteFontGlyph>();
        protected int width;
        protected int height;

        Dictionary<(int, int), int> KernDict;

        [XmlIgnore]
        public UIImage FontImage
        {
            get { return fontImage; }
        }

        public float EmptyLinePercent { get; set; } = 1.0f;
        public float EmptyLineHeight
        {
            get
            {
                return LineHeight * EmptyLinePercent;
            }
        }

        public Dictionary<int, SpriteFontGlyph> Glyphs
        {
            get { return glyphs; }
            set { glyphs = value; }
        }

        public bool IsFixedWidth { get; protected set; }

        public static SpriteFont CreateFromDefinition(SpriteFontDefinition fontDefinition)
        {
            string name = fontDefinition.Name;

            if (fontDefinition.IsFixedWidth)
            {
                fontDefinition.Glyphs = new SpriteFontGlyph[fontDefinition.GlyphString.Length];

                int row = 0;
                int col = 0;

                for (int pos = 0; pos < fontDefinition.GlyphString.Length; pos++)
                {
                    if (col == fontDefinition.GlyphsPerRow)
                    {
                        row++;
                        col = 0;
                    }

                    fontDefinition.Glyphs[pos] = new SpriteFontGlyph
                    {
                        Character = fontDefinition.GlyphString[pos],
                        X = col * fontDefinition.GlyphWidth,
                        Y = row * fontDefinition.GlyphHeight,
                        Width = fontDefinition.GlyphWidth,
                        Height = fontDefinition.GlyphHeight
                    };

                    col++;
                }
            }

            SpriteFont font = new SpriteFont(Layout.Current.GetImage(name), fontDefinition.Glyphs);

            font.LineHeight = fontDefinition.LineHeight;
            font.GlyphHeight = fontDefinition.GlyphHeight;
            font.Spacing = 0;
            font.LineSpacing = 0;
            font.EmptyLinePercent = 0.5f;

            font.KernDict = new Dictionary<(int, int), int>();

            if ((fontDefinition.KernPairs != null) && (fontDefinition.KernPairs.Count > 0))
            {
                foreach (SpriteFontKernPair pair in fontDefinition.KernPairs)
                {
                    font.KernDict[(pair.Ch1, pair.Ch2)] = pair.Kern;
                }
            }

            return font;
        }

        public SpriteFont(UIImage fontImage, SpriteFontGlyph[] glyphs)
        {
            this.fontImage = fontImage;

            foreach (SpriteFontGlyph glyph in glyphs)
            {
                this.glyphs[glyph.Character] = glyph;
            }

            LineHeight = GlyphHeight = glyphs[0].Height;
        }

        public bool HasGlyph(char c)
        {
            return GetGlyphInternal(c) != null;
        }

        public SpriteFontGlyph GetGlyph(char c)
        {
            return GetGlyphInternal(c) ?? GetGlyphInternal('_') ?? glyphs[0xFF3F];
        }

        SpriteFontGlyph? GetGlyphInternal(char c)
        {
            if (!glyphs.ContainsKey(c))
            {
                // This really should be any non-unicode...
                if (IsFixedWidth)
                {
                    return glyphs[' '];
                }

                // Map ascii space to unicode space if necessary
                if (c == 0x20)
                {
                    return glyphs[0x3000];
                }
                else if (c == 0x2D)
                {
                    return glyphs[0x2212];
                }
                else if (c == 0x27)
                {
                    return glyphs[0x2019];
                }

                c = (char)(c + 0xFEE0);

                if (!glyphs.ContainsKey(c))
                {
                    return null;
                }
            }

            return glyphs[c];
        }

        public void MeasureString(String str, out float width, out float height)
        {
            MeasureString(str, 1, out width, out height);
        }

        public void MeasureString(String str, float scale, out float width, out float height)
        {
            (width, height, float xOffset, char lastChar) = MeasureString(str.AsSpan(), scale);
        }

        public void MeasureString(ReadOnlySpan<char> str, out float width, out float height)
        {
            MeasureString(str, 1, out width, out height);
        }

        public void MeasureString(ReadOnlySpan<char> str, float scale, out float width, out float height)
        {
            (width, height, float xOffset, char lastChar) = MeasureString(str, scale);
        }

        public void MeasureString(StringBuilder stringBuilder, out float width, out float height)
        {
            MeasureString(stringBuilder, 1, out width, out height);
        }

        public void MeasureString(StringBuilder stringBuilder, float scale, out float width, out float height)
        {
            (float Width, float Height, float XOffset, char LastChar) state = (0, 0, 0, '\0');

            foreach (ReadOnlyMemory<char> chunk in stringBuilder.GetChunks())
            {
                state = MeasureString(chunk.Span, scale, state.Width, state.Height, state.XOffset, state.LastChar);
            }

            width = state.Width;
            height = state.Height;
        }

        public (float Width, float Height, float XOffset, char LastChar) MeasureString(ReadOnlySpan<char> str, float scale, float width = 0, float height = 0, float xOffset = 0, char lastChar = '\0')
        {
            float rowWidth = xOffset;

            for (int i = 0; i < str.Length; i++)
            {
                char c = str[i];

                if (c == '\n')
                {
                    width = Math.Max(width, rowWidth);

                    height += (((rowWidth == 0) ? EmptyLineHeight : LineHeight) + LineSpacing) * scale;

                    rowWidth = 0;

                    lastChar = '\0';
                }
                else
                {
                    if (height == 0)
                        height = GlyphHeight * scale;

                    SpriteFontGlyph glyph = GetGlyph(c);

                    if (lastChar != '\0')
                    {
                        float spacing = Spacing;

                        int kern;

                        if ((KernDict != null) && KernDict.TryGetValue((lastChar, c), out kern))
                        {
                            spacing += kern;
                        }

                        rowWidth += spacing * scale;
                    }

                    rowWidth += glyph.Width * scale;

                    lastChar = c;
                }
            }

            width = Math.Max(width, rowWidth);

            return (width, height, rowWidth, lastChar);
        }

        public void DrawString(String str, GraphicsContext2D graphicsContext, float x, float y, UIColor color, float scale)
        {
            if (String.IsNullOrEmpty(str))
                return;

            DrawString(str.AsSpan(), graphicsContext, x, y, '\0', color, scale);
        }

        public void DrawString(ReadOnlySpan<char> str, GraphicsContext2D graphicsContext, float x, float y, UIColor color, float scale)
        {
            if (str.IsEmpty)
                return;

            DrawString(str, graphicsContext, x, y, '\0', color, scale);
        }

        public void DrawString(StringBuilder stringBuilder, GraphicsContext2D graphicsContext, float x, float y, UIColor color, float scale)
        {
            (float X, float Y, char LastChar) state = (x, y, '\0');

            foreach (ReadOnlyMemory<char> chunk in stringBuilder.GetChunks())
            {
                state = DrawString(chunk.Span, graphicsContext, state.X, state.Y, state.LastChar, color, scale);
            }
        }

        public (float X, float Y, char LastChar) DrawString(ReadOnlySpan<char> str, GraphicsContext2D graphicsContext, float x, float y, char lastChar, UIColor color, float scale)
        {
            float xOffset = x;
            float yOffset = y;

            System.Drawing.Rectangle drawRect = System.Drawing.Rectangle.Empty;

            for (int i = 0; i < str.Length; i++)
            {
                char c = str[i];

                if (c == '\n')
                {
                    yOffset += (((x == xOffset) ? EmptyLineHeight : LineHeight) + LineSpacing) * scale;
                    xOffset = x;

                    lastChar = '\0';
                }
                else
                {
                    SpriteFontGlyph glyph = GetGlyph(c);

                    drawRect.X = glyph.X;
                    drawRect.Y = glyph.Y;
                    drawRect.Width = glyph.Width;
                    drawRect.Height = glyph.Height;

                    if (lastChar != '\0')
                    {
                        float spacing = Spacing;

                        int kern;

                        if ((KernDict != null) && KernDict.TryGetValue((lastChar, c), out kern))
                        {
                            spacing += kern;
                        }

                        xOffset += spacing * scale;
                    }

                    graphicsContext.DrawImage(fontImage, (int)xOffset, (int)yOffset, drawRect, color, scale);

                    xOffset += glyph.Width * scale;

                    lastChar = c;
                }
            }

            return (xOffset, yOffset, lastChar);
        }
    }

    public class SpriteFontFixedWidth : SpriteFont
    {
        public SpriteFontFixedWidth(UIImage fontImage)
            : base(fontImage, new SpriteFontGlyph[0])   // fix this
        {
            this.fontImage = fontImage;

            width = fontImage.Width;
            height = fontImage.Height;

            InitializeFixedWidth();

            LineHeight = height;
        }

        void InitializeFixedWidth()
        {
            IsFixedWidth = true;

            LineHeight = 8;

            for (int i = 0; i < 26; i++)
            {
                glyphs['A' + i] = new SpriteFontGlyph { Character = (UInt16)('A' + i), X = i * 8, Y = 0, Width = 8, Height = 8 };
                glyphs['a' + i] = new SpriteFontGlyph { Character = (UInt16)('a' + i), X = i * 8, Y = 0, Width = 8, Height = 8 };
            }

            for (int i = 0; i < 10; i++)
            {
                glyphs['0' + i] = new SpriteFontGlyph { Character = (UInt16)('0' + i), X = i * 8, Y = 8, Width = 8, Height = 8 };
            }

            glyphs[' '] = new SpriteFontGlyph { Character = (UInt16)' ', X = width - 8, Y = height - 8, Width = 8, Height = 8 };

            glyphs['.'] = new SpriteFontGlyph { Character = (UInt16)('.'), X = 10 * 8, Y = 8, Width = 8, Height = 8 };
            glyphs[','] = new SpriteFontGlyph { Character = (UInt16)(','), X = 11 * 8, Y = 8, Width = 8, Height = 8 };
            glyphs[';'] = new SpriteFontGlyph { Character = (UInt16)(';'), X = 12 * 8, Y = 8, Width = 8, Height = 8 };
            glyphs[':'] = new SpriteFontGlyph { Character = (UInt16)(':'), X = 13 * 8, Y = 8, Width = 8, Height = 8 };
            glyphs['!'] = new SpriteFontGlyph { Character = (UInt16)('!'), X = 14 * 8, Y = 8, Width = 8, Height = 8 };
            glyphs['?'] = new SpriteFontGlyph { Character = (UInt16)('?'), X = 15 * 8, Y = 8, Width = 8, Height = 8 };
            glyphs['-'] = new SpriteFontGlyph { Character = (UInt16)('-'), X = 17 * 8, Y = 8, Width = 8, Height = 8 };
            glyphs['%'] = new SpriteFontGlyph { Character = (UInt16)('%'), X = 19 * 8, Y = 8, Width = 8, Height = 8 };
        }
    }
}
