using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Windows;
using GalaSoft.MvvmLight.Command;
using Microsoft.Win32;
using SkHttpClient.Annotations;
using SkHttpClient.Model;

namespace SkHttpClient.ViewModel
{
    public class PutSkViewModel : SkViewModel
    {
        public RelayCommand Browse { get; private set; }

        public PutSkViewModel()
        {
            Browse = new RelayCommand(PerformBrowse);
        }

        private string filename;

        public string Filename
        {
            get { return filename; }
            set
            {
                if (value == filename) return;
                filename = value;
                OnPropertyChanged();
            }
        }

        private byte[] content;

        public byte[] Content
        {
            get { return content; }
            private set
            {
                if (Equals(value, content)) return;
                content = value;
                OnPropertyChanged();
            }
        }

        private void PerformBrowse()
        {
            try
            {
                var dlg = new OpenFileDialog
                {
                    Multiselect = false,
                    CheckFileExists = true,
                    CheckPathExists = true
                };
                if (dlg.ShowDialog() == true)
                {
                    Content = File.ReadAllBytes(dlg.FileName);
                    Filename = dlg.FileName;
                }
            }
            catch (Exception exception)
            {
                MessageBox.Show(exception.Message);
            }
        }

        protected override async void PerformNavigate()
        {
            try
            {
                IsBusy = true;

                if (Content == null || Content.Length == 0)
                {
                    throw new Exception("File not selected");
                }

                var uri = new Uri(Address);
                var host = uri.Host;
                var resource = uri.AbsolutePath;
                var port = uri.Port;

                var sc = new SkClient(host, port);

                var request = RequestFactory.CreatePut(resource, Content);
                using (var ms = new MemoryStream())
                {
                    var response = await sc.ExecuteAsync(request, ms, progressIndicator);
                    Status = response.StatusCode + " " + response.ReasonPhase;
                    Headers.Clear();
                    foreach (var header in response.Headers)
                    {
                        Headers.Add(header.Key + ": " + header.Value);
                    }
                    if (response.Viewable)
                        Html = response.PayloadString;
                    else
                    {
                        Html = "";
                    }
                }
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
            }
            finally
            {
                IsBusy = false;
            }
        }
    }
}