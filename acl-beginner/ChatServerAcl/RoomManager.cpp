#define _CRT_SECURE_NO_WARNINGS
#include "RoomManager.h"
#include "PacketDefinition.h"
#include <fiber/fiber_mutex.hpp>

static acl::fiber_mutex room_mutex_;

void RoomManager::EnterRoom(int roomNumber, const std::string& userID, acl::socket_stream* conn) {
    room_mutex_.lock();

    if (rooms_[roomNumber].size() >= 2) {
        std::string message = "Room is full!";
        conn->write(message.c_str(), message.size());
        room_mutex_.unlock();
        return;
    }

    rooms_[roomNumber][userID] = conn;
    room_mutex_.unlock();

    SendUserListToAll(roomNumber);
    std::string joinMessage = userID + " has entered the room.";
    BroadcastMessage(roomNumber, joinMessage, "System");
}

void RoomManager::LeaveRoom(int roomNumber, const std::string& userID) {
    room_mutex_.lock();
    rooms_[roomNumber].erase(userID);

    if (rooms_[roomNumber].empty()) {
        rooms_.erase(roomNumber);
    }

    room_mutex_.unlock();

    std::string leaveMessage = userID + " has left the room.";
    BroadcastMessage(roomNumber, leaveMessage, "System");
    SendUserListToAll(roomNumber);
}

void RoomManager::SendUserListToAll(int roomNumber) {
    room_mutex_.lock();

    if (rooms_.find(roomNumber) == rooms_.end()) {
        room_mutex_.unlock();
        return;
    }

    std::string userID1 = "";
    std::string userID2 = "";

    int userIndex = 0;
    for (const auto& pair : rooms_[roomNumber]) {
        if (userIndex == 0) {
            userID1 = pair.first;
        }
        else if (userIndex == 1) {
            userID2 = pair.first;
            break;
        }
        userIndex++;
    }

    room_mutex_.unlock();

    UserListNotification notification;
    notification.TotalSize = sizeof(UserListNotification);
    notification.Id = PacketID::NtfUserList;
    std::strncpy(notification.UserID1.data(), userID1.c_str(), notification.UserID1.size() - 1);
    std::strncpy(notification.UserID2.data(), userID2.c_str(), notification.UserID2.size() - 1);

    std::array<std::byte, sizeof(UserListNotification)> buffer{};
    notification.Serialize(buffer);

    std::cout << "Broadcasting User List: " << userID1 << " " << userID2 << " to all clients." << std::endl;
    Broadcast(roomNumber, buffer);
}

void RoomManager::BroadcastMessage(int roomNumber, const std::string& message, const std::string& senderID) {
    RoomChatNotification notification;
    notification.TotalSize = sizeof(RoomChatNotification);
    notification.Id = PacketID::NtfRoomChat;
    std::strncpy(notification.UserID.data(), senderID.c_str(), notification.UserID.size() - 1);
    std::strncpy(notification.Message.data(), message.c_str(), notification.Message.size() - 1);

    std::array<std::byte, sizeof(RoomChatNotification)> buffer{};
    notification.Serialize(buffer);

    std::cout << "Broadcasting Message: [" << senderID << "] " << message << " to all clients." << std::endl;
    Broadcast(roomNumber, buffer);
}

void RoomManager::Broadcast(int roomNumber, std::span<const std::byte> buffer) {
    room_mutex_.lock();

    if (rooms_.find(roomNumber) == rooms_.end()) {
        room_mutex_.unlock();
        return;
    }

    for (const auto& pair : rooms_[roomNumber]) {
        acl::socket_stream* conn = pair.second;
        const std::string& receiverID = pair.first;

        std::cout << "Broadcasting data to client [" << receiverID << "]." << std::endl;
        conn->write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    }

    room_mutex_.unlock();
}
