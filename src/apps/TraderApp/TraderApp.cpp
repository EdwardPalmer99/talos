/**
 * @file TraderApp.cpp
 * @author Edward Palmer
 * @date 2025-07-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "Fix/FixTag.hpp"
#include "Socket/FixClient.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

/**
 * Front-office application for traders to book, amend, cancel trades
 *
 * ClientApp (new, cancel, correct) ---> OrderManagementSystem
 *
 */
int main(void)
{
    FixClient client(8080);

    /* Construct dummy Fix */
    FixMessage fix;
    fix.setTag(FixTag::MsgType, "8");
    fix.setTag(FixTag::Side, "1");
    fix.setTag(FixTag::Currency, "GBP");
    fix.setTag(FixTag::OrderQty, "1");
    fix.setTag(FixTag::Price, "100.00");
    fix.setTag(FixTag::ClOrdID, "abcdeg"); /* TODO: - randomize ClOrdID */
    /* TODO: - set product, productID, sending time, sender details, trader ID */

    while (true)
    {
        /* TODO: - use a logger here on a separate thread */
        std::cout << "TraderApp sending Fix: [" << fix.toString() << "]" << std::endl;
        client.doSend(fix);
        sleep(1);
    }

    return 0;
}