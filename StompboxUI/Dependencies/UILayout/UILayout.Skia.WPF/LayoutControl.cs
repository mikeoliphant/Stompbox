using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Numerics;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using SkiaSharp;

namespace UILayout.Skia.WPF
{
    [DefaultEvent("PaintSurface")]
    [DefaultProperty("Name")]
    public class LayoutControl : FrameworkElement
    {
        private const double BitmapDpi = 96.0;

        private readonly bool designMode;

        private WriteableBitmap bitmap;
        private SKSurface surface;
        private bool ignorePixelScaling;

        float scaleX = 1.0f;
        float scaleY = 1.0f;
        bool needRePaint = false;

        public SkiaLayout Layout { get; private set; }

        public SKSize CanvasSize { get; private set; }

        public bool IgnorePixelScaling
        {
            get => ignorePixelScaling;
            set
            {
                ignorePixelScaling = value;

                UpdatePaint();
            }
        }

        public LayoutControl()
        {
            designMode = DesignerProperties.GetIsInDesignMode(this);
        }

        public void SetLayout(SkiaLayout layout)
        {
            this.Layout = layout;
        }

        protected override void OnInitialized(EventArgs e)
        {
            base.OnInitialized(e);

            CompositionTarget.Rendering += CompositionTarget_Rendering;

            MouseDown += LayoutControl_MouseDown;
            MouseUp += LayoutControl_MouseUp;
            MouseMove += LayoutControl_MouseMove;
            MouseLeave += LayoutControl_MouseLeave;
        }

        Vector2 GetPostion(in Point p)
        {
            return new Vector2((float)p.X * (IgnorePixelScaling ? 1.0f : scaleX), (float)p.Y * (IgnorePixelScaling ? 1.0f : scaleY));
        }

        private void LayoutControl_MouseLeave(object sender, System.Windows.Input.MouseEventArgs e)
        {
            if (e.LeftButton == System.Windows.Input.MouseButtonState.Pressed)
            {
                Point p = e.GetPosition(this);

                Touch touch = new Touch()
                {
                    Position = GetPostion(p),
                    TouchState = ETouchState.Invalid
                };

                Layout.HandleTouch(touch);
            }
        }

        private void LayoutControl_MouseMove(object sender, System.Windows.Input.MouseEventArgs e)
        {
            if (e.LeftButton == System.Windows.Input.MouseButtonState.Pressed)
            {
                Point p = e.GetPosition(this);

                Touch touch = new Touch()
                {
                    Position = GetPostion(p),
                    TouchState = ETouchState.Moved
                };

                Layout.HandleTouch(touch);
            }
        }

        private void LayoutControl_MouseDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            Point p = e.GetPosition(this);

            Touch touch = new Touch()
            {
                Position = GetPostion(p),
                TouchState = ETouchState.Pressed
            };

            Layout.HandleTouch(touch);
        }

        private void LayoutControl_MouseUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            Point p = e.GetPosition(this);

            Touch touch = new Touch()
            {
                Position = GetPostion(p),
                TouchState = ETouchState.Released
            };

            Layout.HandleTouch(touch);
        }

        private void CompositionTarget_Rendering(object sender, EventArgs e)
        {
            if (designMode)
                return;

            if (Layout.HaveDirty)
                UpdatePaint();
        }

        protected override void OnRender(DrawingContext drawingContext)
        {
            base.OnRender(drawingContext);

            if (designMode)
                return;

            if (Visibility != Visibility.Visible || PresentationSource.FromVisual(this) == null)
                return;

            var size = CreateSize(out var unscaledSize, out scaleX, out scaleY);
            var userVisibleSize = IgnorePixelScaling ? unscaledSize : size;

            CanvasSize = userVisibleSize;

            if (size.Width <= 0 || size.Height <= 0)
                return;

            var info = new SKImageInfo(size.Width, size.Height, SKImageInfo.PlatformColorType, SKAlphaType.Premul);

            // reset the bitmap if the size has changed
            if (bitmap == null || info.Width != bitmap.PixelWidth || info.Height != bitmap.PixelHeight)
            {
                needRePaint = true;

                bitmap = new WriteableBitmap(info.Width, size.Height, BitmapDpi * scaleX, BitmapDpi * scaleY, PixelFormats.Pbgra32, null);

                surface = SKSurface.Create(info, bitmap.BackBuffer, bitmap.BackBufferStride);

                if (Layout != null)
                {
                    Layout.SetBounds(new RectF(0, 0, info.Width, info.Height));

                    Layout.UpdateLayout();
                }
            }

            if (needRePaint)
            {
                needRePaint = false;

                bitmap.Lock();

                if (IgnorePixelScaling)
                {
                    var canvas = surface.Canvas;
                    canvas.Scale(scaleX, scaleY);
                    canvas.Save();
                }

                RectF dirtyRect = Layout.DirtyRect;
                dirtyRect.IntersectWith(Layout.Bounds);

                PaintSurface(surface, info.WithSize(userVisibleSize));

                Int32Rect bitmapDirty = new Int32Rect((int)dirtyRect.X, (int)dirtyRect.Y, (int)Math.Ceiling(dirtyRect.Width), (int)Math.Ceiling(dirtyRect.Height));

                if ((bitmapDirty.X < bitmap.Width) && (bitmapDirty.Y < bitmap.Height))
                {
                    if ((bitmapDirty.X + Width) > bitmap.Width)
                    {
                        bitmapDirty.Width = (int)(bitmap.Width - bitmapDirty.X);
                    }

                    if ((bitmapDirty.Y + Height) > bitmap.Height)
                    {
                        bitmapDirty.Height = (int)(bitmap.Height - bitmapDirty.Y);
                    }

                    bitmap.AddDirtyRect(bitmapDirty);
                }

                bitmap.Unlock();
            }

            drawingContext.DrawImage(bitmap, new Rect(0, 0, ActualWidth, ActualHeight));
        }

        protected virtual void PaintSurface(SKSurface surface, SKImageInfo info)
        {
            SKCanvas canvas = surface.Canvas;

            if (Layout == null)
            {
                canvas.Clear(SKColors.White);
            }
            else
            {
                Layout.GraphicsContext.Canvas = canvas;

                if (!Layout.Bounds.IsEmpty)
                    Layout.Draw();
            }
        }

        public void UpdatePaint()
        {
            InvalidateVisual();

            needRePaint = true;
        }

        public void RePaint()
        {
            UpdatePaint();
        }

        private SKSizeI CreateSize(out SKSizeI unscaledSize, out float scaleX, out float scaleY)
        {
            unscaledSize = SKSizeI.Empty;
            scaleX = 1.0f;
            scaleY = 1.0f;

            var w = ActualWidth;
            var h = ActualHeight;

            if (!IsPositive(w) || !IsPositive(h))
                return SKSizeI.Empty;

            unscaledSize = new SKSizeI((int)w, (int)h);

            var m = PresentationSource.FromVisual(this).CompositionTarget.TransformToDevice;
            scaleX = (float)m.M11;
            scaleY = (float)m.M22;
            return new SKSizeI((int)(w * scaleX), (int)(h * scaleY));

            bool IsPositive(double value)
            {
                return !double.IsNaN(value) && !double.IsInfinity(value) && value > 0;
            }
        }
    }
}