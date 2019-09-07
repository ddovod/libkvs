
#pragma once

#include <teenypath.h>

namespace kvs
{
    using namespace TeenyPath;

    bool mkdir(const path& p);
    bool mkdirp(const path& root, const path& p);
    path getcwd();
}
