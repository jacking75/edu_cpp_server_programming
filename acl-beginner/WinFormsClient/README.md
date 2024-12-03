# WinFormsClient

이 프로젝트는 C# WinForms 기반의 채팅 클라이언트로, 서버와의 소켓 통신을 통해 채팅을 주고받을 수 있습니다. <br>
서버와의 통신은 C++로 구현된 채팅 서버를 대상으로 하며, 패킷 직렬화 및 역직렬화 방식으로 데이터를 주고받습니다.

## 프로젝트 구조

```
WinFormsClient/
├── Form1.cs               # 클라이언트 메인 UI 및 네트워크 로직 구현
├── PacketDefinition.cs    # 서버와 클라이언트 간 통신에 사용되는 패킷 직렬화/역직렬화 정의
└── Program.cs             # 프로그램 진입점
```

## 각 파일의 역할

### 1. `Form1.cs`
- **클라이언트 메인 로직과 UI 처리**를 담당합니다.
- 사용자가 서버에 접속하고 로그인하며 채팅 메시지를 주고받는 기능을 구현합니다.
- UI에서 버튼 클릭 시 서버로 데이터를 전송하고, 서버로부터 데이터를 수신하면 리스트에 채팅 메시지 또는 시스템 메시지를 표시합니다.

#### 주요 기능
1. **서버 연결 및 로그인**
   - 서버에 연결 후, 로그인 요청을 보내고 응답을 처리합니다.
   - `TcpClient`와 `NetworkStream`을 사용하여 서버와의 TCP 연결을 유지합니다.
   - 로그인 성공/실패 메시지를 수신하고 표시합니다.

2. **채팅방 입장**
   - 사용자가 특정 방에 입장할 수 있습니다. 방 입장 요청을 서버로 보내고, 성공하면 채팅방에 참여할 수 있습니다.

3. **채팅 메시지 송수신**
   - 사용자가 메시지를 입력하고 서버로 전송하면, 해당 메시지를 같은 방에 있는 모든 사용자에게 브로드캐스트합니다.
   - 서버에서 수신된 메시지를 클라이언트의 리스트박스에 추가하여 표시합니다.
   - 패킷 수신 시 메시지와 유저 ID의 끝에 있는 널 문자를 제거하여 클린한 데이터로 표시합니다.

```csharp
private void btnSendChat_Click(object sender, EventArgs e) {
    // 메시지 전송 로직
    var chatRequest = new RoomChatRequest {
        TotalSize = (ushort)(6 + message.Length),
        Id = PacketID.ReqRoomChat,
        Message = message
    };

    byte[] packetData = chatRequest.Serialize();
    _stream.Write(packetData, 0, packetData.Length);
    AddLogMessage($"채팅 메시지 전송: {message}");
}
```

4. **데이터 수신 및 처리**
   - 서버로부터 오는 패킷 데이터를 비동기적으로 수신하고 처리합니다. 수신된 패킷의 ID에 따라 채팅 메시지 또는 유저 목록을 처리하는 로직이 나뉩니다.

```csharp
private void ProcessReceivedData(byte[] buffer) {
    PacketHeader header = PacketHeader.Deserialize(buffer);

    switch (header.Id) {
        case PacketID.NtfRoomChat:
            // 채팅 메시지 처리
            break;
        case PacketID.NtfUserList:
            // 유저 목록 처리
            break;
        default:
            // 기타 처리
            break;
    }
}
```

### 2. `PacketDefinition.cs`
- **서버와 클라이언트 간 통신에 사용되는 패킷 직렬화 및 역직렬화 로직**을 정의합니다.  
- 각 패킷은 `Serialize` 메서드를 통해 바이너리 데이터로 직렬화되며,
- `Deserialize` 메서드를 통해 수신된 바이너리 데이터를 객체로 복원합니다.

#### 주요 클래스
1. **PacketHeader**
   - 모든 패킷의 기본 헤더로, `TotalSize`, `Id`, `Type` 필드를 포함합니다.
   - 이 헤더는 각 패킷의 시작 부분에 위치하며, 패킷의 크기와 종류를 식별합니다.

2. **LoginRequest, RoomEnterRequest, RoomChatRequest**
   - 로그인, 방 입장, 채팅 메시지를 처리하기 위한 패킷 클래스입니다.
   - 각 클래스는 `Serialize`와 `Deserialize` 메서드를 통해 데이터를 바이너리로 변환하거나, 수신된 데이터를 복원합니다.

3. **RoomChatNotification**
   - 서버가 클라이언트에게 브로드캐스트하는 채팅 메시지를 처리하기 위한 패킷입니다.
   - 이 패킷을 통해 서버는 모든 클라이언트에게 특정 사용자의 메시지를 전송합니다.


### 3. `Program.cs`
- 프로그램의 진입점을 정의합니다.
- `Application.Run(new Form1());`를 통해 WinForms 애플리케이션의 메인 폼을 실행합니다.

## 주요 기능 설명

### 1. 서버 연결 및 로그인
- 클라이언트는 서버에 연결된 후, 사용자가 입력한 `UserID`와 `AuthToken`을 서버로 전송하여 로그인을 시도합니다. 
- 서버는 Redis를 이용해 인증을 처리하고 결과를 반환합니다.

### 2. 채팅 기능
- 사용자가 채팅방에 입장한 후 채팅 메시지를 전송할 수 있습니다. 
- 메시지는 직렬화되어 서버로 전송되며, 서버는 해당 방에 있는 모든 사용자에게 메시지를 브로드캐스트합니다. 
- 수신된 메시지는 클라이언트의 리스트박스에 추가됩니다.

### 3. 패킷 직렬화 및 역직렬화 기법
- 서버와 클라이언트 간 데이터를 주고받기 위해 패킷을 정의하고 이를 직렬화하여 바이너리 형식으로 전송합니다.
- 패킷에는 메시지, 유저 ID 등의 데이터가 포함되며, 각 필드의 크기가 고정되어 있어 패킷 구조가 간단하면서도 안정적으로 처리됩니다.

  ```csharp
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
  ```

## 결론
이 WinForms 기반의 채팅 클라이언트는 서버와의 TCP 통신을 통해 실시간 채팅을 구현하였습니다. <br>
Redis를 사용한 로그인, 채팅방 입장 및 메시지 전송 등의 주요 기능이 포함되어 있으며, <br>
패킷 직렬화/역직렬화 기법을 통해 C++로 구현된 서버와의 원활한 통신을 실현합니다. <br>



## 실습 캡쳐

채팅 프로젝트의 실행 결과를 캡처한 화면입니다:

![실습 캡쳐 이미지](../Document/img/ChatServerExample.PNG) 
