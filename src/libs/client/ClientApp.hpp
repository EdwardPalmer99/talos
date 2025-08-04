/**
 * @file ClientApp.hpp
 * @author Edward Palmer
 * @date 2025-07-29
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "App.hpp"
#include "socket/FixClient.hpp"

/* TODO: _ get rid of App class */
/* TODO: - COMPLETELY REWRITE and base on the FixServer but simplify */
/**
 * Simulates a front-office application for traders to book, amend, cancel trades
 */
class ClientApp : public App
{
public:
    ClientApp(int serverPort, std::size_t delayMS = 100); /* Port to connect to; delay between sending messages */

protected:
    void appLoop() final;

    /* TODO: - handle received messages here (i.e. from the OMEngine) --> Print Update for status to logs */

private:
    FixClient _fixClient;
    FixMessage _dummyFix;
    const std::size_t _delayMS;
};