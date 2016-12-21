using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Windows.Threading;
using GalaSoft.MvvmLight.Command;
using SkHttpClient.Annotations;

namespace SkHttpClient.ViewModel
{
    /// <summary>
    /// Bazowy ViewModel dla wszystkich pozostałych, zawiera wszystkie wspólne elementy
    /// </summary>
    public abstract class SkViewModel : INotifyPropertyChanged
    {
        protected readonly IProgress<int> progressIndicator;
        private string address = "http://localhost:8080";
        public RelayCommand Navigate { get; private set; }

        private bool isBusy;

        private int progressValue;

        private string status;

        protected SkViewModel()
        {
            Headers = new ObservableCollection<string>();
            progressIndicator = new Progress<int>(x => Dispatcher.CurrentDispatcher.Invoke(() => ProgressValue = x));

            Navigate = new RelayCommand(PerformNavigate, () => !IsBusy);
            this.PropertyChanged += (o, e) =>
            {
                if (e.PropertyName == "IsBusy")
                    Navigate.RaiseCanExecuteChanged();
            };
        }

        protected abstract void PerformNavigate();

        /// <summary>
        /// Adres zasobu
        /// </summary>
        public string Address
        {
            get { return address; }
            set
            {
                if (value == address) return;
                address = value;
                OnPropertyChanged();
            }
        }

        /// <summary>
        /// Status żadania (kod i reason phase)
        /// </summary>
        public string Status
        {
            get { return status; }
            set
            {
                if (value == status) return;
                status = value;
                OnPropertyChanged();
            }
        }

        /// <summary>
        /// Zwrócone nagłówki
        /// </summary>
        public ObservableCollection<string> Headers { get; private set; }

        /// <summary>
        /// Czy jest w trakcie komunikacji?
        /// </summary>
        public bool IsBusy
        {
            get { return isBusy; }
            set
            {
                if (value == isBusy) return;
                isBusy = value;
                OnPropertyChanged();
            }
        }

        /// <summary>
        /// Wartość postępu przy pobieraniu
        /// </summary>
        public int ProgressValue
        {
            get { return progressValue; }
            set
            {
                if (value == progressValue) return;
                progressValue = value;
                OnPropertyChanged();
            }
        }


        private string html;
        /// <summary>
        /// Kod HTML lub inny zwrócony w odpowiedzi
        /// </summary>
        public string Html
        {
            get { return html; }
            set
            {
                if (value == html) return;
                html = value;
                OnPropertyChanged();
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        [NotifyPropertyChangedInvocator]
        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            var handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}