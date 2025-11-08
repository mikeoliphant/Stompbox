using System;
using System.Collections.Generic;

namespace UILayout.Test
{
    public class LayoutTest : Dock
    {
        TextBlock spaceText;

        public LayoutTest()
        {
            BackgroundColor = new UIColor(25, 25, 25);
            Padding = 10;

            VerticalStack vStack = new VerticalStack()
            {
                HorizontalAlignment = EHorizontalAlignment.Stretch,
                VerticalAlignment = EVerticalAlignment.Stretch,
                ChildSpacing = 20
            };
            Children.Add(vStack);

            HorizontalStack buttonStack = new HorizontalStack()
            {
                HorizontalAlignment = EHorizontalAlignment.Center,
                VerticalAlignment = EVerticalAlignment.Center
            };
            vStack.Children.Add(buttonStack);

            InputDialog dialog = new InputDialog(Layout.Current.DefaultOutlineNinePatch, new TextBlock { Text = "Do you want to?" });

            dialog.AddInput(new DialogInput { Text = "Ok", CloseOnInput = true });
            dialog.AddInput(new DialogInput { Text = "Cancel", CloseOnInput = true });

            buttonStack.Children.Add(new TextButton()
            {
                Text = "Input Dialog",
                HorizontalAlignment = EHorizontalAlignment.Center,
                VerticalAlignment = EVerticalAlignment.Center,
                ClickAction = delegate
                {
                    Layout.Current.ShowPopup(dialog);
                }
            });

            List<MenuItem> menuItems = new List<MenuItem>
            {
                new ContextMenuItem { Text = "Item 1" },
                new ContextMenuItem { Text = "Item 2"},
                new ContextMenuItem { Text = "Item 3 longer"}
            };

            Menu menu = new Menu(menuItems);

            TextButton menuButton = new TextButton()
            {
                Text = "Popup Menu",
                HorizontalAlignment = EHorizontalAlignment.Center,
                VerticalAlignment = EVerticalAlignment.Center,
            };

            menuButton.ClickAction = delegate
            {
                Layout.Current.ShowPopup(menu, menuButton.ContentBounds.Center);
            };

            buttonStack.Children.Add(menuButton);

            HorizontalStack swipeStack = new HorizontalStack()
            {
                HorizontalAlignment = EHorizontalAlignment.Center,
                VerticalAlignment = EVerticalAlignment.Center
            };

            SwipeList swipeList = new SwipeList()
            {
                TextColor = UIColor.Black,
                DesiredHeight = 200,
                HorizontalAlignment = EHorizontalAlignment.Center,
                VerticalAlignment = EVerticalAlignment.Center,
                DesiredWidth = 100,
                BackgroundColor = UIColor.White
            };
            swipeStack.Children.Add(swipeList);

            var scrollBar = new VerticalScrollBarWithArrows()
            {
                VerticalAlignment = EVerticalAlignment.Stretch,
            };

            swipeStack.Children.Add(scrollBar);

            swipeList.SetScrollBar(scrollBar.ScrollBar);

            List<string> items = new List<string>();

            for (int i = 0; i < 30; i++)
            {
                items.Add("Item " + i);
            }

            swipeList.Items = items;
            swipeList.SelectAction = delegate (int item)
            {
                Layout.Current.ClosePopup(swipeStack);
            };

            buttonStack.Children.Add(new TextButton()
            {
                Text = "Swipe List",
                HorizontalAlignment = EHorizontalAlignment.Center,
                VerticalAlignment = EVerticalAlignment.Center,
                ClickAction = delegate
                {
                    Layout.Current.ShowPopup(swipeStack);
                }
            });

            buttonStack.Children.Add(new TextButton()
            {
                Text = "Text Input",
                HorizontalAlignment = EHorizontalAlignment.Center,
                VerticalAlignment = EVerticalAlignment.Center,
                ClickAction = delegate
                {
                    Layout.Current.ShowTextInputPopup("Enter text:", str => Layout.Current.ShowContinuePopup("You entered: " + str));
                }
            });

            HorizontalStack buttonStack2 = new HorizontalStack()
            {
                HorizontalAlignment = EHorizontalAlignment.Center,
                VerticalAlignment = EVerticalAlignment.Center
            };
            vStack.Children.Add(buttonStack2);

            FileSelector fileSelector = new FileSelector("File Selection:", canCreateFolders: false, Layout.Current.DefaultOutlineNinePatch)
            {
                HorizontalAlignment = EHorizontalAlignment.Stretch,
                VerticalAlignment = EVerticalAlignment.Stretch
            };

            fileSelector.SetRootPath(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments));
            fileSelector.FileAction = delegate (string filePath)
            {
                Layout.Current.ShowContinuePopup("Path Selected:\n\n" + filePath);
            };

            buttonStack2.Children.Add(new TextButton()
            {
                Text = "File Selection",
                HorizontalAlignment = EHorizontalAlignment.Center,
                VerticalAlignment = EVerticalAlignment.Center,
                ClickAction = delegate
                {
                    Layout.Current.ShowPopup(fileSelector);
                }});

            buttonStack2.Children.Add(new TextButton()
            {
                Text = "Native File Selection",
                HorizontalAlignment = EHorizontalAlignment.Center,
                VerticalAlignment = EVerticalAlignment.Center,
                ClickAction = delegate
                {
                    Layout.Current.GetFile(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments));
                }
            });

            TabPanel tabPanel = new TabPanel(new UIColor(100, 100, 100), UIColor.White, Layout.Current.GetImage("TabPanelBackground"), Layout.Current.GetImage("TabForeground"), Layout.Current.GetImage("TabBackground"), 5, 5);
            vStack.Children.Add(tabPanel);

            tabPanel.AddTab("Tab 1", new TextBlock("This is tab 1"));
            tabPanel.AddTab("Tab 2", new TextBlock("This is tab 2"));
            tabPanel.AddTab("Tab 3", new TextBlock("This is tab 3"));

            HorizontalStack dragDropStack = new HorizontalStack()
            {
                HorizontalAlignment = EHorizontalAlignment.Center,
                ChildSpacing = 2
            };
            vStack.Children.Add(dragDropStack);

            ListUIDragDropHandler dragDropHander = new ListUIDragDropHandler()
            {
                ListElement = dragDropStack,
                DragType = typeof(NinePatchWrapper),
                InternalOnly = true
            };

            dragDropStack.DragDropHandler = dragDropHander;

            for (int i = 0; i < 5; i++)
            {
                dragDropStack.Children.Add(new NinePatchWrapper(Layout.Current.GetImage("ButtonPressed"))
                {
                    Child = new TextBlock("Drag " + (i + 1).ToString())
                });
            }

            vStack.Children.Add(dragDropStack);

            Layout.Current.InputManager.AddMapping("SpacePressed", new KeyMapping(InputKey.Space));

            vStack.Children.Add(spaceText = new TextBlock
            {
                Text = "Press Space",
                TextColor = UIColor.Black,
                BackgroundColor = UIColor.Green,
                HorizontalAlignment = EHorizontalAlignment.Center,
                VerticalAlignment = EVerticalAlignment.Center,
                Padding = (20, 10)
            });
        }

        public override void HandleInput(InputManager inputManager)
        {
            base.HandleInput(inputManager);

            if (inputManager.WasPressed("SpacePressed"))
            {
                spaceText.BackgroundColor = UIColor.Red;
            }

            if (inputManager.WasReleased("SpacePressed"))
            {
                spaceText.BackgroundColor = UIColor.Green;
            }
        }
    }
}
