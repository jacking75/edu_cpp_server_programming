#pragma once
#include <cstdint>
#include <cstring>
#include <array>
#include <span>
#include <bit>

enum class PacketID : uint16_t {
    ReqLogin = 1002,
    ResLogin = 1003,
    ReqRoomEnter = 1021,
    ResRoomEnter = 1022,
    NtfUserList = 2024,
    ReqRoomChat = 1026,
    NtfRoomChat = 1027,
};

struct PacketHeader {
    uint16_t TotalSize;
    PacketID Id;
    uint8_t Type;

    void Serialize(std::span<std::byte> buffer) const;
    [[nodiscard]] static PacketHeader Deserialize(std::span<const std::byte> buffer);
};

struct LoginRequest : public PacketHeader {
    std::array<char, 32> UserID{};
    std::array<char, 32> AuthToken{};

    void Serialize(std::span<std::byte> buffer) const;
    [[nodiscard]] static LoginRequest Deserialize(std::span<const std::byte> buffer);
};

struct RoomEnterRequest : public PacketHeader {
    int RoomNumber;

    void Serialize(std::span<std::byte> buffer) const;
    [[nodiscard]] static RoomEnterRequest Deserialize(std::span<const std::byte> buffer);
};

struct UserListNotification : public PacketHeader {
    std::array<char, 32> UserID1{};
    std::array<char, 32> UserID2{};

    void Serialize(std::span<std::byte> buffer) const;
    [[nodiscard]] static UserListNotification Deserialize(std::span<const std::byte> buffer);
};

struct RoomChatRequest : public PacketHeader {
    std::array<char, 256> Message{};

    RoomChatRequest() : PacketHeader() {
        Message.fill(0);
    }

    void Serialize(std::span<std::byte> buffer) const;
    [[nodiscard]] static RoomChatRequest Deserialize(std::span<const std::byte> buffer);
};

struct RoomChatNotification : public PacketHeader {
    std::array<char, 32> UserID{};
    std::array<char, 256> Message{};

    RoomChatNotification() : PacketHeader() {
        UserID.fill(0);
        Message.fill(0);
    }

    void Serialize(std::span<std::byte> buffer) const;
    [[nodiscard]] static RoomChatNotification Deserialize(std::span<const std::byte> buffer);
};
