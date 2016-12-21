using System;
using System.IO;
using System.Linq;
using System.Net.NetworkInformation;
using System.Windows;
using GalaSoft.MvvmLight.Command;
using Microsoft.Win32;
using SkHttpClient.Model;

namespace SkHttpClient.ViewModel
{
    public class GetSkViewModel : SkViewModel
    {
        protected override async void PerformNavigate()
        {
            // pobierz plik tymczasowy do którego zostanie zapisany content
            string tmp = Path.GetTempFileName();
            try
            {
                IsBusy = true;
                var uri = new Uri(Address);
                var host = uri.Host;
                var resource = uri.AbsolutePath;
                var port = uri.Port;

                var sc = new SkClient(host, port);

                var request = RequestFactory.CreateGet(resource);

                using (var fs = new FileStream(tmp, FileMode.Create))
                {
                    var response = await sc.ExecuteAsync(request, fs, progressIndicator, stream =>
                    {
                        // zasób ma typ MIME którego nie umiemy wyświetlić, Pytamy o plik do którego go zapisać
                        var dlg = new SaveFileDialog();
                        dlg.OverwritePrompt = true;
                        dlg.FileName = uri.Segments.Last();
                        if (dlg.ShowDialog() == true)
                        {
                            using (var dstFile = dlg.OpenFile())
                            {
                                fs.CopyTo(dstFile);
                            }
                        }
                    });

                    Status = response.StatusCode + " " + response.ReasonPhase;

                    Headers.Clear();
                    foreach (var header in response.Headers)
                    {
                        Headers.Add(header.Key + ": " + header.Value);
                    }

                    if (response.Viewable)
                    {
                        Html = response.PayloadString;
                    }
                    else
                    {
                        Html = "Pobrano plik";
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            finally
            {
                IsBusy = false;
                try
                {
                    File.Delete(tmp);
                }
                catch
                {
                    // pass
                }
            }
        }
    }
}
