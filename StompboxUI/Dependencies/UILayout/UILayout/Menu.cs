using System;
using System.Collections.Generic;

namespace UILayout
{
    public class Menu : NinePatchWrapper, IPopup
    {
        public Action CloseAction { get; set; }

        public static UIFont DefaultFont = Layout.Current.DefaultFont;
        public static UIColor DefaultTextColor = UIColor.White;
        public static UIColor DefaultTextHighlightColor = new UIColor(255, 255, 100, 255);

        public UIColor TextColor { get; set; } = DefaultTextColor;
        public UIFont TextFont { get; set; } = DefaultFont;
        public UIColor TextHighlightColor { get; set; }

        VerticalStack menuStack;

        public Menu()
            : this(Layout.Current.DefaultOutlineNinePatch)
        {

        }

        public Menu(string ninePatchImageName)
            : this(Layout.Current.GetImage(ninePatchImageName))
        {

        }

        public Menu(UIImage ninePatchImage)
            : base(ninePatchImage)
        {
            TextColor = DefaultTextColor;
            TextHighlightColor = DefaultTextHighlightColor;

            menuStack = new VerticalStack();
            Child = menuStack;
        }

        public Menu(List<MenuItem> menuItems)
            : this()
        {
            SetMenuItems(menuItems);
        }

        public void SetMenuItems(List<MenuItem> menuItems)
        {
            menuStack.Children.Clear();

            foreach (MenuItem menuItem in menuItems)
            {
                TextButton button = new TextButton(menuItem.Text)
                {
                    HorizontalAlignment = EHorizontalAlignment.Stretch,
                    TextColor = TextColor,
                    TextFont = TextFont
                };
                button.ClickAction = delegate { DoMenuItem(menuItem); };

                menuStack.Children.Add(button);
            }
        }

        void DoMenuItem(MenuItem item)
        {
            if (item.SelectAction != null)
                item.SelectAction();

            if (item.CloseOnSelect && (CloseAction != null))
            {
                CloseAction();
            }

            if (item.AfterCloseAction != null)
            {
                item.AfterCloseAction();
            }
        }

        public void Opened()
        {

        }
    }

    public class MenuItem
    {
        public string Text { get; set; }
        public Action SelectAction { get; set; }
        public bool CloseOnSelect { get; set; }
        public Action AfterCloseAction { get; set; }
    }

    public class ContextMenuItem : MenuItem
    {
        public ContextMenuItem()
        {
            CloseOnSelect = true;
        }
    }
}
