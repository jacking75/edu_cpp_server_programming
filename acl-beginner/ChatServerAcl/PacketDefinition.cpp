#include "PacketDefinition.h"

void PacketHeader::Serialize(std::span<std::byte> buffer) const {
    auto it = buffer.begin();
    std::memcpy(&*it, &TotalSize, sizeof(TotalSize));
    std::memcpy(&*(it + sizeof(TotalSize)), &Id, sizeof(Id));
    std::memcpy(&*(it + sizeof(TotalSize) + sizeof(Id)), &Type, sizeof(Type));
}

PacketHeader PacketHeader::Deserialize(std::span<const std::byte> buffer) {
    PacketHeader header;
    auto it = buffer.begin();
    std::memcpy(&header.TotalSize, &*it, sizeof(header.TotalSize));
    std::memcpy(&header.Id, &*(it + sizeof(header.TotalSize)), sizeof(header.Id));
    std::memcpy(&header.Type, &*(it + sizeof(header.TotalSize) + sizeof(header.Id)), sizeof(header.Type));
    return header;
}

void LoginRequest::Serialize(std::span<std::byte> buffer) const {
    PacketHeader::Serialize(buffer);
    std::memcpy(buffer.data() + sizeof(PacketHeader), UserID.data(), UserID.size());
    std::memcpy(buffer.data() + sizeof(PacketHeader) + UserID.size(), AuthToken.data(), AuthToken.size());
}

LoginRequest LoginRequest::Deserialize(std::span<const std::byte> buffer) {
    LoginRequest request;
    auto it = buffer.begin();
    std::memcpy(&request.TotalSize, &*it, sizeof(request.TotalSize));
    std::memcpy(&request.Id, &*(it + sizeof(request.TotalSize)), sizeof(request.Id));
    std::memcpy(&request.Type, &*(it + sizeof(request.TotalSize) + sizeof(request.Id)), sizeof(request.Type));
    std::memcpy(request.UserID.data(), buffer.data() + sizeof(PacketHeader), request.UserID.size());
    std::memcpy(request.AuthToken.data(), buffer.data() + sizeof(PacketHeader) + request.UserID.size(), request.AuthToken.size());
    return request;
}

void RoomEnterRequest::Serialize(std::span<std::byte> buffer) const {
    PacketHeader::Serialize(buffer);
    std::memcpy(buffer.data() + sizeof(PacketHeader), &RoomNumber, sizeof(RoomNumber));
}

RoomEnterRequest RoomEnterRequest::Deserialize(std::span<const std::byte> buffer) {
    RoomEnterRequest request;
    auto it = buffer.begin();
    std::memcpy(&request.TotalSize, &*it, sizeof(request.TotalSize));
    std::memcpy(&request.Id, &*(it + sizeof(request.TotalSize)), sizeof(request.Id));
    std::memcpy(&request.Type, &*(it + sizeof(request.TotalSize) + sizeof(request.Id)), sizeof(request.Type));
    std::memcpy(&request.RoomNumber, buffer.data() + sizeof(PacketHeader), sizeof(request.RoomNumber));
    return request;
}

void UserListNotification::Serialize(std::span<std::byte> buffer) const {
    PacketHeader::Serialize(buffer);
    std::memcpy(buffer.data() + sizeof(PacketHeader), UserID1.data(), UserID1.size());
    std::memcpy(buffer.data() + sizeof(PacketHeader) + UserID1.size(), UserID2.data(), UserID2.size());
}

UserListNotification UserListNotification::Deserialize(std::span<const std::byte> buffer) {
    UserListNotification notification;
    auto it = buffer.begin();
    std::memcpy(&notification.TotalSize, &*it, sizeof(notification.TotalSize));
    std::memcpy(&notification.Id, &*(it + sizeof(notification.TotalSize)), sizeof(notification.Id));
    std::memcpy(&notification.Type, &*(it + sizeof(notification.TotalSize) + sizeof(notification.Id)), sizeof(notification.Type));
    std::memcpy(notification.UserID1.data(), buffer.data() + sizeof(PacketHeader), notification.UserID1.size());
    std::memcpy(notification.UserID2.data(), buffer.data() + sizeof(PacketHeader) + notification.UserID1.size(), notification.UserID2.size());
    return notification;
}

void RoomChatRequest::Serialize(std::span<std::byte> buffer) const {
    PacketHeader::Serialize(buffer);
    std::memcpy(buffer.data() + sizeof(PacketHeader), Message.data(), Message.size());
}

RoomChatRequest RoomChatRequest::Deserialize(std::span<const std::byte> buffer) {
    RoomChatRequest request{};
    auto it = buffer.begin();
    std::memcpy(&request.TotalSize, &*it, sizeof(request.TotalSize));
    std::memcpy(&request.Id, &*(it + sizeof(request.TotalSize)), sizeof(request.Id));
    std::memcpy(&request.Type, &*(it + sizeof(request.TotalSize) + sizeof(request.Id)), sizeof(request.Type));
    std::memcpy(request.Message.data(), buffer.data() + sizeof(PacketHeader), request.Message.size());
    return request;
}

void RoomChatNotification::Serialize(std::span<std::byte> buffer) const {
    PacketHeader::Serialize(buffer);
    std::memcpy(buffer.data() + sizeof(PacketHeader), UserID.data(), UserID.size());
    std::memcpy(buffer.data() + sizeof(PacketHeader) + UserID.size(), Message.data(), Message.size());
}

RoomChatNotification RoomChatNotification::Deserialize(std::span<const std::byte> buffer) {
    RoomChatNotification notification{};
    auto it = buffer.begin();
    std::memcpy(&notification.TotalSize, &*it, sizeof(notification.TotalSize));
    std::memcpy(&notification.Id, &*(it + sizeof(notification.TotalSize)), sizeof(notification.Id));
    std::memcpy(&notification.Type, &*(it + sizeof(notification.TotalSize) + sizeof(notification.Id)), sizeof(notification.Type));
    std::memcpy(notification.UserID.data(), buffer.data() + sizeof(PacketHeader), notification.UserID.size());
    std::memcpy(notification.Message.data(), buffer.data() + sizeof(PacketHeader) + notification.UserID.size(), notification.Message.size());
    return notification;
}
