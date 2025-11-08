using System;
using System.Collections.Generic;
using System.Text;
using UILayout;

namespace UILayout
{
    public class TabPanelTab
    {
        public TabPanelTab(string name, UIElement contents)
        {
            this.Name = name;
            this.Contents = contents;
        }

        public string Name { get; set; }
        public UIElement Contents { get; set; }
        public TabPanelButton Button { get; internal set; }
    }

    public class TabPanelButton : NinePatchButton
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

        public TabPanelButton(string text)
            : this()
        {
            Text = text;
        }

        public TabPanelButton()
        {
            VerticalAlignment = EVerticalAlignment.Bottom;

            textBlock = new TextBlock
            {
                HorizontalAlignment = EHorizontalAlignment.Center,
                VerticalAlignment = EVerticalAlignment.Center,
            };

            SetElements(textBlock, textBlock);
        }
    }

    //             TabPanel tabPanel = new TabPanel(new UIColor(100, 100, 100), UIColor.White, Layout.Current.GetImage("TabPanelBackground"), Layout.Current.GetImage("TabForeground"), Layout.Current.GetImage("TabBackground"), 5, 5);
    public class TabPanel : VerticalStack
    {
        public TabPanelTab ActiveTab { get; private set; }

        public static UIColor BackgroundColor = new UIColor(100, 100, 100);
        public static UIColor TextColor = Layout.Current.DefaultForegroundColor;
        public static UIImage BackgroundImage = Layout.Current.GetImage("TabPanelBackground");
        public static UIImage TabButtonPressed = Layout.Current.GetImage("TabForeground");
        public static UIImage TabButtonUnpressed = Layout.Current.GetImage("TabBackground");
        public static float TabStartOffsetX = 5;
        public static float TabStartOffsetY = 5;

        Dictionary<string, TabPanelTab> tabs = new Dictionary<string, TabPanelTab>();

        UIColor textColor;
        HorizontalStack headerStack;
        NinePatchWrapper contentsPanel;
        UIElementWrapper contentsWrapper;
        UIImage tabButtonPressed;
        UIImage tabButtonUnpressed;

        public TabPanel()
            : this(BackgroundColor, TextColor, BackgroundImage, TabButtonPressed, TabButtonUnpressed, TabStartOffsetX, TabStartOffsetY)
        {

        }

        public TabPanel(UIColor backgroundColor, UIColor textColor, UIImage backroundImage, UIImage tabButtonPressed, UIImage tabButtonUnpressed, float tabStartOffsetX, float tabStartOfsetY)
        {
            HorizontalAlignment = EHorizontalAlignment.Stretch;
            VerticalAlignment = EVerticalAlignment.Stretch;

            this.textColor = textColor;
            this.tabButtonPressed = tabButtonPressed;
            this.tabButtonUnpressed = tabButtonUnpressed;

            Children.Add(new UIElement { DesiredHeight = tabStartOfsetY });

            headerStack = new HorizontalStack()
            {
                HorizontalAlignment = EHorizontalAlignment.Stretch,
                Padding = (tabStartOffsetX, 0),
                ChildSpacing = 5
            };
            Children.Add(headerStack);

            contentsPanel = new NinePatchWrapper(backroundImage)
            {
                DrawCenter = false,
                //DrawOnTop = true,
                HorizontalAlignment = EHorizontalAlignment.Stretch,
                VerticalAlignment = EVerticalAlignment.Stretch
            };
            Children.Add(contentsPanel);

            contentsWrapper = new UIElementWrapper
            {
                BackgroundColor = backgroundColor,
                HorizontalAlignment = EHorizontalAlignment.Stretch,
                VerticalAlignment = EVerticalAlignment.Stretch,
                Padding = 11
            };
            contentsPanel.Child = contentsWrapper;
        }

        protected override void GetContentSize(out float width, out float height)
        {
            width = 0;
            height = 0;

            UIElement currentContents = contentsWrapper.Child;

            foreach (var tab in tabs.Values)
            {
                float tabWidth;
                float tabHeight;

                contentsWrapper.Child = tab.Contents;

                base.GetContentSize(out tabWidth, out tabHeight);

                width = Math.Max(width, tabWidth);
                height = Math.Max(height, tabHeight);
            }

            contentsWrapper.Child = currentContents;
        }

        public TabPanelTab AddTab(string text, UIElement contents)
        {
            TabPanelTab tab = new TabPanelTab(text, contents);

            tabs[tab.Name] = tab;

            headerStack.Children.Add(tab.Button = new TabPanelButton(tab.Name)
            {
                TextColor = textColor,
                PressAction = delegate
                {
                    SetTab(tab.Name);
                }
            });

            if (tabs.Count == 1)
            {
                ActiveTab = tab;
                contentsWrapper.Child = tab.Contents;

                tab.Button.PressedNinePatch.Image = tab.Button.UnpressedNinePatch.Image = tabButtonPressed;
            }
            else
            {
                tab.Button.PressedNinePatch.Image = tab.Button.UnpressedNinePatch.Image = tabButtonUnpressed;
            }

            return tab;
        }

        public void SetTab(string tabName)
        {
            foreach (TabPanelTab tab in tabs.Values)
            {
                if (tab.Name == tabName)
                {
                    ActiveTab = tab;

                    contentsWrapper.Child = tab.Contents;

                    tab.Button.PressedNinePatch.Image = tab.Button.UnpressedNinePatch.Image = tabButtonPressed;
                }
                else
                {
                    tab.Button.PressedNinePatch.Image = tab.Button.UnpressedNinePatch.Image = tabButtonUnpressed;
                }
            }

            UpdateContentLayout();
        }
    }
}
