/**
 * @file UUID.cpp
 * @author Edward Palmer
 * @date 2025-07-28
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "UUID.hpp"
#include <iostream>

std::string UUID::generate(std::size_t n)
{
    if (n == 0)
    {
        return "";
    }

    std::string uuid;
    uuid.resize(n);

    /* TODO: - profile. This may be a bottlekneck. We could create one per thread */
    { /* Thread-safe block */
        std::lock_guard<std::mutex> guard(_mutex);

        for (std::size_t i = 0; i != n; ++i)
        {
            uuid[i] = randomizeChar();
        }
    }

    return uuid;
}


char UUID::randomizeChar()
{
    char next = (char)_distribution(_generator);
    return next;
}
