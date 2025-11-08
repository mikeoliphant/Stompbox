using System;
using System.Collections.Generic;
using System.Numerics;
using System.Text;

namespace UILayout
{
    public interface IScrollable
    {
        void ScrollBackward();
        void ScrollForward();
        void ScrollPageBackward();
        void ScrollPageForward();
        void SetScrollPercent(float scrollPercent);
    }

    public class VerticalScrollBarWithArrows : VerticalStack
    {
        public VerticalScrollBar ScrollBar { get; private set; }

        public VerticalScrollBarWithArrows()
        {
            VerticalAlignment = EVerticalAlignment.Stretch;

            NinePatchWrapper gutter = new NinePatchWrapper(Layout.Current.GetImage("ScrollBarGutter"))
            {
                Padding = new LayoutPadding(0),
                HorizontalAlignment = EHorizontalAlignment.Stretch,
                VerticalAlignment = EVerticalAlignment.Stretch
            };

            ScrollBar = new VerticalScrollBar()
            {
                HorizontalAlignment = EHorizontalAlignment.Stretch,
                VerticalAlignment = EVerticalAlignment.Stretch,
                Padding = new LayoutPadding(gutter.Image.Width / 2)
            };


            Children.Add(new ImageButton("ScrollUpArrow")
            {
                HorizontalAlignment = EHorizontalAlignment.Stretch,
                DesiredHeight = 20,
                ClickAction = ScrollBar.ScrollBackward
            });

            Children.Add(gutter);

            gutter.Child = ScrollBar;

            Children.Add(new ImageButton("ScrollDownArrow")
            {
                HorizontalAlignment = EHorizontalAlignment.Stretch,
                DesiredHeight = 20,
                ClickAction = ScrollBar.ScrollForward
            });
        }

        public override void UpdateContentLayout()
        {
            base.UpdateContentLayout();
        }
    }

    public class VerticalScrollBar : UIElementWrapper
    {
        public IScrollable Scrollable { get; set; }

        float visiblePercent = 1.0f;
        bool inDrag;
        int touchID;
        Vector2 dragStart;
        float scrollPercent = 0.0f;
        float startY;

        NinePatchWrapper bar;

        public VerticalScrollBar()
        {
            Child = bar = new NinePatchWrapper(Layout.Current.GetImage("ScrollBar"))
            {
                Padding = new LayoutPadding(0),
                HorizontalAlignment = EHorizontalAlignment.Stretch,
                VerticalAlignment = EVerticalAlignment.Absolute                             
            };
        }

        public override void SetBounds(in RectF bounds, UIElement parent)
        {
            base.SetBounds(bounds, parent);
        }

        public void ScrollBackward()
        {
            if (Scrollable != null)
                Scrollable.ScrollBackward();
        }

        public void ScrollForward()
        {
            if (Scrollable != null)
                Scrollable.ScrollForward();
        }

        public void SetVisiblePercent(float visiblePercent)
        {
            this.visiblePercent = visiblePercent;

            UpdateContentLayout();
        }

        public void SetScrollPercent(float scrollPercent)
        {
            this.scrollPercent = scrollPercent;

            UpdateContentLayout();
        }

        public override void UpdateContentLayout()
        {
            if (visiblePercent >= 1.0f)
            {
                bar.DesiredHeight = ContentBounds.Height;

                bar.Margin = new LayoutPadding(0);
            }
            else
            {
                bar.DesiredHeight = visiblePercent * ContentBounds.Height;

                bar.Margin = new LayoutPadding(0, (int)(ContentBounds.Height * scrollPercent));
            }

            base.UpdateContentLayout();
        }

        public override bool HandleTouch(in Touch touch)
        {
            switch (touch.TouchState)
            {
                case ETouchState.Pressed:
                    if (touch.Position.Y < bar.ContentBounds.Top)
                    {
                        if (Scrollable != null)
                            Scrollable.ScrollPageBackward();
                    }
                    else if (touch.Position.Y > bar.ContentBounds.Bottom)
                    {
                        if (Scrollable != null)
                            Scrollable.ScrollPageForward();
                    }
                    else
                    {
                        inDrag = true;
                        touchID = touch.TouchID;

                        dragStart = touch.Position;
                        startY = bar.ContentBounds.Top;

                        CaptureTouch(touch);
                    }
                    break;
                case ETouchState.Moved:
                    if (inDrag)
                    {
                        float deltaY = touch.Position.Y - dragStart.Y;

                        float yOffset = startY + deltaY;

                        if (yOffset < ContentBounds.Top)
                        {
                            yOffset = ContentBounds.Top;
                        }
                        else if ((yOffset + bar.DesiredHeight) > ContentBounds.Bottom)
                        {
                            yOffset = ContentBounds.Bottom - bar.DesiredHeight;
                        }

                        scrollPercent = (yOffset - ContentBounds.Top) / ContentBounds.Height;

                        if (Scrollable != null)
                        {
                            Scrollable.SetScrollPercent(scrollPercent);
                        }

                        UpdateContentLayout();
                    }
                    break;
                case ETouchState.Released:
                case ETouchState.Invalid:
                    if (touch.TouchID == touchID)
                    {
                        inDrag = false;
                        ReleaseTouch(touchID);

                        UpdateContentLayout();
                    }
                    break;
            }

            return true;
        }
    }
}
