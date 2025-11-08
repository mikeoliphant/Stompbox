using System;
using System.Collections.Generic;
using System.Numerics;

namespace UILayout
{
    public class LayoutElement : UIElement
    {
    }

    public class UIElementWrapper : LayoutElement
    {
        public virtual UIElement Child { get; set; }

        protected override void GetContentSize(out float width, out float height)
        {
            if (Child != null)
                Child.GetSize(out width, out height);
            else
            {
                width = 0;
                height = 0;
            }
        }

        public override void UpdateContentLayout()
        {
            base.UpdateContentLayout();

            if (Child != null)
                Child.SetBounds(ContentBounds, this);
        }

        protected override void DrawContents()
        {
            base.DrawContents();

            if (Child != null)
                Child.Draw();
        }

        public override bool HandleTextInput(char c)
        {
            if (Child != null)
                return Child.HandleTextInput(c);

            return false;
        }

        public override bool HandleTouch(in Touch touch)
        {
            if (Child != null)
                return Child.HandleTouch(touch);

            return false;
        }

        public override void HandleInput(InputManager inputManager)
        {
            if (Child != null)
                Child.HandleInput(inputManager);
        }

        public override UIElement AcceptsDrop(UIElement dragElement, object dropObject, in Touch touch)
        {
            if (Child != null)
            {
                UIElement dropElement = Child.AcceptsDrop(dragElement, dropObject, touch);

                if (dropElement != null)
                    return dropElement;
            }

            return base.AcceptsDrop(dragElement, dropObject, touch);
        }
    }

    public class ListUIElement : LayoutElement
    {
        protected List<UIElement> children = new List<UIElement>();

        public bool DrawInReverse { get; set; }
        public float ChildSpacing { get; set; }
        public bool ChildrenEqualSize { get; set; }

        public virtual List<UIElement> Children
        {
            get { return children; }
            set { children = value; }
        }

        protected override void DrawContents()
        {
            if (!Visible)
                return;

            base.DrawContents();

            if (DrawInReverse)
            {
                for (int i = children.Count - 1; i >= 0; i--)
                {
                    (children[i] as UIElement).Draw();
                }
            }
            else
            {
                foreach (UIElement child in children)
                {
                    child.Draw();
                }
            }
        }

        public UIElement FindChildContainingPoint(float x, float y)
        {
            for (int i = Children.Count - 1; i >= 0; i--)
            {
                UIElement child = Children[i] as UIElement;

                if (child.Visible && child.ContentBounds.Contains(x, y))
                {
                    return child;
                }
            }

            return null;
        }

        public UIElement FindClosestChild(in Vector2 point)
        {
            float minDist = float.MaxValue;
            UIElement minChild = null;

            for (int i = Children.Count - 1; i >= 0; i--)
            {
                UIElement child = Children[i] as UIElement;

                if (child.Visible)
                {
                    float dist = Vector2.Distance(point, child.ContentBounds.Center);

                    if (dist < minDist)
                    {
                        minDist = dist;

                        minChild = child;
                    }
                }
            }

            return minChild;
        }

        public override bool HandleTouch(in Touch touch)
        {
            for (int i = Children.Count - 1; i >= 0; i--)
            {
                UIElement child = Children[i] as UIElement;

                if (child.Visible && child.LayoutBounds.Contains(touch.Position))
                {
                    if (child.HandleTouch(touch))
                        return true;
                }
            }

            return base.HandleTouch(touch);
        }

        public override bool HandleTextInput(char c)
        {
            for (int i = Children.Count - 1; i >= 0; i--)
            {
                UIElement child = Children[i] as UIElement;

                if (child.Visible)
                {
                    if (child.HandleTextInput(c))
                        return true;
                }
            }

            return base.HandleTextInput(c);
        }

        public override void HandleInput(InputManager inputManager)
        {
            base.HandleInput(inputManager);

            for (int i = Children.Count - 1; i >= 0; i--)
            {
                (Children[i] as UIElement).HandleInput(inputManager);
            }
        }

        public override UIElement AcceptsDrop(UIElement dragElement, object dropObject, in Touch touch)
        {
            for (int i = Children.Count - 1; i >= 0; i--)
            {
                UIElement child = Children[i] as UIElement;

                if (child.Visible && child.LayoutBounds.Contains(touch.Position))
                {
                    UIElement dropElement = child.AcceptsDrop(dragElement, dropObject, touch);

                    if (dropElement != null)
                        return dropElement;
                }
            }

            return base.AcceptsDrop(dragElement, dropObject, touch);
        }
    }

    public class VerticalStack : ListUIElement
    {
        protected override void GetContentSize(out float width, out float height)
        {
            width = 0;
            height = 0;

            for (int i = 0; i < children.Count; i++)
            {
                UIElement child = children[i] as UIElement;

                if (!child.Visible)
                    continue;

                float childWidth;
                float childHeight;

                child.GetSize(out childWidth, out childHeight);

                if (childWidth > width)
                    width = childWidth;

                height += childHeight;

                if (i < (children.Count - 1))
                {
                    height += ChildSpacing;
                }
            }
        }

        public override void UpdateContentLayout()
        {
            base.UpdateContentLayout();

            float neededHeight = 0;

            int greedyChildCount = 0;

            for (int i = 0; i < children.Count; i++)
            {
                UIElement child = children[i] as UIElement;

                if (!child.Visible)
                    continue;

                float childWidth;
                float childHeight;

                child.GetSize(out childWidth, out childHeight);

                neededHeight += childHeight;

                if (i < (children.Count - 1))
                {
                    neededHeight += ChildSpacing;
                }

                if (ChildrenEqualSize || (child.VerticalAlignment == EVerticalAlignment.Stretch))
                    greedyChildCount++;
            }

            float extraHeight = (ContentBounds.Height - neededHeight) / greedyChildCount;

            float yOffset = 0;

            for (int i = 0; i < children.Count; i++)
            {
                UIElement child = children[i] as UIElement;

                if (!child.Visible)
                    continue;

                if (ChildrenEqualSize)
                {
                    float height = ContentBounds.Height / greedyChildCount;

                    child.SetBounds(new RectF(ContentBounds.Left, ContentBounds.Top + yOffset, ContentBounds.Width, height), this);

                    yOffset += height;
                }
                else
                {
                    float childWidth;
                    float childHeight;

                    child.GetSize(out childWidth, out childHeight);

                    if (child.VerticalAlignment == EVerticalAlignment.Stretch)
                    {
                        child.SetBounds(new RectF(ContentBounds.Left, ContentBounds.Top + yOffset, ContentBounds.Width, childHeight + extraHeight), this);

                        yOffset += childHeight + extraHeight;
                    }
                    else
                    {
                        child.SetBounds(new RectF(ContentBounds.Left, ContentBounds.Top + yOffset, ContentBounds.Width, childHeight), this);

                        yOffset += childHeight;
                    }

                    if (i < (children.Count - 1))
                    {
                        yOffset += ChildSpacing;
                    }
                }
            }
        }
    }

    public class HorizontalStack : ListUIElement
    {
        protected override void GetContentSize(out float width, out float height)
        {
            width = 0;
            height = 0;

            for (int i = 0; i < children.Count; i++)
            {
                UIElement child = children[i] as UIElement;

                if (!child.Visible)
                    continue;

                float childWidth;
                float childHeight;

                child.GetSize(out childWidth, out childHeight);

                if (childHeight > height)
                    height = childHeight;

                width += childWidth;

                if (i < (children.Count - 1))
                {
                    width += ChildSpacing;
                }
            }
        }

        public override void UpdateContentLayout()
        {
            base.UpdateContentLayout();
           
            float neededWidth = 0;

            int greedyChildCount = 0;

            for (int i = 0; i < children.Count; i++)
            {
                UIElement child = children[i] as UIElement;

                if (!child.Visible)
                    continue;

                float childWidth;
                float childHeight;

                child.GetSize(out childWidth, out childHeight);

                neededWidth += childWidth;

                if (i < (children.Count - 1))
                {
                    neededWidth += ChildSpacing;
                }

                if (ChildrenEqualSize || (child.HorizontalAlignment == EHorizontalAlignment.Stretch))
                    greedyChildCount++;
            }

            float extraWidth = (ContentBounds.Width - neededWidth) / greedyChildCount;

            float xOffset = 0;

            for (int i = 0; i < children.Count; i++)
            {
                UIElement child = children[i] as UIElement;

                if (!child.Visible)
                    continue;

                if (ChildrenEqualSize)
                {
                    float width = ContentBounds.Width / greedyChildCount;

                    child.SetBounds(new RectF(ContentBounds.Left + xOffset, ContentBounds.Top, width, ContentBounds.Height), this);

                    xOffset += width;
                }
                else
                {
                    float childWidth;
                    float childHeight;

                    child.GetSize(out childWidth, out childHeight);

                    if (child.HorizontalAlignment == EHorizontalAlignment.Stretch)
                    {
                        child.SetBounds(new RectF(ContentBounds.Left + xOffset, ContentBounds.Top, childWidth + extraWidth, ContentBounds.Height), this);

                        xOffset += childWidth + extraWidth;
                    }
                    else
                    {
                        child.SetBounds(new RectF(ContentBounds.Left + xOffset, ContentBounds.Top, childWidth, ContentBounds.Height), this);

                        xOffset += childWidth;
                    }
                }

                if (i < (children.Count - 1))
                {
                    xOffset += ChildSpacing;
                }
            }
        }
    }

    public class Dock : ListUIElement
    {
        public Dock()
        {
            HorizontalAlignment = EHorizontalAlignment.Stretch;
            VerticalAlignment = EVerticalAlignment.Stretch;
        }

        protected override void GetContentSize(out float width, out float height)
        {
            width = 0;
            height = 0;

            foreach (UIElement child in children)
            {
                float childWidth;
                float childHeight;

                child.GetSize(out childWidth, out childHeight);

                if (childHeight > height)
                    height = childHeight;

                if (childWidth > width)
                    width = childWidth;
            }
        }

        public override void UpdateContentLayout()
        {
            base.UpdateContentLayout();

            foreach (UIElement child in children)
            {
                if (child.Visible)
                {
                    child.SetBounds(ContentBounds, this);
                }
            }
        }
    }

    public class DragDropHandler
    {
        public bool InternalOnly { get; set; }
        public Type DragType { get; set; }
        public Action<object> DragCompleteAction { get; set; }

        public virtual bool HandleTouch(in Touch touch)
        {
            return false;
        }

        public virtual UIElement AcceptsDrop(UIElement dragElement, object dropObject, in Touch touch)
        {
            return null;
        }

        public virtual bool HandleDrop(UIElement dragElement, object dropObject, in Touch touch)
        {
            return false;
        }

        public virtual void HandleDragCancelled(object dropObject)
        {
        }

        public virtual void HandleDragCompleted(object dropObject)
        {
            if (DragCompleteAction != null)
                DragCompleteAction(dropObject);
        }
    }

    public class ListUIDragDropHandler : DragDropHandler
    {
        public ListUIElement ListElement { get; set; }

        public ListUIDragDropHandler()
        {
            InternalOnly = true;
        }

        Vector2 startPosition;
        UIElement dragElement;

        public override bool HandleTouch(in Touch touch)
        {
            switch (touch.TouchState)
            {
                case ETouchState.Pressed:
                    ListElement.CaptureTouch(touch);
                    dragElement = ListElement.FindClosestChild(touch.Position);
                    startPosition = touch.Position;
                    break;
                case ETouchState.Moved:
                    if (ListElement.HaveTouchCapture && (dragElement != null))
                    {
                        float delta = Vector2.Distance(startPosition, touch.Position);

                        if (delta > 5)
                        {
                            ListElement.BeginDrag(touch.TouchID, dragElement);
                        }
                    }
                    break;
                case ETouchState.Released:
                case ETouchState.Invalid:
                    ListElement.ReleaseTouch();
                    break;
            }

            return false;
        }

        public override UIElement AcceptsDrop(UIElement dragElement, object dropObject, in Touch touch)
        {
            if (InternalOnly)
            {
                if (dragElement != ListElement)
                    return null;

                if (ListElement.Children.Contains(dropObject as UIElement))
                {
                    return ListElement;
                }
            }
            else
            {
                if ((DragType != null) && (DragType.IsAssignableFrom(dropObject.GetType())))
                    return ListElement;
            }

            return null;
        }

        public override bool HandleDrop(UIElement dragElement, object dropObject, in Touch touch)
        {
            UIElement dropOnChild = ListElement.FindClosestChild(touch.Position);

            if (dropObject == dropOnChild)
                return false;

            int dropIndex = ListElement.Children.IndexOf(dropOnChild);

            if (ListElement is VerticalStack)
            {
                if (touch.Position.Y > dropOnChild.ContentBounds.CenterY)
                {
                    dropIndex++;
                }
            }
            else
            {
                if (touch.Position.X > dropOnChild.ContentBounds.CenterX)
                {
                    dropIndex++;
                }
            }

            if (dragElement == ListElement)
            {
                int currentIndex = ListElement.Children.IndexOf(dropObject as UIElement);

                if ((currentIndex < (dropIndex - 1)) || (currentIndex == dropIndex))
                    dropIndex--;                

                ListElement.Children.Remove(dropObject as UIElement);
                ListElement.Children.Insert(dropIndex, dropObject as UIElement);
            }
            else
            {
                (dragElement as ListUIElement).Children.Remove(dropObject as UIElement);
                ListElement.Children.Insert(dropIndex, dropObject as UIElement);

                if (DragCompleteAction != null)
                    DragCompleteAction(dropObject);
            }

            Layout.Current.UpdateLayout();

            return true;
        }

        public override void HandleDragCancelled(object dropObject)
        {
            ListElement.ReleaseTouch();
        }

        public override void HandleDragCompleted(object dropObject)
        {
            ListElement.ReleaseTouch();
        }
    }
}
