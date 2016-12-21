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
    public class DeleteSkViewModel : SkViewModel
    {
        protected override async void PerformNavigate()
        {
            try
            {
                var uri = new Uri(Address);
                var host = uri.Host;
                var resource = uri.AbsolutePath;
                int port = uri.Port;

                var sc = new SkClient(host, port);

                var request = RequestFactory.CreateDelete(resource);
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
                MessageBox.Show(e.ToString());
            }
        }
    }
}