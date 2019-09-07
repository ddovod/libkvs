
#pragma once

#include <linear_hashing_table.h>
#include "Record.hpp"

namespace kvs
{
    using HashTable = LinearHashingTable<std::string, Record>;
}
