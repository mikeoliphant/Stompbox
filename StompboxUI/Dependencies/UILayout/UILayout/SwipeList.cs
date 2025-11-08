using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;

namespace UILayout
{
    public class SwipeList : UIElement, IScrollable
    {
        public UIFont Font
        {
            get => font;

            set
            {
                font = value;

                UpdateItemHeight();
            }
        }

        public float FontScale
        {
            get => fontScale;
            set
            {
                fontScale = value;

                UpdateItemHeight();
            }
        }

        public UIColor TextColor { get; set; } = UIColor.White;
        public UIColor HighlightColor { get; set; } = new UIColor(200, 200, 200, 255);
        public Action<int> SelectAction { get; set; }
        public Action<int> HoldAction { get; set; }
        public float ItemHeight { get; set; }
        public float ItemXOffset { get; set; }
        public float ItemYOffset { get; set; }
        public int LastSelectedItem { get; set; } = -1;

        public int CurrentTopItemIndex
        {
            get
            {
                return (int)(offset / ItemHeight);
            }
        }

        public int NumDisplayedItems
        {
            get
            {
                return (int)(ContentBounds.Height / ItemHeight);
            }
        }

        IList items;

        UIFont font = Layout.Current.DefaultFont;
        float fontScale = 1.0f;
        float offset = 0;
        float velocity;
        protected StringBuilder sb = new StringBuilder();
        bool haveTouch = false;
        int touchItem;
        int firstVisibleItem;
        int lastVisibleItem;
        VerticalScrollBar scrollBar;

        public int ItemCount
        {
            get
            {
                if (items != null)
                {
                    return items.Count;
                }

                return 0;
            }
        }

        public IList Items
        {
            get { return items; }
            set
            {
                items = value;
                SetOffset(0);

                if (scrollBar != null)
                {
                    scrollBar.SetVisiblePercent((ContentBounds.Height / ItemHeight) / (float)ItemCount);
                }
            }
        }

        public SwipeList()
        {
            TextColor = UIColor.White;

            ItemXOffset = 10;

            UpdateItemHeight();
        }

        public void SetScrollBar(VerticalScrollBar scrollBar)
        {
            this.scrollBar = scrollBar;

            if (items != null)
            {
                scrollBar.SetVisiblePercent((ContentBounds.Height / ItemHeight) / (float)ItemCount);
            }
            else
            {
                scrollBar.SetVisiblePercent(1.0f);
            }

            scrollBar.Scrollable = this;
        }

        public void ScrollPageBackward()
        {
            PreviousPage();
        }

        public void ScrollPageForward()
        {
            NextPage();
        }

        public void SetScrollPercent(float scrollPercent)
        {
            if (Items != null)
            {
                SetOffset((float)ItemCount * scrollPercent * ItemHeight);
            }
        }

        void SetOffset(float newOffset)
        {
            this.offset = newOffset;

            if (offset < 0)
                offset = 0;

            float maxOffset = (ItemCount * ItemHeight) - ContentBounds.Height;

            if (maxOffset < 0)
            {
                maxOffset = 0;
            }

            if (offset > maxOffset)
            {
                offset = maxOffset;
            }

            if (scrollBar != null)
            {
                scrollBar.SetScrollPercent((offset / ItemHeight) / (float)ItemCount);
            }

            UpdateContentLayout();
        }

        public void SetTopItem(int topItem)
        {
            SetOffset(topItem * ItemHeight);
        }

        void UpdateItemHeight()
        {
            ItemHeight = (Font.TextHeight * FontScale) * 1.1f;
            ItemYOffset = (ItemHeight - (Font.TextHeight * FontScale)) / 2;
        }

        public override void UpdateContentLayout()
        {
            base.UpdateContentLayout();

            if (scrollBar != null)
            {
                scrollBar.SetVisiblePercent((ContentBounds.Height / ItemHeight) / (float)ItemCount);
            }
        }

        protected override void DrawContents()
        {
            base.DrawContents();

            firstVisibleItem = -1;
            lastVisibleItem = -1;

            if (items == null)
                return;

            int item = (int)Math.Floor(offset / ItemHeight);
            float itemOffset = (item * ItemHeight) - offset;

            float y = ContentBounds.Y;

            while ((y + itemOffset) < ContentBounds.Bottom)
            {
                if (item > (ItemCount - 1))
                    break;

                if (item >= 0)
                {
                    if (firstVisibleItem == -1)
                        firstVisibleItem = item;

                    lastVisibleItem = item;

                    DrawItem(item, y + itemOffset);
                }

                y += ItemHeight;
                item++;
            }

            haveTouch = false;
        }

        protected virtual void DrawItem(int item, float y)
        {
            if (haveTouch && (item == touchItem))
            {
                Layout.Current.GraphicsContext.DrawRectangle(new RectF(ContentBounds.X, y, ContentBounds.Width, ItemHeight), HighlightColor);
            }

            if (item == LastSelectedItem)
            {
                Layout.Current.GraphicsContext.DrawRectangle(new RectF(ContentBounds.X, y, ContentBounds.Width, ItemHeight), HighlightColor);
            }

            DrawItemContents(item, ContentBounds.X + ItemXOffset, y + ItemYOffset);
        }

        public virtual void GetItemText(int item, StringBuilder stringBuilder)
        {
            sb.Append(items[item].ToString());
        }

        public virtual void DrawItemContents(int item, float x, float y)
        {
            sb.Remove(0, sb.Length);

            GetItemText(item, sb);

            Layout.Current.GraphicsContext.DrawText(sb, Font, (int)x, (int)y, TextColor, FontScale);
        }

        float dragStartY = 0;
        float lastDragY = 0;
        float dragStartOffset = 0;

        public override bool HandleTouch(in Touch touch)
        {
            int itemPos = (int)Math.Floor(((touch.Position.Y - ContentBounds.Y) + offset) / ItemHeight);

            if ((touch.TouchState == ETouchState.Pressed) || (touch.TouchState == ETouchState.Moved) || (touch.TouchState == ETouchState.Held))
            {
                haveTouch = true;

                touchItem = itemPos;
            }

            if (touch.TouchState == ETouchState.Pressed)
            {
                dragStartY = lastDragY = touch.Position.Y;
                dragStartOffset = offset;
            }

            if (touch.TouchState == ETouchState.Moved)
            {
                float diff = touch.Position.Y - dragStartY;

                SetOffset(dragStartOffset - diff);

                lastDragY = touch.Position.Y;

                UpdateContentLayout();
            }

            if (IsTap(touch, this))
            {
                if (itemPos < ItemCount)
                {
                    SelectItem(itemPos);
                }
            }

            return true;
        }

        public override void HandleInput(InputManager inputManager)
        {
            base.HandleInput(inputManager);

            int delta = inputManager.MouseWheelDelta;

            if (delta > 0)
            {
                ScrollBackward();
            }
            else if (delta < 0)
            {
                ScrollForward();
            }
        }

        public void SelectItem(int itemPos)
        {
            LastSelectedItem = itemPos;

            if (SelectAction != null)
                SelectAction(itemPos);

            EnsureSelectedItemIsVisible();
        }

        public void NextItem()
        {
            if (Items != null)
            {
                if (LastSelectedItem == -1)
                {
                    ScrollForward();
                }
                else
                {
                    if (LastSelectedItem < (ItemCount - 1))
                    {
                        SelectItem(LastSelectedItem + 1);
                    }
                }
            }
        }

        public void PreviousItem()
        {
            if (Items != null)
            {
                if (LastSelectedItem == -1)
                {
                    ScrollBackward();
                }
                else
                {
                    if (LastSelectedItem > 0)
                    {
                        SelectItem(LastSelectedItem - 1);
                    }
                }
            }
        }

        public void ScrollBackward()
        {
            offset -= (ItemHeight * 0.9f);

            EnforceEvenItemBounds();
        }

        public void ScrollForward()
        {
            offset += (ItemHeight * 1.1f);

            EnforceEvenItemBounds();
        }

        public void NextPage()
        {
            if (Items != null)
            {
                offset += ContentBounds.Height;

                EnforceEvenItemBounds();
            }
        }

        public void PreviousPage()
        {
            if (items != null)
            {
                offset -= ContentBounds.Height;

                EnforceEvenItemBounds();
            }
        }

        public void GoToFirstItem()
        {
            SetOffset(0);
        }

        public void GoToLastItem()
        {
            if (Items != null)
            {
                SetOffset((ItemCount * (ItemHeight + 1)) - ContentBounds.Height);

                EnforceEvenItemBounds();
            }
        }

        void EnsureSelectedItemIsVisible()
        {
            if (LastSelectedItem != -1)
            {
                float itemOffset = LastSelectedItem * ItemHeight;

                if (offset > itemOffset)
                    SetTopItem(LastSelectedItem);
                else
                {
                    float diff = itemOffset + ItemHeight - (offset + ContentBounds.Height);

                    if (diff > 0)
                    {
                        SetOffset(offset + diff);
                    }
                }
            }
        }

        void EnforceEvenItemBounds()
        {
            if (offset < 0)
            {
                SetOffset(0);
            }
            else
            {
                SetOffset((int)(offset / ItemHeight) * ItemHeight);

                int itemsDisplayed = (int)(ContentBounds.Height / ItemHeight);

                if ((int)(offset / ItemHeight) > (ItemCount - itemsDisplayed))
                {
                    SetOffset((ItemCount - itemsDisplayed) * ItemHeight);
                }
            }
        }
    }
}
