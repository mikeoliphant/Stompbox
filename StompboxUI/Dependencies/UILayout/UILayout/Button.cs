using System;

namespace UILayout
{
    public class Button : UIElementWrapper
    {
        public Action PressAction { get; set; }
        public Action ClickAction { get; set; }
        public Action DoubleClickAction { get; set; }
        public bool IsToggleButton { get; set; }
        public bool IsPressed { get; private set; }

        UIElement pressedElement;
        UIElement unpressedElement;

        public UIElement PressedElement
        {
            get => pressedElement;
            
            set
            {
                pressedElement = value;

                if ((pressedElement != null) && !ContentBounds.IsEmpty)
                    pressedElement.SetBounds(ContentBounds, this);

                if (IsPressed)
                    Child = pressedElement;
            }
        }

        public UIElement UnpressedElement
        {
            get => unpressedElement;

            set
            {
                unpressedElement = value;

                if ((unpressedElement != null) && !ContentBounds.IsEmpty)
                    unpressedElement.SetBounds(ContentBounds, this);

                if (!IsPressed)
                    Child = unpressedElement;
            }
        }

        public void SetPressed(bool pressed)
        {
            if (this.IsPressed != pressed)
                Toggle();
        }

        public void Toggle()
        {
            IsPressed = !IsPressed;

            Child = IsPressed ? pressedElement : unpressedElement;

            UpdateContentLayout();
        }

        protected override void GetContentSize(out float width, out float height)
        {
            float pressedWidth;
            float pressedHight;

            pressedElement.GetSize(out pressedWidth, out pressedHight);

            float unpressedWidth;
            float unpressedHight;

            unpressedElement.GetSize(out unpressedWidth, out unpressedHight);

            width = Math.Max(pressedWidth, unpressedWidth);
            height = Math.Max(pressedHight, unpressedHight);
        }

        public override bool HandleTouch(in Touch touch)
        {
            switch (touch.TouchState)
            {
                case ETouchState.Pressed:
                    if (IsToggleButton)
                    {
                        Toggle();

                        if (ClickAction != null)
                            ClickAction();

                        if (PressAction != null)
                            PressAction();
                    }
                    else
                    {
                        if (!IsPressed)
                        {
                            CaptureTouch(touch);

                            Toggle();

                            if (PressAction != null)
                                PressAction();
                        }
                    }
                    break;
                case ETouchState.Released:
                    if (!IsToggleButton)
                    {
                        ReleaseTouch();

                        if (IsPressed)
                        {
                            Toggle();

                            if (ClickAction != null)
                                ClickAction();
                        }
                    }
                    break;
            }

            if ((DoubleClickAction != null) && (IsDoubleTap(touch, this)))
            {
                DoubleClickAction();
            }

            return true;
        }
    }

    public class NinePatchButton : Button
    {
        UIImage pressedNinePatchImage;
        UIImage unpressedNinePatchImage;

        public NinePatchWrapper PressedNinePatch
        {
            get { return PressedElement as NinePatchWrapper; }
        }

        public NinePatchWrapper UnpressedNinePatch
        {
            get { return UnpressedElement as NinePatchWrapper; }
        }

        public NinePatchButton()
            : this(Layout.Current.DefaultPressedNinePatch, Layout.Current.DefaultUnpressedNinePatch)
        {
        }

        public NinePatchButton(UIImage ninePatchImage)
        {
            this.pressedNinePatchImage = ninePatchImage;
            this.unpressedNinePatchImage = ninePatchImage;
        }

        public NinePatchButton(UIImage pressedNinePatchImage, UIImage unpressedNinePatchImage)
        {
            this.pressedNinePatchImage = pressedNinePatchImage;
            this.unpressedNinePatchImage = unpressedNinePatchImage;
        }

        public void SetElements(UIElement pressedElement, UIElement unpressedElement)
        {
            if (pressedNinePatchImage != null)
            {
                PressedElement = new NinePatchWrapper(pressedNinePatchImage)
                {
                    Child = pressedElement,
                    HorizontalAlignment = EHorizontalAlignment.Stretch,
                    VerticalAlignment = EVerticalAlignment.Stretch
                };

                UnpressedElement = new NinePatchWrapper(unpressedNinePatchImage)
                {
                    Child = unpressedElement,
                    HorizontalAlignment = EHorizontalAlignment.Stretch,
                    VerticalAlignment = EVerticalAlignment.Stretch
                };
            }
            else
            {
                PressedElement = new UIElementWrapper
                {
                    Child = pressedElement,
                    HorizontalAlignment = EHorizontalAlignment.Stretch,
                    VerticalAlignment = EVerticalAlignment.Stretch,
                    Padding = new LayoutPadding(2, 5),
                    BackgroundColor = UIColor.Green
                };

                UnpressedElement = new UIElementWrapper()
                {
                    Child = unpressedElement,
                    HorizontalAlignment = EHorizontalAlignment.Stretch,
                    VerticalAlignment = EVerticalAlignment.Stretch,
                    Padding = new LayoutPadding(2, 5),
                    BackgroundColor = UIColor.Red
                };
            }
        }
    }

    public class EmptyButton : NinePatchButton
    {
        public EmptyButton()
        {
            SetElements(null, null);
        }
    }

    public class TextButton : NinePatchButton
    {
        public string Text
        {
            get => textBlock.Text;
            set => textBlock.Text = value;
        }

        public UIColor TextColor
        {
            get { return textBlock.TextColor; }
            set { textBlock.TextColor = value; }
        }

        public UIFont TextFont
        {
            get { return textBlock.TextFont; }
            set { textBlock.TextFont = value; }
        }

        TextBlock textBlock;

        public TextButton(string text)
            : this()
        {
            Text = text;
        }

        public TextButton()
        {
            textBlock = new TextBlock
            {
                HorizontalAlignment = EHorizontalAlignment.Center,
                VerticalAlignment = EVerticalAlignment.Center,
            };

            SetElements(textBlock, textBlock);
        }
    }

    public class TextToggleButton : NinePatchButton
    {
        public string PressedText
        {
            get => pressedTextBlock.Text;
            set => pressedTextBlock.Text = value;
        }

        TextBlock pressedTextBlock;

        public string UnpressedText
        {
            get => unpressedTextBlock.Text;
            set => unpressedTextBlock.Text = value;
        }

        TextBlock unpressedTextBlock;

        public TextToggleButton(string pressedText, string unpressedText)
            : this()
        {
            PressedText = pressedText;
            UnpressedText = unpressedText;
        }

        public TextToggleButton()
        {
            IsToggleButton = true;

            pressedTextBlock = new TextBlock
            {
                HorizontalAlignment = EHorizontalAlignment.Center,
                VerticalAlignment = EVerticalAlignment.Center,
            };

            unpressedTextBlock = new TextBlock
            {
                HorizontalAlignment = EHorizontalAlignment.Center,
                VerticalAlignment = EVerticalAlignment.Center,
            };

            SetElements(pressedTextBlock, unpressedTextBlock);
        }
    }

    public class ImageButton : NinePatchButton
    {
        public UIColor ImageColor
        {
            get { return imageElement.Color;  }
            set { imageElement.Color = value; }
        }


        ImageElement imageElement;

        public ImageButton(string imageName)
            : this(Layout.Current.GetImage(imageName))
        {
        }

        public ImageButton(UIImage image)
        {
            imageElement = new ImageElement(image)
            {
                HorizontalAlignment = EHorizontalAlignment.Stretch,
                VerticalAlignment = EVerticalAlignment.Stretch
            };

            SetElements(imageElement, imageElement);
        }
    }

    public class ImageToggleButton : NinePatchButton
    {
        public UIColor ImageColor
        {
            get { return pressedImageElement.Color; }
            set { pressedImageElement.Color = unpressedImageElement.Color = value; }
        }

        ImageElement pressedImageElement;
        ImageElement unpressedImageElement;

        public ImageToggleButton(string pressedImageName, string unpressedImageName)
            : this(Layout.Current.GetImage(pressedImageName), Layout.Current.GetImage(unpressedImageName))
        {
        }

        public ImageToggleButton(UIImage pressedImage, UIImage unpressedImage)
        {
            pressedImageElement = new ImageElement(pressedImage);
            unpressedImageElement = new ImageElement(unpressedImage);

            SetElements(pressedImageElement, unpressedImageElement);

            IsToggleButton = true;
        }
    }
}
