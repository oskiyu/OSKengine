using System;
using System.Buffers.Binary;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace SocketsTest {

    enum Protocol {
        TCP,
        UDP
    }

    struct Connection {

        public void Set(string ip, int port) {
            ipAddress = IPAddress.Parse(ip);

            EndPoint = new IPEndPoint(ipAddress, port);
        }

        public void Connect(Protocol protocol) {
            ProtocolType p = ProtocolType.Tcp;
            if (protocol == Protocol.UDP)
                p = ProtocolType.Udp;

            ConnectionSocket = new Socket(ipAddress.AddressFamily, SocketType.Stream, p);
            ConnectionSocket.Connect(EndPoint);
        }

        public void Send(Message msg) {
            ConnectionSocket.Send(msg.GetBytesToSend());
        }

        private IPAddress ipAddress;
        internal IPEndPoint EndPoint { get; private set; }
        internal Socket ConnectionSocket { get; private set; }

    }

    class Message {
        List<byte> bytes = new List<byte>();

        void Write(uint data) {
            bytes.AddRange(BitConverter.GetBytes(data));
        }

        void Write(int data) {
            bytes.AddRange(BitConverter.GetBytes(data));
        }

        void Write(float data) {
            bytes.AddRange(BitConverter.GetBytes(data));
        }

        void Write(double data) {
            bytes.AddRange(BitConverter.GetBytes(data));
        }

        void Write(long data) {
            bytes.AddRange(BitConverter.GetBytes(data));
        }

        void Write(ulong data) {
            bytes.AddRange(BitConverter.GetBytes(data));
        }

        void Write(char data) {
            bytes.AddRange(BitConverter.GetBytes(data));
        }

        void Write(short data) {
            bytes.AddRange(BitConverter.GetBytes(data));
        }

        void Write(ushort data) {
            bytes.AddRange(BitConverter.GetBytes(data));
        }

        void Write(bool data) {
            bytes.AddRange(BitConverter.GetBytes(data));
        }

        void Write(byte data) {
            bytes.Add(data);
        }

        void Write(string data) {
            bytes.AddRange(Encoding.ASCII.GetBytes(data));
        }
        
        internal byte[] GetBytesToSend() {
            return bytes.ToArray();
        }

    }

    class Client {

        Connection ConnectedServer;

        public Client() {
            ConnectedServer.Set("localhost", 12345);

            List<ArraySegment<byte>> buffer = new List<ArraySegment<byte>>();
            int bytes = ConnectedServer.ConnectionSocket.Receive(buffer);
        }

        public void Send(Message msg) {
            ConnectedServer.Send(msg);
        }

    }

}
