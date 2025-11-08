using System;
using System.Collections.Generic;
using System.IO;

namespace UILayout
{
    public class FileMetadata : IComparable<FileMetadata>
    {
        public string FilePath { get; set; }
        public string FileExtension { get; set; }
        public bool IsDirectory { get; set; }

        public override string ToString()
        {
            if (IsDirectory)
            {
                return "-> " + Path.GetFileName(FilePath);
            }
            else
            {
                return Path.GetFileName(FilePath);
            }
        }

        public int CompareTo(FileMetadata other)
        {
            if (IsDirectory == other.IsDirectory)
            {
                return FilePath.CompareTo(other.FilePath);
            }

            return other.IsDirectory.CompareTo(IsDirectory);
        }
    }

    public class FileSelector : InputDialog
    {
        public static bool DoNativeFileSelector { get; set; }

        public Action<string> FileAction { get; set; }
        public bool ShowExtensions { get; set; }
        public string[] AllowedExtensions { get; set; }
        public bool SelectFolders { get; set; }
        public bool IsSaveMode { get; set; }
        public string DefaultSaveName { get; set; }
        public bool ShowFolders { get; set; }
        public Action RecoverFromFileError { get; set; }

        protected List<FileMetadata> files = new List<FileMetadata>();
        protected string rootPath = "";
        protected string currentPath = "";

        Dock mainDock;
        SwipeList fileList;
        Stack<string> backDirStack = new Stack<string>();
        bool canCreateFolders;
        TextBlock headerTextBlock;
        bool isSaveMode;

        public FileSelector(string headerText, bool canCreateFolders, UIImage ninePatchImage)
            : base(ninePatchImage)
        {
            this.canCreateFolders = canCreateFolders;

            ShowFolders = true;

            mainDock = new Dock();
            this.SetContents(mainDock);

            VerticalStack stack = new VerticalStack();
            stack.DrawInReverse = true;
            stack.HorizontalAlignment = EHorizontalAlignment.Stretch;
            stack.VerticalAlignment = EVerticalAlignment.Stretch;
            mainDock.Children.Add(stack);

            VerticalStack header = new VerticalStack
            {
                BackgroundColor = new UIColor(100, 100, 100),
                HorizontalAlignment = EHorizontalAlignment.Stretch,
                Padding = new LayoutPadding(0, 5)
            };
            stack.Children.Add(header);

            headerTextBlock = new TextBlock(headerText) { Padding = new LayoutPadding(5) };
            header.Children.Add(headerTextBlock);

            inputStack.HorizontalAlignment = EHorizontalAlignment.Stretch;
            inputStack.Padding = new LayoutPadding(0, 5);
            inputStack.BackgroundColor = new UIColor(100, 100, 100);

            if (isSaveMode)
            {
                AddInput(new DialogInput()
                {
                    Text = "Save As",
                    Action = SaveAs
                });

            }

            if (canCreateFolders)
            {
                AddInput(new DialogInput()
                {
                    Text = "New Folder",
                    Action = CreateFolder
                });
            }

            AddInput(new DialogInput()
            {
                Text = "Back",
                Action = delegate
                {
                    if (backDirStack.Count > 0)
                    {
                        currentPath = backDirStack.Pop();

                        UpdateListing();
                    }
                }
            });

            AddInput(new DialogInput()
            {
                Text = "Cancel",
                Action = Exit
            });

            //NinePatch separator = new NinePatch(PixGame.Instance.GetImage("Separator")) { HorizontalAlignment = EHorizontalAlignment.Stretch, Height = PixGame.PixelScale * 2 };
            //stack.Children.Add(separator);

            HorizontalStack listStack = new HorizontalStack()
            {
                HorizontalAlignment = EHorizontalAlignment.Stretch,
                VerticalAlignment = EVerticalAlignment.Stretch,
            };
            stack.Children.Add(listStack);

            fileList = new SwipeList();
            fileList.TextColor = Layout.Current.DefaultForegroundColor;
            fileList.SelectAction = FileSelected;
            //fileList.HoldAction = ShowContextMenu;
            fileList.ItemHeight = Layout.Current.DefaultFont.TextHeight * 1.2f;
            fileList.ItemYOffset = (fileList.ItemHeight - Layout.Current.DefaultFont.TextHeight) / 2;
            fileList.HorizontalAlignment = EHorizontalAlignment.Stretch;
            fileList.VerticalAlignment = EVerticalAlignment.Stretch;
            fileList.Items = new List<FileMetadata>();
            listStack.Children.Add(fileList);

            VerticalScrollBarWithArrows scrollBar = new VerticalScrollBarWithArrows()
            {
                VerticalAlignment = EVerticalAlignment.Stretch,
            };
            listStack.Children.Add(scrollBar);

            fileList.SetScrollBar(scrollBar.ScrollBar);
            scrollBar.ScrollBar.Scrollable = fileList;

        }

        public static string SanitizeFilename(string filename)
        {
            return filename; // PixGame.Instance.StorageManager.SanitizeFilename(filename).Replace('.', '_');
        }

        string nativePath = null;

        public void SetNativePath(string path)
        {
            nativePath = path;
        }

        public override void Opened()
        {
            base.Opened();

            UpdateListing();
        }

        public void ShowFileErrorPopup(Exception ex)
        {
            //return base.ShowFileErrorPopup(ex, delegate
            //{
            //    PixGame.Instance.PopGameState();

            //    if (RecoverFromFileError != null)
            //        RecoverFromFileError();
            //});
        }

        public void SetRootPath(string path)
        {
            SetRootPath(path, null);
        }

        public void SetRootPath(string rootPath, string relativePath)
        {
            this.rootPath = rootPath;

            backDirStack.Clear();
            currentPath = rootPath;

            if (!string.IsNullOrEmpty(relativePath))
            {
                if (relativePath.StartsWith(rootPath))
                {
                    relativePath = relativePath.Substring(rootPath.Length);

                    string[] dirs = relativePath.Split('/');

                    foreach (string dir in dirs)
                    {
                        PushPath(Path.Combine(currentPath, dir));
                    }
                }
            }
        }

        protected virtual void ClearFiles()
        {
            if (files != null)
            {
                files.Clear();
            }

            if (fileList.Items != null)
            {
                fileList.Items.Clear();
            }
        }

        public virtual void GetFiles()
        {
            foreach (string file in Directory.GetFiles(currentPath))
            {
                if (AllowedExtensions != null)
                {
                    bool allowed = false;

                    foreach (string allowedExt in AllowedExtensions)
                    {
                        if (Path.GetExtension(file).Equals(allowedExt, StringComparison.InvariantCultureIgnoreCase))
                        {
                            allowed = true;
                            break;
                        }
                    }

                    if (!allowed)
                        continue;
                }

                FileMetadata metaData = new FileMetadata { FilePath = ShowExtensions ? Path.GetFileName(file) : Path.GetFileNameWithoutExtension(file), FileExtension = Path.GetExtension(file), IsDirectory = false };

                files.Add(metaData);
            }

            if (ShowFolders)
            {
                foreach (string subdir in Directory.GetDirectories(currentPath))
                {
                    FileMetadata metaData = new FileMetadata { FilePath = subdir, IsDirectory = true };

                    files.Add(metaData);
                }
            }
        }

        public virtual void DisplayFiles()
        {
            //backButton.Visible = (backDirStack.Count > 0);
            //PixGame.Instance.UserInterface.NeedLayoutUpdate = true;

            files.Sort();

            fileList.Items = files;
        }

        public virtual void UpdateListing()
        {
            ClearFiles();

            try
            {
                GetFiles();
            }
            catch (Exception ex)
            {
                ShowFileErrorPopup(ex);
            }

            DisplayFiles();
        }

        public void PushPath(string path)
        {
            backDirStack.Push(currentPath);
            currentPath = path;

            UpdateListing();
        }

        protected virtual void SaveAs()
        {
            Layout.Current.ShowTextInputPopup("File Name:", DefaultSaveName, Save);
        }

        protected virtual void Save(string name)
        {
            if (name != null)
            {
                name = SanitizeFilename(name);

                if (!string.IsNullOrEmpty(name))
                {
                    FileAction(Path.Combine(currentPath, name));
                }
            }
        }

        protected virtual void CreateFolder()
        {
            Layout.Current.ShowTextInputPopup("Folder Name:", null, CreateFolder);
        }

        void CreateFolder(string name)
        {
            if (name != null)
            {
                name = SanitizeFilename(name);

                if (!String.IsNullOrEmpty(name))
                {
                    string dirPath = Path.Combine(currentPath, name);

                    try
                    {
                        if (Directory.Exists(dirPath))
                        {
                            Layout.Current.ShowContinuePopup("A folder by that name already exists.");
                        }
                        else
                        {
                            Directory.CreateDirectory(dirPath);

                            UpdateListing();
                        }
                    }
                    catch (Exception ex)
                    {
                        ShowFileErrorPopup(ex);
                    }
                }
            }
        }

        void FileSelected(int index)
        {
            SelectFile(index);
        }

        void SelectFile(int index)
        {
            if (files[index].IsDirectory)
            {
                PushPath(Path.Combine(currentPath, files[index].FilePath));
            }
            else
            {
                DoFileAction(index);
            }
        }

        protected virtual void DoFileAction(int index)
        {
            FileAction(Path.Combine(currentPath, files[index].FilePath));
        }

        protected void DeleteFileWithPrompt(int index)
        {
            Layout.Current.ShowConfirmationPopup(String.Format("Are you sure you want to\ndelete '{0}'?", files[index].FilePath), delegate { DeleteFile(files[index]); }); 
        }

        protected virtual void DeleteFile(FileMetadata file)
        {
            try
            {
                    File.Delete(Path.Combine(currentPath, file.FilePath) + file.FileExtension);
            }
            catch (Exception ex)
            {
                ShowFileErrorPopup(ex);
            }

            UpdateListing();
        }

        protected void DeleteFolderWithPrompt(int index)
        {
            Layout.Current.ShowConfirmationPopup(String.Format("Are you sure you want to delete folder\n'{0}' and all of its contents?", files[index].FilePath),
                delegate { DeleteFolderRecursively(files[index].FilePath); });
        }

        protected void DeleteFolderRecursively(string name)
        {
            DeleteFolder(Path.Combine(currentPath, name));

            UpdateListing();
        }

        protected void DeleteFolder(string folderPath)
        {
            try
            {
                foreach (string subdir in Directory.GetDirectories(folderPath))
                {
                    DeleteFolder(Path.Combine(folderPath, subdir));
                }

                foreach (string file in Directory.GetFiles(folderPath))
                {
                    File.Delete(Path.Combine(folderPath, file));
                }

                Directory.Delete(folderPath);
            }
            catch (Exception ex)
            {
                ShowFileErrorPopup(ex);
            }
        }
    }
}
