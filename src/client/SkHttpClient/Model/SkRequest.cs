using System;
using System.Collections.Generic;
using System.Text;

namespace SkHttpClient.Model
{
    public class SkRequest
    {
        public readonly Dictionary<string, string> headers = new Dictionary<string, string>();

        private byte[] payload;

        /// <summary>
        /// paylaod requesta, automatycznie ustawia odpowiedni nagłóweg Content-Length
        /// </summary>
        public byte[] Payload
        {
            set
            {
                if (value == null || value.Length == 0)
                {
                    payload = new byte[0];
                }
                else
                {
                    payload = value;
                    headers["Content-Length"] = payload.Length.ToString();
                }
            }
        }

        private string mime;

        /// <summary>
        /// Typ MIME
        /// </summary>
        public string Mime
        {
            get { return mime; }
            set
            {
                if (mime != value && !string.IsNullOrEmpty(mime))
                {
                    mime = value;
                    headers["Content-type"] = mime;
                }
            }
        }

        /// <summary>
        /// Nazwa zasobu
        /// </summary>
        public string Resource { get; private set; }

        /// <summary>
        /// Metoda
        /// </summary>
        private readonly HttpMethod method;

        /// <summary>
        /// Tworzy request
        /// </summary>
        /// <param name="method">Metoda</param>
        /// <param name="resource">Nazwa zasobu</param>
        /// <param name="payload">Payload</param>
        public SkRequest(HttpMethod method, string resource, byte[] payload = null)
        {
            headers.Add("Cache-Control", "no-cache");
            headers.Add("Connection", "close");
            headers.Add("User-Agent", "SK HTTP Client");
            this.method = method;
            this.Resource = resource;
            this.Payload = payload;
            this.Mime = "application/octet-stream"; // niech serwer nie interpetuje tylko weźmie
            // jak leci tym bardziej że i tak nie sprawdza tego nagłówka
        }

        public HttpMethod Method
        {
            get { return method; }
        }

        public IEnumerable<byte> GetHttpVersionBytes()
        {
            return Encoding.ASCII.GetBytes(string.Format("{0} {1} HTTP/1.1\r\n", method, Resource));
        }

        public IEnumerable<byte> GetHeadersBytes()
        {
            List<byte> headerBytes = new List<byte>();
            foreach (var header in headers)
            {
                headerBytes.AddRange(Encoding.ASCII.GetBytes(string.Format("{0}: {1}\r\n", header.Key, header.Value)));
            }
            headerBytes.AddRange(Encoding.ASCII.GetBytes("\r\n"));
            return headerBytes;
        }

        public IEnumerable<byte> GetPayloadBytes()
        {
            return payload;
        }
    }
}