using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace UILayout
{
    public partial class Layout
    {
        public string GetFolder(string initialPath)
        {
            FolderBrowserDialog dialog = new FolderBrowserDialog();

            dialog.SelectedPath = initialPath;

            if (dialog.ShowDialog() == DialogResult.OK)
            {
                return dialog.SelectedPath;
            }

            return null;
        }

        public string GetFile(string initialPath)
        {
            return GetFile(initialPath, null, null);
        }

        public string GetFile(string initialPath, string patternName, string patternWildcard)
        {
            OpenFileDialog dialog = new OpenFileDialog();

            if (patternName != null)
                dialog.Filter = patternName + "|" + patternWildcard;
            dialog.InitialDirectory = initialPath;

            if (dialog.ShowDialog() == DialogResult.OK)
            {
                return dialog.FileName;
            }

            return null;
        }
    }
}
