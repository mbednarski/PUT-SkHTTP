using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;

namespace SkHttpClient.Model
{
    public class SkResponse
    {
        private const int MAX_HEADER_SIZE = 4096;
        private readonly int statusCode;

        private readonly Dictionary<string, string> headers = new Dictionary<string, string>();
        private readonly List<byte> payload;
        public Dictionary<string, string> Headers { get { return headers; } }

        public bool Viewable { get; private set; }


        public string PayloadString { get; set; }

        public int StatusCode
        {
            get { return statusCode; }
        }

        public string ReasonPhase { get; set; }

        public delegate void WriteResponseToDiskDelegate(Stream stream);

        /// <summary>
        /// Tworzy odpowiedź ze strumienia
        /// </summary>
        /// <param name="stream">Strumień źródłowy</param>
        /// <param name="toDiskDelegate">Funkcja odpalana w wypadku kiedy nalezy zapisać payload na dysk</param>
        public SkResponse(Stream stream, WriteResponseToDiskDelegate toDiskDelegate)
        {
            // pusta odpowiedź
            stream.Seek(0, SeekOrigin.Begin);
            if (stream.Length == 0)
            {
                throw new Exception("Empty response!");
            }

            var sr = new StreamReader(stream, Encoding.Default, false, 64, true);

            // parsuj pierwszą linię
            string firstLine = sr.ReadLine();
            var headerLine = firstLine.Split(' ');
            statusCode = int.Parse(headerLine[1]);
            ReasonPhase = string.Join(" ", headerLine.Skip(2));
            long headerEnd = firstLine.Length + 2;

            string line;
            do
            {
                // parsuj kolejne linie zawierające nagłówki. Jeśli jest pust to oznacza koniec i start paylaodu
                line = sr.ReadLine();
                if (string.IsNullOrEmpty(line))
                {
                    Debug.WriteLine(stream.Position);
                    break;
                }
                var separated = line.Split(new string[] { ": " }, StringSplitOptions.None);
                headers.Add(separated[0], separated[1]);
                headerEnd += line.Length + 2;

            } while (true);

            // "zgub" ostatnie łamanuie linii
            headerEnd += 2;

            Viewable = CheckMimeToDisplay();

            // jeśli nie nadaje się do wyświetlenia zapisz na dysk
            if (!Viewable)
            {
                // sam paylaod, bez nagłówków
                stream.Seek(headerEnd, SeekOrigin.Begin);
                if(toDiskDelegate != null)
                    toDiskDelegate(stream);
            }
            else
            {
                PayloadString = sr.ReadToEnd();
            }
        }

        /// <summary>
        /// Sprawdza czy potrafimy wyświetlić dany tym MIME
        /// </summary>
        /// <returns></returns>
        private bool CheckMimeToDisplay()
        {
            if (!headers.ContainsKey("Content-Type"))
                return true;
            var mime = headers["Content-Type"];
            var allowedMimes = new SortedSet<string> { "text/html", "text/plain" };
            return allowedMimes.Any(allowedMime => mime.StartsWith(allowedMime));
        }
    }
}