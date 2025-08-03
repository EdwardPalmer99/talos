/**
 * @file Server.hpp
 * @author Edward Palmer
 * @date 2025-07-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <queue>
#include <shared_mutex>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unordered_map>


class Server /* TODO: - implement MsgSequenceNo for incoming/outgoing connections and use to check */
{
public:
    using Port = uint16_t;
    using SocketFD = int; /* Socket file descriptor (FD) */
    using Message = std::string;

    explicit Server(Port port);
    virtual ~Server();

    /* Connect to a downstream server (e.g. exchange, database) */
    bool connectToServer(Port serverPort);

    /* TODO: - add a retry loop if we cannot connect or to reconnect if a downstream connection drops
     * i.e. to retry and send all messages in queue if we lose connection
     */

    /* Accessor for server's port */
    [[nodiscard]] inline Port port() const { return _port; }

    /* Accessor for server's listening socket */
    [[nodiscard]] inline SocketFD listenSocket() const { return _listeningSocket; }

    /* Starts the server; nonblocking */
    void start();

    /* Waits for server completion; blocking */
    void wait();

    /* Shutdown the server; nonblocking */
    void stop();

protected:
    Server() = delete;
    Server(const Server &) = delete;
    Server &operator=(const Server &) = delete;

    /* Called when we receive a message from a client or server */
    virtual void handleMessage(Message clientMessage, SocketFD clientSocket) = 0;

    /* Sends a message back to a client */
    void sendMessage(Message message, SocketFD clientSocket);

    /* Port <--> Socket mappings */
    class PortSocketMappings
    {
    public:
        void clear();
        void update(Port port, SocketFD socket);
        void erase(SocketFD socket);

        /* Returns (-1) if not found */
        SocketFD getSocket(Port port) const;

        /* Returns 0 if not found (special port) */
        Port getPort(SocketFD socket) const;

    private:
        mutable std::shared_mutex _portSocketMutex;
        std::unordered_map<Port, SocketFD> _portToSocket;
        std::unordered_map<SocketFD, Port> _socketToPort;
    };

    PortSocketMappings _portSocketMappings;

private:
    void closeSocket(SocketFD socket);

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

    using ClientMessage = std::pair<Message, SocketFD>;

    /* Accept incoming connections and spawn new connection/sender loops */
    void listenLoop();

    /* Receive from client. One per connection */
    void connectionLoop(ClientSession &clientSocket);

    /* Process incoming messages. One per server */
    void handleMessageLoop();

    /* Send to client. One per connection */
    void senderLoop(ClientSession &clientSocket);

    Port _port;                    /* Server's port */
    SocketFD _listeningSocket{-1}; /* Server socket's file-descriptor (FD). (-1) on error */

    /* Outgoing message queues */
    std::shared_mutex _clientSessionMutex; /* NB: note the shared mutex */
    std::unordered_map<SocketFD, std::unique_ptr<ClientSession>> _clientSessionMap;

    /* Single incoming message queue */
    std::condition_variable _incomingMsgQueueCV;
    std::mutex _incomingMsgQueueMutex;
    std::queue<ClientMessage> _incomingMsgQueue;

    /* Whether server is active */
    std::atomic<bool> _active{false};

    /* Server threads */
    std::thread _handleMessageLoopThread;
    std::thread _listenLoopThread;
};
