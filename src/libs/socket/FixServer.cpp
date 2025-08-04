/**
 * @file FixServer.cpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "FixServer.hpp"
#include "logger/Logger.hpp"
#include <iomanip>

FixServer::FixServer(Port serverPort) : Server(serverPort)
{
}


void FixServer::handleMessage(Message clientMessage, SocketFD clientSocket)
{
    /* Construct FIX message and pass down to handleFixMessage() */
    Logger::instance().log("Received FixMsg (source: " + std::to_string(clientSocket) + "): " + clientMessage);
    handleFixMessage(FixMessage(std::move(clientMessage)), clientSocket);
}


void FixServer::sendFixMessage(FixMessage message, SocketFD clientSocket)
{
    enrichFixMessage(message);

    Logger::instance().log("Sent FixMsg (destination: " + std::to_string(clientSocket) + "): " + message.toString());
    sendMessage(message.toString(), clientSocket);
}


void FixServer::enrichFixMessage(FixMessage &fixMsg)
{
    /* TODO: - update the MsgSeqNo for that connection */
    fixMsg.setTag(FixTag::SendingTime, nowUTC());
    fixMsg.setTag(FixTag::SenderSubID, std::to_string(port())); /* TODO: - add option to override this */
}


std::string FixServer::nowUTC() const
{
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;

    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::ostringstream os;

    struct tm currentGMTime; /* gmtime_r is a thread-safe verison */
    os << std::put_time(gmtime_r(&currentTime, &currentGMTime), "%Y%m%d-%H:%M:%S") << "." << std::setw(3) << std::setfill('0') << ms;

    return os.str();
}
