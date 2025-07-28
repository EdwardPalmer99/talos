/**
 * @file ExchangeApp.cpp
 * @author Edward Palmer
 * @date 2025-07-28
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "Exchange.hpp"

int main(void)
{
    Exchange exchange(8081, 8080); /* TODO: - put these connections somewhere */
    exchange.start();
    return 0;
}