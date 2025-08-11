/**
 * @file FixClient.hpp
 * @author Edward Palmer
 * @date 2025-07-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "Client.hpp"
#include "FixEndpoint.hpp"

using FixClient = FixEndpoint<Client>;
