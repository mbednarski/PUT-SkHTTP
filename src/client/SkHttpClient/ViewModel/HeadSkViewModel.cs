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
    public class HeadSkViewModel : SkViewModel
    {
        protected override async void PerformNavigate()
        {
            try
            {
                var uri = new Uri(Address);
                var host = uri.Host;
                var resource = uri.PathAndQuery;
                var port = uri.Port;

                var sc = new SkClient(host, port);

                var request = RequestFactory.CreateHead(resource);

                using (var ms = new MemoryStream())
                {
                    var response = await sc.ExecuteAsync(request, ms, progressIndicator);
                    Status = response.StatusCode + " " + response.ReasonPhase;

                    Headers.Clear();
                    foreach (var header in response.Headers)
                    {
                        Headers.Add(header.Key + ": " + header.Value);
                    }
                }
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
            }
        }
        
    }
}