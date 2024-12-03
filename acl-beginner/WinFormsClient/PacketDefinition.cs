using System.Text;

namespace WinFormsClient;

internal class PacketDefinition
{
    public enum PacketID : ushort
    {
        ReqLogin = 1002,
        ResLogin = 1003,
        ReqRoomEnter = 1021,
        ResRoomEnter = 1022,
        NtfUserList = 2024,
        ReqRoomChat = 1026,
        NtfRoomChat = 1027,
    }

    public class PacketHeader
    {
        public ushort TotalSize { get; set; }
        public PacketID Id { get; set; }
        public byte Type { get; set; }

        public byte[] Serialize()
        {
            byte[] buffer = new byte[6];
            BitConverter.GetBytes(TotalSize).CopyTo(buffer, 0); // TotalSize는 전체 패킷 크기를 포함
            BitConverter.GetBytes((ushort)Id).CopyTo(buffer, 2);
            buffer[4] = Type;
            return buffer;
        }

        public static PacketHeader Deserialize(byte[] buffer)
        {
            return new PacketHeader
            {
                TotalSize = BitConverter.ToUInt16(buffer, 0),
                Id = (PacketID)BitConverter.ToUInt16(buffer, 2),
                Type = buffer[4]
            };
        }
    }

    public class LoginRequest : PacketHeader
    {
        public string UserID { get; set; }
        public string AuthToken { get; set; }

        public byte[] Serialize()
        {
            byte[] headerBuffer = base.Serialize();
            byte[] userIdBuffer = Encoding.UTF8.GetBytes(UserID.PadRight(32, '\0'));
            byte[] authTokenBuffer = Encoding.UTF8.GetBytes(AuthToken.PadRight(32, '\0'));

            byte[] buffer = new byte[headerBuffer.Length + userIdBuffer.Length + authTokenBuffer.Length];
            Array.Copy(headerBuffer, 0, buffer, 0, headerBuffer.Length);
            Array.Copy(userIdBuffer, 0, buffer, headerBuffer.Length, userIdBuffer.Length);
            Array.Copy(authTokenBuffer, 0, buffer, headerBuffer.Length + userIdBuffer.Length, authTokenBuffer.Length);

            return buffer;
        }

        public static LoginRequest Deserialize(byte[] buffer)
        {
            return new LoginRequest
            {
                TotalSize = BitConverter.ToUInt16(buffer, 0),
                Id = (PacketID)BitConverter.ToUInt16(buffer, 2),
                Type = buffer[4],
                UserID = Encoding.UTF8.GetString(buffer, 6, 32).TrimEnd('\0'),
                AuthToken = Encoding.UTF8.GetString(buffer, 38, 32).TrimEnd('\0')
            };
        }
    }

    public class RoomEnterRequest : PacketHeader
    {
        public int RoomNumber { get; set; }

        public byte[] Serialize()
        {
            byte[] headerBuffer = base.Serialize();
            byte[] roomNumberBuffer = BitConverter.GetBytes(RoomNumber);

            byte[] buffer = new byte[headerBuffer.Length + roomNumberBuffer.Length];
            Array.Copy(headerBuffer, 0, buffer, 0, headerBuffer.Length);
            Array.Copy(roomNumberBuffer, 0, buffer, headerBuffer.Length, roomNumberBuffer.Length);

            return buffer;
        }

        public static RoomEnterRequest Deserialize(byte[] buffer)
        {
            return new RoomEnterRequest
            {
                TotalSize = BitConverter.ToUInt16(buffer, 0),
                Id = (PacketID)BitConverter.ToUInt16(buffer, 2),
                Type = buffer[4],
                RoomNumber = BitConverter.ToInt32(buffer, 6)
            };
        }
    }

    public class RoomChatRequest : PacketHeader
    {
        public string Message { get; set; }

        public byte[] Serialize()
        {
            byte[] headerBuffer = base.Serialize();
            byte[] messageBuffer = Encoding.UTF8.GetBytes(Message);

            // TotalSize는 고정된 헤더 크기(6바이트) + 가변 메시지 크기
            TotalSize = (ushort)(headerBuffer.Length + messageBuffer.Length);

            byte[] buffer = new byte[TotalSize];
            Array.Clear(buffer, 0, buffer.Length); // 이전 데이터 제거

            Array.Copy(headerBuffer, 0, buffer, 0, headerBuffer.Length);
            Array.Copy(messageBuffer, 0, buffer, headerBuffer.Length, messageBuffer.Length);

            return buffer;
        }

        public static RoomChatRequest Deserialize(byte[] buffer)
        {
            var request = new RoomChatRequest
            {
                TotalSize = BitConverter.ToUInt16(buffer, 0),
                Id = (PacketID)BitConverter.ToUInt16(buffer, 2),
                Type = buffer[4]
            };

            // 문자열 데이터를 정확하게 추출 (널 문자 제거)
            int messageLength = request.TotalSize - 6;
            request.Message = Encoding.UTF8.GetString(buffer, 6, messageLength).TrimEnd('\0');

            return request;
        }
    }

    public class RoomChatNotification : PacketHeader
    {
        public string UserID { get; set; }
        public string Message { get; set; }

        public byte[] Serialize()
        {
            byte[] headerBuffer = base.Serialize();
            byte[] userIdBuffer = Encoding.UTF8.GetBytes(UserID.PadRight(32, '\0'));
            byte[] messageBuffer = Encoding.UTF8.GetBytes(Message);

            byte[] buffer = new byte[headerBuffer.Length + userIdBuffer.Length + messageBuffer.Length];
            Array.Copy(headerBuffer, 0, buffer, 0, headerBuffer.Length);
            Array.Copy(userIdBuffer, 0, buffer, headerBuffer.Length, userIdBuffer.Length);
            Array.Copy(messageBuffer, 0, buffer, headerBuffer.Length + userIdBuffer.Length, messageBuffer.Length);

            return buffer;
        }

        public static RoomChatNotification Deserialize(byte[] buffer)
        {
            var notification = new RoomChatNotification
            {
                TotalSize = BitConverter.ToUInt16(buffer, 0),
                Id = (PacketID)BitConverter.ToUInt16(buffer, 2),
                Type = buffer[4],
            };

            notification.UserID = Encoding.UTF8.GetString(buffer, 6, 32).TrimEnd('\0');
            int messageLength = notification.TotalSize - 6 - 32; // 패킷 크기에서 헤더와 UserID 크기를 뺀 값이 메시지 크기
            notification.Message = Encoding.UTF8.GetString(buffer, 38, messageLength).TrimEnd('\0');

            return notification;
        }

    }

    public class UserListNotification : PacketHeader
    {
        public string UserID1 { get; set; }
        public string UserID2 { get; set; }

        public byte[] Serialize()
        {
            byte[] headerBuffer = base.Serialize();
            byte[] userId1Buffer = Encoding.UTF8.GetBytes(UserID1.PadRight(32, '\0'));
            byte[] userId2Buffer = Encoding.UTF8.GetBytes(UserID2.PadRight(32, '\0'));

            byte[] buffer = new byte[headerBuffer.Length + userId1Buffer.Length + userId2Buffer.Length];
            Array.Copy(headerBuffer, 0, buffer, 0, headerBuffer.Length);
            Array.Copy(userId1Buffer, 0, buffer, headerBuffer.Length, userId1Buffer.Length);
            Array.Copy(userId2Buffer, 0, buffer, headerBuffer.Length + userId1Buffer.Length, userId2Buffer.Length);

            return buffer;
        }

        public static UserListNotification Deserialize(byte[] buffer)
        {
            var notification = new UserListNotification
            {
                TotalSize = BitConverter.ToUInt16(buffer, 0),
                Id = (PacketID)BitConverter.ToUInt16(buffer, 2),
                Type = buffer[4],
            };

            notification.UserID1 = Encoding.UTF8.GetString(buffer, 6, 32).TrimEnd('\0');
            notification.UserID2 = Encoding.UTF8.GetString(buffer, 38, 32).TrimEnd('\0');

            return notification;
        }
    }
}
