/**
 * @file UUID.hpp
 * @author Edward Palmer
 * @date 2025-07-28
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include <mutex>
#include <random>
#include <string>

/* TODO: - set the seed */
/* Generator singleton for UUIDs */
class UUID
{
public:
    static UUID &instance()
    {
        static UUID generator;
        return generator;
    }

    /* Thread-safe generator */
    std::string generate(std::size_t n = 15);

protected:
    UUID() = default;
    UUID &operator=(const UUID &) = delete;
    UUID(const UUID &) = delete;

private:
    /* Not thread-safe */
    char randomizeChar();

    std::mt19937 _generator{24}; /* Seeded */
    std::uniform_int_distribution<> _distribution{'a', 'z'};

    std::mutex _mutex; /* Lock. Use a single UUID to ensure uniqueness across a trading day */
};