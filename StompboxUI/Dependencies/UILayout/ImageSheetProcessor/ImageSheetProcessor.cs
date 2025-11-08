using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using System.Xml.Serialization;
using SharpFont;
using UILayout;

namespace ImageSheetProcessor
{
    public class ProcImage : UIColorCanvas
    {
        public override int ImageWidth { get { return imageWidth; } }
        public override int ImageHeight { get { return imageHeight; } }

        int imageWidth = 0;
        int imageHeight = 0;

        public ProcImage(string imageFileName)
        {
            using (Stream pngStream = File.OpenRead(imageFileName))
            {
                var image = StbImageSharp.ImageResult.FromStream(pngStream, StbImageSharp.ColorComponents.RedGreenBlueAlpha);

                this.imageWidth = image.Width;
                this.imageHeight = image.Height;

                canvasData = MemoryMarshal.Cast<byte, UIColor>(image.Data).ToArray();
            }
        }

        public ProcImage(int imageWidth, int imageHeight)
        {
            this.imageWidth = imageWidth;
            this.imageHeight = imageHeight;

            canvasData = new UIColor[imageWidth * imageHeight];
        }

        public Span<byte> GetByteData()
        {
            return MemoryMarshal.Cast<UIColor, byte>(canvasData);
        }
    }

    public class ImageSheetEntry
    {
        public ProcImage Image { get; set; }
        public string NamePrefix { get; set; }
    }

    public class ImageSheetProcessor
    {
        public string SrcPath { get; set; }
        public string DestPath { get; set; }
        public int VerticalPadding { get; set; }
        public bool ForceRegen { get; set; }
        public bool PadToPowerOfTwo { get; set; }
        public int MaxImageSheetSize { get; set; }
        public int ImageGutterSize { get; set; }
        public bool WriteToDDS { get; set; }
        public UIColor ImageMaskColor { get; set; }
        public string ImageNamePrefix { get; set; }
        public bool InSpriteSheet
        {
            get { return imageSheetGroupName != null; }
        }

        protected ImageManifest imageManifest;
        protected Dictionary<string, ImageManifestSheetImage> imageDictionary = new Dictionary<string, ImageManifestSheetImage>();
        protected XmlSerializer serializer;
        protected string imageSheetGroupName;
        protected ImageManifestSheet imageSheet;
        protected Dictionary<string, ImageSheetEntry> imageSheetGroupImages = new Dictionary<string, ImageSheetEntry>();
        protected string tmpDir;
        protected List<string> directoryStack = new List<string>();

        Dictionary<int, int> jisMapping;

        public ImageSheetProcessor()
        {
            PadToPowerOfTwo = true;
            MaxImageSheetSize = 1024;
            ImageGutterSize = 1;
            ImageMaskColor = UIColor.Transparent;
            serializer = new XmlSerializer(typeof(ImageManifest));
        }

        public void BeginRenderImages(string destPath)
        {
            DestPath = destPath;

            tmpDir = Path.Combine(SrcPath, "tmp");

            if (!Directory.Exists(tmpDir))
            {
                Directory.CreateDirectory(tmpDir);
            }

            imageManifest = null;

            if (!ForceRegen)
            {
                try
                {
                    using (Stream reader = File.Open(Path.Combine(DestPath, "ImageManifest.xml"), FileMode.Open))
                    {
                        imageManifest = serializer.Deserialize(reader) as ImageManifest;
                    }
                }
                catch (Exception ex)
                {
                }
            }

            if (imageManifest == null)
                imageManifest = new ImageManifest();

            imageDictionary.Clear();

            imageManifest.SpriteSheets.Clear();
        }

        public void EndRenderImages()
        {
            using (Stream writer = File.Open(Path.Combine(DestPath, "ImageManifest.xml"), FileMode.Create))
            {
                serializer.Serialize(writer, imageManifest);
            }
        }

        public void PushDirectory(string directory)
        {
            directoryStack.Insert(directoryStack.Count, directory);
        }

        public void PopDirectory()
        {
            directoryStack.RemoveAt(directoryStack.Count - 1);
        }

        public string GetSourcePath(string imageName)
        {
            return GetSourcePath(imageName, ".png");
        }

        public string GetSourcePath(string imageName, string extension)
        {

            return Path.Combine(GetSourcePath(), imageName) + extension;
        }

        public string GetSourcePath()
        {
            string path = SrcPath;

            foreach (string directory in directoryStack)
            {
                path = Path.Combine(path, directory);
            }

            return path;
        }

        public void BeginSpriteSheetGroup(string groupName)
        {
            BeginSpriteSheetGroup(groupName, false, false);
        }

        public void BeginSpriteSheetGroup(string groupName, bool requireReadbleImages, bool generateImageMasks)
        {
            this.imageSheetGroupName = groupName;

            imageSheetGroupImages.Clear();

            imageSheet = new ImageManifestSheet();
        }

        public void EndSpriteSheetGroup()
        {
            int sheetNum = 0;

            MaxRectsBinPack binPack = new MaxRectsBinPack(MaxImageSheetSize, MaxImageSheetSize, false);

            List<string> images = new List<string>(imageSheetGroupImages.Keys);

            ProcImage outBitmap = new (MaxImageSheetSize, MaxImageSheetSize);

            images.Sort(delegate (string s1, string s2) { return Math.Max(imageSheetGroupImages[s2].Image.ImageHeight, imageSheetGroupImages[s2].Image.ImageWidth).
                CompareTo(Math.Max(imageSheetGroupImages[s1].Image.ImageHeight, imageSheetGroupImages[s1].Image.ImageWidth)); });

            do
            {
                string filename;
                ImageSheetEntry imageBitmap;
                Rectangle outRect;

                int pos = 0;

                do
                {
                    filename = images[pos];

                    imageBitmap = imageSheetGroupImages[filename];

                    outRect = binPack.Insert(imageBitmap.Image.ImageWidth + (ImageGutterSize * 2), imageBitmap.Image.ImageHeight + (ImageGutterSize * 2), MaxRectsBinPack.FreeRectChoiceHeuristic.RectContactPointRule);

                    if (outRect.Height > 0)
                        break;

                    pos++;
                }
                while (pos < images.Count);

                if (outRect.Height > 0)
                {
                    DrawImageWithGutter(imageBitmap.Image, outBitmap, outRect);

                    outRect.X += ImageGutterSize;
                    outRect.Y += ImageGutterSize;
                    outRect.Width -= (ImageGutterSize * 2);
                    outRect.Height -= (ImageGutterSize * 2);

                    ImageManifestSheetImage imageEntry = new ImageManifestSheetImage();
                    imageEntry.ImageName = Path.GetFileNameWithoutExtension(filename);

                    if (!String.IsNullOrEmpty(imageBitmap.NamePrefix))
                        imageEntry.ImageName = imageBitmap.NamePrefix + imageEntry.ImageName;

                    imageEntry.XOffset = outRect.X;
                    imageEntry.YOffset = outRect.Y;
                    imageEntry.Width = outRect.Width;
                    imageEntry.Height = outRect.Height;

                    imageSheet.Images.Add(imageEntry);

                    images.RemoveAt(pos);
                }

                if ((outRect.Height == 0) || (images.Count == 0))
                {
                    imageSheet.SheetName = imageSheetGroupName + sheetNum;
                    imageSheet.SheetWidth = MaxImageSheetSize;
                    imageSheet.SheetHeight = MaxImageSheetSize;
                    imageManifest.SpriteSheets.Add(imageSheet);

                    if (images.Count == 0)
                    {
                        ProcImage shrunk = AttemptShrink(imageSheet, outBitmap);

                        if (shrunk != null)
                        {
                            outBitmap = shrunk;
                        }
                    }

                    SaveImage(outBitmap, Path.Combine(DestPath, imageSheetGroupName + sheetNum + ".png"));

                    sheetNum++;

                    binPack.Init(MaxImageSheetSize, MaxImageSheetSize, false);

                    outBitmap = new (MaxImageSheetSize, MaxImageSheetSize);
                    imageSheet = new ImageManifestSheet();
                }
            }
            while (images.Count > 0);

            this.imageSheetGroupName = null;
            this.imageSheet = null;
        }

        void DrawImageWithGutter(ProcImage imageBitmap, ProcImage outBitmap, Rectangle outRect)
        {
            Rectangle srcRect;
            Rectangle destRect;

            // Top
            srcRect = new Rectangle(0, 0, imageBitmap.ImageWidth, 1);
            destRect = new Rectangle(outRect.X + ImageGutterSize, outRect.Y, outRect.Width - (ImageGutterSize * 2), ImageGutterSize);
            outBitmap.Draw(imageBitmap, destRect, srcRect);

            // Bottom
            srcRect = new Rectangle(0, imageBitmap.ImageHeight - 1, imageBitmap.ImageWidth, 1);
            destRect = new Rectangle(outRect.X + ImageGutterSize, outRect.Bottom - ImageGutterSize, outRect.Width - (ImageGutterSize * 2), ImageGutterSize);
            outBitmap.Draw(imageBitmap, destRect, srcRect);

            // Left
            srcRect = new Rectangle(0, 0, 1, imageBitmap.ImageHeight);
            destRect = new Rectangle(outRect.X, outRect.Y + ImageGutterSize, ImageGutterSize, outRect.Height - (ImageGutterSize * 2));
            outBitmap.Draw(imageBitmap, destRect, srcRect);

            // Right
            srcRect = new Rectangle(imageBitmap.ImageWidth - 1, 0, 1, imageBitmap.ImageHeight);
            destRect = new Rectangle(outRect.Right - ImageGutterSize, outRect.Y + ImageGutterSize, ImageGutterSize, outRect.Height - (ImageGutterSize * 2));
            outBitmap.Draw(imageBitmap, destRect, srcRect);

            // TopLeft
            srcRect = new Rectangle(0, 0, 1, 1);
            destRect = new Rectangle(outRect.X, outRect.Y, ImageGutterSize, ImageGutterSize);
            outBitmap.Draw(imageBitmap, destRect, srcRect);

            // TopRight
            srcRect = new Rectangle(imageBitmap.ImageWidth - 1, 0, 1, 1);
            destRect = new Rectangle(outRect.Right - ImageGutterSize, outRect.Y, ImageGutterSize, ImageGutterSize);
            outBitmap.Draw(imageBitmap, destRect, srcRect);

            // BottomLeft
            srcRect = new Rectangle(0, imageBitmap.ImageHeight - 1, 1, 1);
            destRect = new Rectangle(outRect.X, outRect.Bottom - ImageGutterSize, ImageGutterSize, ImageGutterSize);
            outBitmap.Draw(imageBitmap, destRect, srcRect);

            // BottomLeft
            srcRect = new Rectangle(imageBitmap.ImageWidth - 1, imageBitmap.ImageHeight - 1, 1, 1);
            destRect = new Rectangle(outRect.Right - ImageGutterSize, outRect.Bottom - ImageGutterSize, ImageGutterSize, ImageGutterSize);
            outBitmap.Draw(imageBitmap, destRect, srcRect);

            outRect.X += ImageGutterSize;
            outRect.Y += ImageGutterSize;
            outRect.Width -= (ImageGutterSize * 2);
            outRect.Height -= (ImageGutterSize * 2);

            outBitmap.Draw(imageBitmap, outRect, new Rectangle(0, 0, imageBitmap.ImageWidth, imageBitmap.ImageHeight));
        }

        ProcImage AttemptShrink(ImageManifestSheet imageSheet, ProcImage sourceBitmap)
        {
            int width = imageSheet.SheetWidth;
            int height = imageSheet.SheetHeight;

            List<ImageManifestSheetImage> newImages = new List<ImageManifestSheetImage>();

            if (width > height)
            {
                width /= 2;
            }
            else
            {
                height /= 2;
            }

            MaxRectsBinPack binPack = new MaxRectsBinPack(width, height, false);
            ProcImage outBitmap = new (width, height);

            newImages.Clear();

            foreach (ImageManifestSheetImage sprite in imageSheet.Images)
            {
                int gutterWidth = ImageGutterSize;
                int gutterHeight = ImageGutterSize;

                if (sprite.Width == width)
                    gutterWidth = 0;

                Rectangle outRect = binPack.Insert(sprite.Width + (gutterWidth * 2), sprite.Height + (gutterHeight * 2), MaxRectsBinPack.FreeRectChoiceHeuristic.RectContactPointRule);

                if (outRect.Height == 0)
                {
                    return null;
                }

                outBitmap.Draw(sourceBitmap, outRect, new Rectangle(sprite.XOffset - gutterWidth, sprite.YOffset - gutterHeight, sprite.Width + (gutterWidth * 2), sprite.Height + (gutterHeight * 2)));

                outRect.X += gutterWidth;
                outRect.Y += gutterHeight;
                outRect.Width -= (gutterWidth * 2);
                outRect.Height -= (gutterHeight * 2);

                ImageManifestSheetImage spriteEntry = new ImageManifestSheetImage();
                spriteEntry.ImageName = sprite.ImageName;
                spriteEntry.XOffset = outRect.X;
                spriteEntry.YOffset = outRect.Y;
                spriteEntry.Width = outRect.Width;
                spriteEntry.Height = outRect.Height;

                newImages.Add(spriteEntry);
            }

            imageSheet.SheetWidth = width;
            imageSheet.SheetHeight = height;
            imageSheet.Images = newImages;

            ProcImage shrunk = AttemptShrink(imageSheet, outBitmap);

            if (shrunk != null)
            {
                return shrunk;
            }
            else
            {
                return outBitmap;
            }
        }

        public void ToBlack(ProcImage srcBitmap)
        {
            byte a = 0;

            for (int x = 0; x < srcBitmap.ImageWidth; x++)
            {
                for (int y = 0; y < srcBitmap.ImageHeight; y++)
                {
                    UIColor color = srcBitmap.GetPixel(x, y);

                    if (color.A == 255)
                    {
                        srcBitmap.SetPixel(x, y, UIColor.Black);
                    }
                }
            }
        }

        public void Add(string textureName, ProcImage bitmap)
        {
            string destFile = Path.Combine(DestPath, textureName) + ".png";
            SaveAndManifest(bitmap, destFile);
        }

        public void Add(string textureName)
        {
            string srcFile = GetSourcePath(textureName);
            string destFile = Path.Combine(DestPath, textureName) + ".png";

            ProcImage original = new(srcFile);

            SaveAndManifest(original, destFile);
        }

        public void AddWithShadow(string textureName)
        {
            string srcFile = GetSourcePath(textureName);
            string destFile = Path.Combine(DestPath, textureName) + ".png";

            if (imageSheetGroupName != null)
            {
                if (!ForceRegen)
                {
                    string tmpFile = Path.Combine(tmpDir, textureName) + ".png";

                    if (File.Exists(tmpFile) && (new FileInfo(srcFile).LastWriteTime < new FileInfo(tmpFile).LastWriteTime))
                    {
                        imageSheetGroupImages[destFile] = new ImageSheetEntry { Image = new (tmpFile), NamePrefix = ImageNamePrefix };

                        return;
                    }
                }
            }
            else
            {
                if (!ForceRegen && !IsNewer(srcFile, destFile))
                    return;
            }

            ProcImage original = new(srcFile);

            int imageWidth = original.ImageWidth;
            int imageHeight = original.ImageHeight;

            ProcImage newBitmap = new (original.ImageWidth, (original.ImageHeight + (VerticalPadding * 2)));

            AddShadow(original, newBitmap, new Rectangle(0, 0, imageWidth, imageHeight), new Point(0, VerticalPadding));

            SaveAndManifest(newBitmap, destFile);
        }

        public ProcImage AddShadow(ProcImage srcBitmap)
        {
            ProcImage newBitmap = new (srcBitmap.ImageWidth, srcBitmap.ImageHeight);

            AddShadow(srcBitmap, newBitmap);

            return newBitmap;
        }

        public void AddShadow(ProcImage srcBitmap, ProcImage destBitmap)
        {
            AddShadow(srcBitmap, destBitmap, new Rectangle(0, 0, srcBitmap.ImageWidth, srcBitmap.ImageHeight), new Point(0, 0));
        }

        public void AddShadow(ProcImage srcBitmap, ProcImage destBitmap, Rectangle srcRect, Point destOffset)
        {
            ProcImage blurredImage = new (srcRect.Width, srcRect.Height);
            GaussianBlur blur = new GaussianBlur(3, 1.0f / 1.5f);

            destBitmap.Draw(srcBitmap, srcRect, srcRect);

            ToBlack(destBitmap);

            blur.Apply(destBitmap, blurredImage);

            destBitmap.Draw(blurredImage, new Rectangle(destOffset.X, destOffset.Y, srcRect.Width, srcRect.Height), new Rectangle(0, 0, srcRect.Width, srcRect.Height));
            destBitmap.DrawBlend(srcBitmap, new Rectangle(destOffset.X, destOffset.Y, srcRect.Width, srcRect.Height), new Rectangle(0, 0, srcRect.Width, srcRect.Height));
        }

        public void FillVertical(ProcImage bitmap, int padding)
        {
            for (int offset = 0; offset < padding; offset++)
            {
                for (int x = 0; x < bitmap.ImageWidth; x++)
                {
                    bitmap.SetPixel(x, offset, bitmap.GetPixel(x, padding));

                    bitmap.SetPixel(x, bitmap.ImageHeight - (offset + 1), bitmap.GetPixel(x, bitmap.ImageHeight - (padding + 1)));
                }
            }
        }

        public void AddSvg(string svgName)
        {
            AddSvg(svgName, null);
        }

        public void AddSvg(string svgName, float size)
        {
            AddSvg(svgName, new SizeF(size, size));
        }

        public void AddSvg(string svgName, SizeF? size)
        {
            string srcFile = Path.Combine(GetSourcePath(), svgName) + ".svg";
            string destFile = Path.Combine(DestPath, svgName) + ".png";

            var svg = Svg.SvgDocument.Open(srcFile);

            if (size == null)
            {
                size = Size.Round(svg.GetDimensions());
            }

            Bitmap bitmap = svg.Draw((int)size.Value.Width, (int)size.Value.Height);

            SaveAndManifest(ImageFromBitmap(bitmap), destFile);
        }

        public void AddFont(string name, string fontPath, float emSize)
        {
            AddFont(name, fontPath, emSize, 0x20, 0xff, 16);
        }

        public void AddFont(string name, string fontPath, float emSize, UInt16 minChar, UInt16 maxChar, int glphsPerLine)
        {
            Library library = new();

            Face face = library.NewFace(fontPath, 0);

            //face.SetCharSize(0, 62, 96, 96);

            face.SetPixelSizes(0, (uint)Math.Round((emSize * 96.0f) / 72.0f));

            string destFile = Path.Combine(DestPath, name) + ".png";

            SpriteFontDefinition fontEntry = new SpriteFontDefinition();

            fontEntry.Name = name;
            fontEntry.LineHeight = (int)face.Size.Metrics.Height;

            var ascentDescent = GetMaxDescent(face, minChar, maxChar);

            fontEntry.GlyphHeight = ascentDescent.MaxAscent + ascentDescent.MaxDescent;

            if (face.HasKerning)
            {
                fontEntry.KernPairs = new List<SpriteFontKernPair>();

                for (UInt16 kern1 = minChar; kern1 <= maxChar; kern1++)
                {
                    for (UInt16 kern2 = 0; kern2 <= maxChar; kern2++)
                    {
                        float kern = (float)face.GetKerning(kern1, kern2, KerningMode.Default).X;

                        if (kern != 0)
                        {
                            fontEntry.KernPairs.Add(new SpriteFontKernPair { Ch1 = kern1, Ch2 = kern2, Kern = (int)kern });
                        }
                    }
                }
            }

            List<int> charVals = new();
            List<ProcImage> bitmaps = new();
            List<Rectangle> cropRects = new();
            List<int> xPositions = new();
            List<int> yPositions = new();

            const int padding = 1;

            int width = padding;
            int height = padding;
            int lineWidth = padding;
            int lineHeight = padding;
            int count = 0;

            for (char ch = (char)minChar; ch < maxChar; ch++)
            {
                ProcImage charBitmap = RasterizeCharacter(ch, face, fontEntry.GlyphHeight, ascentDescent.MaxDescent);

                if (charBitmap == null)
                    continue;

                Rectangle cropRect = new Rectangle(0, 0, charBitmap.ImageWidth, charBitmap.ImageHeight);

                charVals.Add(ch);
                bitmaps.Add(charBitmap);
                xPositions.Add(lineWidth);
                yPositions.Add(height);
                cropRects.Add(cropRect);

                lineWidth += cropRect.Width + padding;
                lineHeight = Math.Max(lineHeight, cropRect.Height + padding);

                count++;

                if ((count == glphsPerLine) || (ch == maxChar - 1))
                {
                    width = Math.Max(width, lineWidth);
                    height += lineHeight;
                    lineWidth = padding;
                    lineHeight = padding;
                    count = 0;
                }
            }

            fontEntry.Glyphs = new SpriteFontGlyph[cropRects.Count];

            for (int i = 0; i < cropRects.Count; i++)
            {
                fontEntry.Glyphs[i].Character = (ushort)(charVals[i]);
                fontEntry.Glyphs[i].X = xPositions[i];
                fontEntry.Glyphs[i].Y = yPositions[i];
                fontEntry.Glyphs[i].Width = cropRects[i].Width;
                fontEntry.Glyphs[i].Height = cropRects[i].Height;
            }

            imageSheet.Fonts.Add(fontEntry);

            ProcImage bitmap = new (width, height);

            for (int i = 0; i < bitmaps.Count; i++)
            {
                bitmap.Draw(bitmaps[i], new Rectangle(xPositions[i], yPositions[i], cropRects[i].Width, cropRects[i].Height), cropRects[i]);
            }

            SaveAndManifest(bitmap, destFile);
        }

        (int MaxAscent, int MaxDescent) GetMaxDescent(Face face, UInt16 minChar, UInt16 maxChar)
        {
            int maxDescent = 0;
            int maxAscent = 0;

            for (char ch = (char)minChar; ch < maxChar; ch++)
            {
                uint glyphIndex = face.GetCharIndex(ch);

                if (glyphIndex == 0)
                    continue;    // no glyph

                face.LoadGlyph(glyphIndex, LoadFlags.Default, LoadTarget.Normal);

                int descent = (int)face.Glyph.Metrics.Height - (int)face.Glyph.Metrics.HorizontalBearingY;

                if (descent > maxDescent)
                    maxDescent = descent;

                int ascent = (int)face.Glyph.Metrics.HorizontalBearingY;

                if (ascent > maxAscent)
                    maxAscent = ascent;
            }

            return (maxAscent, maxDescent);
        }

        ProcImage RasterizeCharacter(char ch, Face face, int maxHeight, int maxDescent)
        {
            uint glyphIndex = face.GetCharIndex(ch);

            if (glyphIndex == 0)
                return null;    // no glyph

            face.LoadGlyph(glyphIndex, LoadFlags.Default, LoadTarget.Normal);
            face.Glyph.RenderGlyph(RenderMode.Normal);

            int width = (int)face.Glyph.Metrics.Width;
            int height = (int)face.Glyph.Metrics.Height;

            if (face.Glyph.Bitmap.PixelMode == PixelMode.Mono)
            {
                throw new InvalidDataException("Mono fonts not supported");
            }

            int yOffset = maxHeight - maxDescent - face.Glyph.BitmapTop;

            ProcImage bitmap = new((int)face.Glyph.Metrics.HorizontalAdvance, maxHeight);

            if ((width > 0) && (height > 0))
            {
                byte[] buffer = face.Glyph.Bitmap.BufferData;

                int bufPos = 0;

                for (int y = 0; y < height; y++)
                {
                    for (int x = 0; x < width; x++)
                    {
                        bitmap.SetPixel(x + face.Glyph.BitmapLeft, y + yOffset, new UIColor((byte)255, (byte)255, (byte)255, buffer[bufPos++]));
                    }
                }
            }

            return bitmap;
        }

        private static bool BitmapIsEmpty(ProcImage bitmap, int x)
        {
            for (int y = 0; y < bitmap.ImageHeight; y++)
            {
                if (bitmap.GetPixel(x, y).A != 0)
                    return false;
            }

            return true;
        }

        ProcImage ImageFromBitmap(Bitmap bitmap)
        {
            BitmapData bitmapData = bitmap.LockBits(new Rectangle(0, 0, bitmap.Width, bitmap.Height), ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);

            ProcImage image = new ProcImage(bitmap.Width, bitmap.Height);

            unsafe
            {
                byte* srcPixels = (byte*)bitmapData.Scan0;
                {
                    for (int y = 0; y < bitmap.Height; y++)
                    {
                        for (int x = 0; x < bitmap.Width; x++)
                        {
                            byte* offset = srcPixels + (y * bitmapData.Stride) + (x * 4);

                            byte b = *(offset++);
                            byte g = *(offset++);
                            byte r = *(offset++);
                            byte a = *(offset++);

                            image.SetPixel(x, y, new UIColor(r, g, b, a));
                        }
                    }
                }
            }

            return image;
        }

        public void SaveAndManifest(ProcImage bitmap, string destFile)
        {
            if (imageSheetGroupName != null)
            {
                imageSheetGroupImages[destFile] = new ImageSheetEntry { Image = bitmap, NamePrefix = ImageNamePrefix };

                string tmpFile = Path.Combine(tmpDir, Path.GetFileName(destFile));

                SaveImage(bitmap, tmpFile);

                return;
            }

            int usedWidth = bitmap.ImageWidth;
            int usedHeight = bitmap.ImageHeight;

            if (PadToPowerOfTwo)
            {
                int newWidth = UpperPowerOfTwo(bitmap.ImageWidth);
                int newHeight = UpperPowerOfTwo(bitmap.ImageHeight);

                if ((newWidth != bitmap.ImageWidth) || (newHeight != bitmap.ImageHeight))
                {
                    ProcImage padded = new (newWidth, newHeight);

                    Rectangle rect = new Rectangle(0, 0, usedWidth, usedHeight);

                    padded.Draw(bitmap, rect, rect);

                    bitmap = padded;
                }
            }

            SaveImage(bitmap, destFile);
        }

        public void SaveImage(ProcImage image, string destFile)
        {
            using (Stream stream = File.OpenWrite(destFile))
            {
                StbImageWriteSharp.ImageWriter writer = new ();

                writer.WritePng(image.GetByteData().ToArray(), image.ImageWidth, image.ImageHeight, StbImageWriteSharp.ColorComponents.RedGreenBlueAlpha, stream);
            }
        }

        public void DeleteFile(string textureName)
        {
            string destFile = Path.Combine(DestPath, textureName) + ".png";

            File.Delete(destFile);
        }

        public int UpperPowerOfTwo(int v)
        {
            v--;
            v |= v >> 1;
            v |= v >> 2;
            v |= v >> 4;
            v |= v >> 8;
            v |= v >> 16;
            v++;

            return v;
        }

        public bool IsNewer(string file1, string file2)
        {
            if (!File.Exists(file2))
                return true;

            return !(new FileInfo(file1).LastWriteTime < new FileInfo(file2).LastWriteTime);
        }

    }
}
