#include <iostream>
#include <unordered_set>
#include <rioring.h>

using namespace rioring;
using namespace std::chrono_literals;


class event_receiver {
public:
    event_receiver() = default;
    ~event_receiver() = default;

    void on_accept_client(tcp_socket_ptr& new_socket) {
        // register events for new socket at this time.
        // Normally receive and close events and or error event.
        // If you need sending completion event, you can also register 'set_send_complete_event'.

        new_socket->set_close_event([event = this](auto&& socket) {
            event->on_disconnect_client(std::forward< decltype(socket) >(socket));
            });

        new_socket->set_receive_event([](auto&& socket, auto&& buffer, auto&& addr) {
            event_receiver::on_read_message(std::forward< decltype(socket) >(socket),
            std::forward< decltype(buffer) >(buffer),
            std::forward< decltype(addr) >(addr));
            });

        std::cout << "Connected client " << new_socket->remote_ipv4() << ":" << new_socket->remote_port() << std::endl;

        std::scoped_lock sc{ container_lock };
        activating_sockets.insert(new_socket);
    }

    static void on_read_message(socket_ptr& socket, io_buffer* buffer, sockaddr* addr [[maybe_unused]] ) {
        // just echo
        auto tcp = to_tcp_socket_ptr(socket);
        tcp->send(*(*buffer), buffer->size());

        // pop contents from buffer
        buffer->pop(buffer->size());
    }

    void on_disconnect_client(socket_ptr& socket) {
        std::cout << "Disconnect client " << socket->remote_ipv4() << ":" << socket->remote_port() << std::endl;

        std::scoped_lock sc{ container_lock };
        activating_sockets.erase(socket);
    }

    void leave_clients() {
        std::scoped_lock sc{ container_lock };
        for (auto& socket : activating_sockets) {
            auto tcp = to_tcp_socket_ptr(socket);
            tcp->close();
        }
    }

    void wait() const {
        while (!activating_sockets.empty()) {
            std::this_thread::sleep_for(1ms);
        }
    }

    critical_section container_lock;
    std::unordered_set< socket_ptr > activating_sockets;
};

int main() {
    constexpr unsigned short port = 8000;
    event_receiver receiver;
    std::string in;

#ifdef WIN32
    io_service::initialize_winsock();
#endif

    // Initialize i/o service.
    io_service io;
    if (!io.run(static_cast<int>(std::thread::hardware_concurrency()))) {
        std::cerr << "failed to run io service" << std::endl;
        return 1;
    }
    std::cout << "started io service" << std::endl;

    // Initialize tcp listener.
    tcp_server_ptr server = tcp_server::create(&io);
    if (!server) {
        std::cerr << "failed to create server object" << std::endl;
        return 1;
    }

    // accept event.
    server->set_accept_event([event = &receiver](auto&& new_socket) {
        event->on_accept_client(std::forward< decltype(new_socket)>(new_socket));
        });

    // run this server.
    if (!server->run(port)) {
        std::cerr << "failed to open server port" << std::endl;
        return 1;
    }
    std::cout << "started server. port: " << port << std::endl;

    std::cout << "Enter 'exit' to terminate server" << std::endl;
    while (true) {
        std::cin >> in;
        if (in == "exit") {
            break;
        }
    }

    receiver.leave_clients();
    receiver.wait();

    server->stop();
    io.stop();

#ifdef WIN32
    io_service::deinitialize_winsock();
#endif

    return 0;
}