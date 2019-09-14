
#pragma once

#include <teenypath.h>

namespace kvs::utility
{
    using namespace TeenyPath;

    /**
     * Creates a directory with given path.
     * \param p Path of the directory.
     * \return `true` if directory is created and `false` otherwise.
     */
    bool mkdir(const path& p);

    /**
     * Creates a directories structure.
     * \param root Root path.
     * \param p Path for which all intermediate directories should be created.
     * \return `true` if all directories are created and `false` otherwise.
     */
    bool mkdirp(const path& root, const path& p);

    /** Returns current working directory path. */
    path getcwd();
}
