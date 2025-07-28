/**
 * @file FixServer.hpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "Fix/FixMessage.hpp"
#include "Server.hpp"

class FixServer : public Server
{
public:
    FixServer(uint16_t serverPort);

protected:
    /* To be implemented. What to do when we receive a client Fix */
    virtual void handleFixMessage(FixMessage message) = 0;

private:
    void handleClientMessage(std::string message) final;
};
