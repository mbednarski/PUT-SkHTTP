using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using SkHttpClient.ViewModel;

namespace SkHttpClient.View
{
    /// <summary>
    /// Interaction logic for GetView.xaml
    /// </summary>
    public partial class GetView : UserControl
    {
        public GetView()
        {
            InitializeComponent();
        }

        private void WebBrowser_Navigated(object sender, NavigationEventArgs e)
        {
            var vm = (SkViewModel) this.DataContext;
            if(e.Uri != null)
                vm.Address = e.Uri.ToString();
        }
        
        private void TextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                var vm = (SkViewModel) this.DataContext;
                vm.Navigate.Execute(null);
            }
        }
    }
}
