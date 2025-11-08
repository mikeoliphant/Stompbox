using System;
using System.Numerics;

namespace UILayout
{
    public class Slider : Dock
    {
        public Action<float> ChangeAction { get; set; }
        public bool InvertLevel { get; set; }

        public float Level { get; protected set; }
        protected UIImage levelImage;
        protected float captureStartLevel;

        bool isHorizontal;
        protected ImageElement levelImageElement;

        public Slider(string imageName, bool isHorizontal)
        {
            this.isHorizontal = isHorizontal;

            HorizontalAlignment = EHorizontalAlignment.Left;
            VerticalAlignment = EVerticalAlignment.Top;

            levelImage = Layout.Current.GetImage(imageName);

            Children.Add(levelImageElement = new ImageElement(levelImage)
            {
                HorizontalAlignment = isHorizontal ? EHorizontalAlignment.Left : EHorizontalAlignment.Left,
                VerticalAlignment = isHorizontal ? EVerticalAlignment.Center : EVerticalAlignment.Top,
            });
        }

        public override void UpdateContentLayout()
        {
            if (isHorizontal)
            {
                levelImageElement.Padding = new LayoutPadding((ContentBounds.Width * Level) - (float)Math.Ceiling((levelImage.Width) / 2f), 0);
            }
            else
            {
                levelImageElement.Padding = new LayoutPadding(0, (ContentBounds.Height * Level) - (float)Math.Ceiling((levelImage.Height) / 2f));
            }

            base.UpdateContentLayout();
        }

        public void SetLevel(float level)
        {
            UpdateLevel(InvertLevel ? (1.0f - level) : level, sendChange: false);
        }

        void UpdateLevel(float level, bool sendChange)
        {
            level = MathUtil.Saturate(level);
            this.Level = level;

            if (sendChange && (ChangeAction != null))
                ChangeAction(InvertLevel ? (1.0f - level) : level);

            UpdateContentLayout();
        }

        public override bool HandleTouch(in Touch touch)
        {
            switch (touch.TouchState)
            {
                case ETouchState.Pressed:
                    captureStartLevel = Level;
                    CaptureTouch(touch);
                    break;
                case ETouchState.Moved:
                    float delta = isHorizontal ? ((touch.Position.X - TouchCaptureStartPosition.X) / ContentBounds.Width) : ((touch.Position.Y - TouchCaptureStartPosition.Y) / ContentBounds.Height);

                    UpdateLevel(captureStartLevel + delta, sendChange: true);
                    break;
                case ETouchState.Released:
                    ReleaseTouch();
                    break;
            }

            return base.HandleTouch(touch);
        }

        //public override bool HandleGesture(PixGesture gesture)
        //{
        //    if (haveCapture)
        //    {
        //        if (gesture.GestureType == EPixGestureType.Drag)
        //        {
        //            float delta = ((isHorizontal ? gesture.Position.X : gesture.Position.Y) - startOffset) / (isHorizontal ? ContentLayout.Width : ContentLayout.Height);

        //            UpdateLevel(startLevel + delta, sendChange: true);

        //            return true;
        //        }
        //    }

        //    //if (gesture.GestureType == EPixGestureType.Tap)
        //    //{
        //    //    float delta = (isHorizontal ? (gesture.Position.X - ContentLayout.Offset.X) : (gesture.Position.Y - ContentLayout.Offset.Y)) /
        //    //        (isHorizontal ? ContentLayout.Width : ContentLayout.Height);

        //    //    UpdateLevel(delta, sendChange: true);
        //    //}

        //    return base.HandleGesture(gesture);
        //}
    }

    public class VerticalPointer : Slider
    {
        public VerticalPointer()
            : base("VerticalPointer", isHorizontal: false)
        {
        }

        protected override void GetContentSize(out float width, out float height)
        {
            base.GetContentSize(out width, out height);

            height = 0;
        }


        public override void SetBounds(in RectF layout, UIElement parent)
        {
            base.SetBounds(layout, parent);

            //InputLayout = new UILayout(new Vector2(layout.Offset.X - (levelImage.Width * PixUI.DefaultScale), layout.Offset.Y), layout.Width + (levelImage.Width * PixUI.DefaultScale), layout.Height);
        }
    }

    public class HorizontalSlider : Slider
    {
        public HorizontalSlider(string sliderImageName)
            : base(sliderImageName, isHorizontal: true)
        {
            Padding = new LayoutPadding(levelImage.Width / 2f, 0);

            Children.Insert(0, new UIElement
            {
                HorizontalAlignment = EHorizontalAlignment.Stretch,
                VerticalAlignment = EVerticalAlignment.Center,
                BackgroundColor = UIColor.Black,
                DesiredHeight = levelImage.Height * 0.25f
            });
        }

        public override void SetBounds(in RectF layout, UIElement parent)
        {
            base.SetBounds(layout, parent);

            //InputLayout = new UILayout(new Vector2(ContentLayout.Offset.X - ((levelImage.Width * PixUI.DefaultScale) / 2f), ContentLayout.Offset.Y),
            //    ContentLayout.Width + (levelImage.Width * PixUI.DefaultScale), ContentLayout.Height);
        }
    }

    public class VerticalSlider : Slider
    {
        public VerticalSlider(string sliderImageName)
            : base(sliderImageName, isHorizontal: false)
        {
            Padding = new LayoutPadding(0, levelImage.Height / 2f);

            levelImageElement.HorizontalAlignment = EHorizontalAlignment.Center;

            Children.Insert(0, new UIElement
            {
                HorizontalAlignment = EHorizontalAlignment.Center,
                VerticalAlignment = EVerticalAlignment.Stretch,
                BackgroundColor = UIColor.Black,
                DesiredWidth = levelImage.Width * 0.25f
            });
        }

        public override void SetBounds(in RectF layout, UIElement parent)
        {
            base.SetBounds(layout, parent);

            //InputLayout = new UILayout(new Vector2(ContentLayout.Offset.X, ContentLayout.Offset.Y - ((levelImage.Height * PixUI.DefaultScale) / 2f)),
            //    ContentLayout.Width, ContentLayout.Height + (levelImage.Height * PixUI.DefaultScale));
        }
    }

}
