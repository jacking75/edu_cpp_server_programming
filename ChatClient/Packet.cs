using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace csharp_test_client
{
    struct PacketData
    {
        public Int16 DataSize;
        public Int16 PacketID;
        public SByte Type;
        public byte[] BodyData;
    }

    public class PacketDump
    {
        public static string Bytes(byte[] byteArr)
        {
            StringBuilder sb = new StringBuilder("[");
            for (int i = 0; i < byteArr.Length; ++i)
            {
                sb.Append(byteArr[i] + " ");
            }
            sb.Append("]");
            return sb.ToString();
        }
    }
    

    public class ErrorNtfPacket
    {
        public ERROR_CODE Error;

        public bool FromBytes(byte[] bodyData)
        {
            Error = (ERROR_CODE)BitConverter.ToInt16(bodyData, 0);
            return true;
        }
    }
    

    public class LoginReqPacket
    {
        byte[] UserID = new byte[PacketDef.MAX_USER_ID_BYTE_LENGTH];
        byte[] UserPW = new byte[PacketDef.MAX_USER_PW_BYTE_LENGTH];

        public void SetValue(string userID, string userPW)
        {
            Encoding.UTF8.GetBytes(userID).CopyTo(UserID, 0);
            Encoding.UTF8.GetBytes(userPW).CopyTo(UserPW, 0);
        }

        public byte[] ToBytes()
        {
            List<byte> dataSource = new List<byte>();
            dataSource.AddRange(UserID);
            dataSource.AddRange(UserPW);
            return dataSource.ToArray();
        }
    }
    
    public class LoginResPacket
    {
        public Int16 Result;

        public bool FromBytes(byte[] bodyData)
        {
            Result = BitConverter.ToInt16(bodyData, 0);
            return true;
        }
    }


    public class RoomEnterReqPacket
    {
        Int16 RoomNumber;
        public void SetValue(Int16 roomNumber)
        {
            RoomNumber = roomNumber;
        }

        public byte[] ToBytes()
        {
            List<byte> dataSource = new List<byte>();
            dataSource.AddRange(BitConverter.GetBytes(RoomNumber));
            return dataSource.ToArray();
        }
    }

    public class RoomEnterResPacket
    {
        public Int16 Result;

        public bool FromBytes(byte[] bodyData)
        {
        
            Result = BitConverter.ToInt16(bodyData, 0);
            return true;
        }
    }

    public class RoomUserListNtfPacket
    {
        public int UserCount = 0;
        public List<Int64> UserUniqueIdList = new List<Int64>();
        public List<string> UserIDList = new List<string>();

        public bool FromBytes(byte[] bodyData)
        {
            var readPos = 0;
            var userCount = (SByte)bodyData[readPos];
            ++readPos;

            for (int i = 0; i < userCount; ++i)
            {
                var uniqeudId = BitConverter.ToInt64(bodyData, readPos);
                readPos += 8;

                var idlen = (SByte)bodyData[readPos];
                ++readPos;

                var id = Encoding.UTF8.GetString(bodyData, readPos, idlen);
                readPos += idlen;

                UserUniqueIdList.Add(uniqeudId);
                UserIDList.Add(id);
            }

            UserCount = userCount;
            return true;
        }
    }

    public class RoomNewUserNtfPacket
    {
        public Int64 UserUniqueId;
        public string UserID;

        public bool FromBytes(byte[] bodyData)
        {
       
            var readPos = 0;

            UserUniqueId = BitConverter.ToInt64(bodyData, readPos);
            readPos += 8;

            var idlen = (SByte)bodyData[readPos];
            ++readPos;

            UserID = Encoding.UTF8.GetString(bodyData, readPos, idlen);
            readPos += idlen;

            return true;
        }
    }

    public class MatchUserResPacket
    {
        public Int16 Result;
     
        public bool FromBytes(byte[] bodyData)
        {
            Result = BitConverter.ToInt16(bodyData, 0);
            return true;
        }
    }

    public class GameResultResPacket
    {
        public Int16 Result;
        public string UserID;
        public bool FromBytes(byte[] bodyData)
        {
            Result = BitConverter.ToInt16(bodyData, 0);

            var userLen = PacketDef.MAX_USER_ID_BYTE_LENGTH;
            byte[] userIdTemp = new byte[userLen];
            Buffer.BlockCopy(bodyData, 2, userIdTemp, 0, userLen);
            UserID = Encoding.GetEncoding(949).GetString(userIdTemp);
            return true;
        }
    }
    public class PutStoneReqPacket
    {
        int xPos;
        int yPos;
        public void SetValue(int x, int y)
        {
            xPos = x;
            yPos = y;
        }

        public byte[] ToBytes()
        {
            List<byte> dataSource = new List<byte>();
            dataSource.AddRange(BitConverter.GetBytes(xPos));
            dataSource.AddRange(BitConverter.GetBytes(yPos));
            return dataSource.ToArray();
        }
    }

    public class PutStoneResPacket
    {
        public Int16 Result;
        public string UserID;
        public bool FromBytes(byte[] bodyData)
        {
            Result = BitConverter.ToInt16(bodyData, 0);

            var userLen = PacketDef.MAX_USER_ID_BYTE_LENGTH;
            byte[] userIdTemp = new byte[userLen];
            Buffer.BlockCopy(bodyData, 2, userIdTemp, 0, userLen);
            UserID = Encoding.GetEncoding(949).GetString(userIdTemp);

            return true;
        }
    }

    public class GameStartResPacket
    {
        public Int16 Result;
        public string UserID;
        public bool FromBytes(byte[] bodyData)
        {
            Result = BitConverter.ToInt16(bodyData, 0);

            var userLen = PacketDef.MAX_USER_ID_BYTE_LENGTH;
            byte[] userIdTemp = new byte[userLen];
            Buffer.BlockCopy(bodyData, 2, userIdTemp, 0, userLen);
            UserID = Encoding.GetEncoding(949).GetString(userIdTemp);
          
            return true;
        }
    }

    public class RoomChatReqPacket
    {
        byte[] Msg = new byte[PacketDef.MAX_ROOM_CHAT_MSG_SIZE + 1];

        public void SetValue(string message)
        {
            Msg = Encoding.UTF8.GetBytes(message);
        }

        public byte[] ToBytes()
        {
            List<byte> dataSource = new List<byte>();
            dataSource.AddRange(Msg);
            return dataSource.ToArray();
        }
    }

    public class RoomChatResPacket
    {
        public Int16 Result;
        
        public bool FromBytes(byte[] bodyData)
        {
            Result = BitConverter.ToInt16(bodyData, 0);
            return true;
        }
    }

    public class RoomChatNtfPacket
    {
        public string UserID;
        public string Message;

        public bool FromBytes(byte[] bodyData)
        {
            var userLen = PacketDef.MAX_USER_ID_BYTE_LENGTH;
            byte[] userIdTemp = new byte[userLen];
            Buffer.BlockCopy(bodyData, 0, userIdTemp, 0, userLen);
            UserID = Encoding.GetEncoding(949).GetString(userIdTemp);
            //UserID = new string(Encoding.GetEncoding(949).GetChars(userIdTemp));

            var msgLen = PacketDef.MAX_ROOM_CHAT_MSG_SIZE;
            byte[] messageTemp = new byte[msgLen];
            Buffer.BlockCopy(bodyData, PacketDef.MAX_USER_ID_BYTE_LENGTH + 1, messageTemp, 0, msgLen);
            Message = Encoding.GetEncoding(949).GetString(messageTemp);
           // Message = new string(Encoding.GetEncoding(949).GetChars(messageTemp));
           return true;
        }
    }

    /// ///////////////////////////////////////////////
   public class PutStoneNtfPacket
    {
        public Int16 Result;
        public string UserID;

        public bool FromBytes(byte[] bodyData)
        {
            Result = BitConverter.ToInt16(bodyData, 0);

            var userLen = PacketDef.MAX_USER_ID_BYTE_LENGTH;
            byte[] userIdTemp = new byte[userLen];
            Buffer.BlockCopy(bodyData, 0, userIdTemp, 0, userLen);
            UserID = Encoding.GetEncoding(949).GetString(userIdTemp);

            return true;
        }
    }

    public class RoomLeaveResPacket
    {
        public Int16 Result;
        
        public bool FromBytes(byte[] bodyData)
        {
            Result = BitConverter.ToInt16(bodyData, 0);
            return true;
        }
    }

    public class RoomLeaveUserNtfPacket
    {
        public Int64 UserUniqueId;

        public bool FromBytes(byte[] bodyData)
        {
            UserUniqueId = BitConverter.ToInt64(bodyData, 0);
            return true;
        }
    }


    
    public class RoomRelayNtfPacket
    {
        public Int64 UserUniqueId;
        public byte[] RelayData;

        public bool FromBytes(byte[] bodyData)
        {
            UserUniqueId = BitConverter.ToInt64(bodyData, 0);

            var relayDataLen = bodyData.Length - 8;
            RelayData = new byte[relayDataLen];
            Buffer.BlockCopy(bodyData, 8, RelayData, 0, relayDataLen);
            return true;
        }
    }


    public class PingRequest
    {
        public Int16 PingNum;

        public byte[] ToBytes()
        {
            return BitConverter.GetBytes(PingNum);
        }

    }
}
