/**
 * @file ConnectionManager.hpp
 * @author Edward Palmer
 * @date 2025-08-11
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include <atomic>
#include <condition_variable>
#include <queue>
#include <shared_mutex>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>


class ConnectionManager
{
public:
    using Port = uint16_t;
    using SocketFD = int; /* Socket file descriptor (FD) */
    using Message = std::string;

    // TODO: - add retry loop if cannot immediately connect
    // TODO: - add a broadcast method to send a message to all connections
    // TODO: - implement MsgSequenceNo for incoming/outgoing connections and use to check

    virtual ~ConnectionManager();

    bool connectToServer(Port serverPort);
    void sendMessage(Message message, SocketFD socket);

    /* Starts the server; nonblocking */
    void start();

    /* Shutdown the server; nonblocking */
    void stop();

    /* Waits for server completion; blocking */
    void wait();

protected:
    ConnectionManager() = default;
    ConnectionManager(const ConnectionManager &) = delete;
    ConnectionManager &operator=(const ConnectionManager &) = delete;

    /* Hooks to be implemented by subclasses if required */
    virtual void onStartup() {}
    virtual void onShutdown() {}
    virtual void onWait() {}

    /* Called when we receive a message from a client or server */
    virtual void handleMessage(Message message, SocketFD fromSocket) = 0;

    /* Port <--> Socket mappings */
    class PortSocketMappings
    {
    public:
        void clear();
        void update(Port port, SocketFD socket);
        void erase(SocketFD socket);

        /* Returns (-1) if not found */
        SocketFD getSocket(Port port) const;

        /* Returns a set of all active connections */
        std::vector<SocketFD> getSockets() const;

        /* Returns 0 if not found (special port) */
        Port getPort(SocketFD socket) const;

    private:
        mutable std::shared_mutex _portSocketMutex;
        std::unordered_map<Port, SocketFD> _portToSocket;
        std::unordered_map<SocketFD, Port> _socketToPort;
    };

    struct ClientSession
    {
        ClientSession() = delete;
        ClientSession(SocketFD clientSocket) : clientSocket(clientSocket) {}

        /* Deleted copy constructors */
        ClientSession(const ClientSession &) = delete;
        ClientSession &operator=(const ClientSession &) = delete;

        SocketFD clientSocket;
        std::atomic<bool> active{false};

        std::mutex outgoingMutex;
        std::condition_variable outgoingCV;
        std::queue<Message> outgoingMsgQueue;

        std::thread connectionThread;
        std::thread senderThread;
    };

    void addClientSession(SocketFD clientSocket);
    void closeClientSession(ClientSession &session);
    void removeClientSession(SocketFD clientSocket);

    void closeSocket(SocketFD socket);

    PortSocketMappings _portSocketMappings;
    std::atomic<bool> _active{false};

private:
    using ClientMessage = std::pair<Message, SocketFD>;

    /* Receive from client. One per connection */
    void connectionLoop(ClientSession &clientSocket);

    /* Send to client. One per connection */
    void senderLoop(ClientSession &clientSocket);

    /* Process incoming messages. One per server */
    void handleMessageLoop();

    /* Outgoing message queues */
    std::shared_mutex _clientSessionMutex; /* NB: note the shared mutex */
    std::unordered_map<SocketFD, std::unique_ptr<ClientSession>> _clientSessionMap;

    /* Single incoming message queue */
    std::condition_variable _incomingMsgQueueCV;
    std::mutex _incomingMsgQueueMutex;
    std::queue<ClientMessage> _incomingMsgQueue;

    /* Server threads */
    std::thread _handleMessageLoopThread;
};