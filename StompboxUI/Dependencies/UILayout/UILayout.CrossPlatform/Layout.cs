using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;
using NativeFileDialogNET;

namespace UILayout
{
    public partial class Layout
    {
        public string GetFolder(string initialPath)
        {
            //try
            //{
            //    Process process = new Process();
            //    process.StartInfo.RedirectStandardOutput = true;
            //    process.StartInfo.FileName = "zenity";
            //    process.StartInfo.Arguments = "--file-selection --filename=" + initialPath + " --directory";

            //    process.Start();
            //    return process.StandardOutput.ReadToEnd().Trim();
            //}
            //catch { }

            //return null;

            using var selectFolderDialog = new NativeFileDialog()
                .SelectFolder();

            string? folder;

            var result = selectFolderDialog.Open(out folder, initialPath);

            if (result == DialogResult.Okay)
            {
                return folder;
            }

            return null;
        }

        public string GetFile(string initialPath)
        {
            return GetFile(initialPath, null, null);
        }

        public string GetFile(string initialPath, string patternName, string patternWildcard)
        {
            //try
            //{
            //    Process process = new Process();
            //    process.StartInfo.RedirectStandardOutput = true;
            //    process.StartInfo.FileName = "zenity";
            //    process.StartInfo.Arguments = "--file-selection --filename=" + initialPath;

            //    process.Start();
            //    return process.StandardOutput.ReadToEnd().Trim();
            //}
            //catch { }

            using var selectFileDialog = new NativeFileDialog()
                .SelectFile();

            if (!string.IsNullOrEmpty(patternName))
                selectFileDialog.AddFilter(patternName, patternWildcard);

            string? file;

            var result = selectFileDialog.Open(out file, initialPath);

            if (result == DialogResult.Okay)
            {
                return file;
            }

            return null;
        }
    }    
}
