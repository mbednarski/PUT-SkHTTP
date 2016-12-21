using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Threading.Tasks;
using System.Linq;

namespace SkHttpClient.Model
{
    public class SkClient
    {
        private const int BUFFER_SIZE = 1024 * 1024;
        private readonly string server;
        private readonly int port;

        public SkClient(string server, int port)
        {
            this.server = server;
            this.port = port;
        }

        public Task<SkResponse> ExecuteAsync(SkRequest request, Stream stream, IProgress<int> progress, SkResponse.WriteResponseToDiskDelegate toDiskDelegate = null)
        {
            return Task.Factory.StartNew(() =>
            {
                if(progress != null)
                    progress.Report(0);
                IPHostEntry hostEntry = Dns.GetHostEntry(server);
                IPEndPoint endpoint = new IPEndPoint(hostEntry.AddressList.First(
                    x => x.AddressFamily == AddressFamily.InterNetwork), port);

                Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                socket.Connect(endpoint);
                if (!socket.Connected)
                {
                    throw new Exception(string.Format("Failed to connect to {0}:{1}", server, port));
                }
                request.headers["Host"] = server;
                int sent = socket.Send(GetData(request));
                Debug.WriteLine("Sent {0} bytes.", sent);

                byte[] buffer = new byte[BUFFER_SIZE];
                int received;
                int i = 0;
                do
                {
                    received = socket.Receive(buffer);
                    Debug.WriteLine("Received {0} bytes", received);
                    stream.Write(buffer, 0, received);
                    if (i++ % 27 == 0 && progress != null)
                        progress.Report((int)(stream.Length / 1024));

                } while (received > 0);

                socket.Close();

                return new SkResponse(stream, toDiskDelegate);
            });
        }

        private byte[] GetData(SkRequest request)
        {
            List<byte> rawRequest = new List<byte>();

            rawRequest.AddRange(request.GetHttpVersionBytes());
            rawRequest.AddRange(request.GetHeadersBytes());
            rawRequest.AddRange(request.GetPayloadBytes());

            return rawRequest.ToArray();
        }
    }
}