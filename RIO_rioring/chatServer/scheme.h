//
// Created by newbiediver on 22. 11. 13.
//

#ifndef CHATSERVER_SCHEME_H
#define CHATSERVER_SCHEME_H

constexpr unsigned char req_chat = 1;
constexpr unsigned char resp_chat = 11;

#pragma pack(push, 1)

struct scheme {
    unsigned char msg;
    unsigned short size;
};

struct request_chat : public scheme {
    request_chat() : scheme() { msg = req_chat; size = sizeof( *this ); }

    int chat_size{};
};

struct response_chat : public scheme {
    response_chat() : scheme() { msg = resp_chat; size = sizeof( *this ); }

    char name[20]{};
    int chat_size{};
};

#pragma pack(pop)

#endif //CHATSERVER_SCHEME_H
