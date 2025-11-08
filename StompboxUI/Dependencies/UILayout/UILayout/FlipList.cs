using System;
using System.Collections.Generic;
using System.Numerics;

namespace UILayout
{
    public class FlipList : ListUIElement
    {
        public int LeftmostElement
        {
            get
            {
                return (int)(offset / childWidth);
            }
        }

        float childWidth;
        float offset = 0;
        float desiredOffset = 0;
        float captureStartOffset;

        public FlipList(float childWidth)
        {
            this.childWidth = childWidth;
        }

        protected override void DrawContents()
        {
            if (desiredOffset != offset)
            {
                if (Math.Abs(desiredOffset - offset) < 1)
                {
                    offset = desiredOffset;
                }
                else
                {
                    offset = MathUtil.Lerp(offset, desiredOffset, 0.4f);
                }

                UpdateContentLayout();
            }
            else
            {
                SetOffset(offset);

                desiredOffset = offset;
            }

            foreach (UIElement child in children)
            {
                if (child.ContentBounds.Intersects(ContentBounds))
                    child.Draw();
            }
        }

        public override void UpdateContentLayout()
        {
            base.UpdateContentLayout();

            float xOffset = -offset;

            foreach (UIElement child in children)
            {
                float actualOffset = xOffset;

                if (actualOffset < -childWidth)
                {
                    actualOffset += (children.Count) * childWidth;
                }

                child.SetBounds(new RectF(actualOffset, ContentBounds.Y, childWidth, ContentBounds.Height), this);

                xOffset += childWidth;
            }
        }

        public void SetOffset(float newOffset)
        {
            offset = newOffset;

            if (offset >= (childWidth * children.Count))
            {
                offset -= (childWidth * children.Count);
            }
            else if (newOffset < 0)
            {
                offset += (childWidth * Children.Count);
            }

            UpdateContentLayout();
        }

        public void SetDesiredOffset(float newOffset)
        {
            desiredOffset = newOffset;
        }

        bool CanMove()
        {
            return (childWidth * Children.Count) > ContentBounds.Width;
        }

        public void SwipeLeft()
        {
            if (offset == 0)
            {
                offset = childWidth * children.Count;
            }

            float remainder = offset % childWidth;

            if (remainder == 0)
            {
                remainder = childWidth;
            }

            SetDesiredOffset(offset - remainder);
        }

        public void SwipeRight()
        {
            float remainder = offset % childWidth;

            SetDesiredOffset(offset + (childWidth - remainder));
        }

        float totDrag = 0;
        float lastDragX = 0;

        public override bool HandleTouch(in Touch touch)
        {
            if (CanMove())
            {
                switch (touch.TouchState)
                {
                    case ETouchState.Pressed:
                        CaptureTouch(touch);

                        captureStartOffset = offset;
                        lastDragX = touch.Position.X;
                        totDrag = 0;
                        break;

                    case ETouchState.Moved:
                        float delta = touch.Position.X - TouchCaptureStartPosition.X;

                        totDrag += Math.Abs(touch.Position.X - lastDragX);
                        lastDragX = touch.Position.X;

                        SetOffset(captureStartOffset - delta);
                        desiredOffset = offset;
                        break;

                    case ETouchState.Released:
                        ReleaseTouch();

                        if (totDrag <= 5)
                        {
                            base.HandleTouch(new Touch()
                            {
                                Position = touch.Position,
                                TouchID = touch.TouchID,
                                TouchState = ETouchState.Pressed
                            });

                            base.HandleTouch(touch);
                        }
                        
                        float remainder = offset % childWidth;

                        if (remainder > (childWidth / 2))
                        {
                            SetDesiredOffset(offset + (childWidth - remainder));
                        }
                        else
                        {
                            SetDesiredOffset(offset - remainder);
                        }

                        break;
                }
            }

            return false;
        }
    }
}
