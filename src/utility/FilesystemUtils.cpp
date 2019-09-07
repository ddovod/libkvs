
#include "FilesystemUtils.hpp"
#include <algorithm>
#include <errno.h>
#include <pwd.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace kvs
{
    bool mkdir(const path& p) { return ::mkdir(p.string().c_str(), S_IRWXU) == 0; }

    bool mkdirp(const path& root, const path& p)
    {
        auto path = p.string();
        auto rootPath = root.string();
        mode_t mode = S_IRWXU;

        struct stat st;

        for (auto iter = path.begin(); iter != path.end();) {
            auto newIter = std::find(iter, path.end(), '/');
            std::string newPath = rootPath + "/" + std::string(path.begin(), newIter);

            if (stat(newPath.c_str(), &st) != 0) {
                if (::mkdir(newPath.c_str(), mode) != 0 && errno != EEXIST) {
                    return false;
                }
            } else if (!S_ISDIR(st.st_mode)) {
                errno = ENOTDIR;
                return false;
            }

            iter = newIter;
            if (newIter != path.end()) {
                ++iter;
            }
        }
        return true;
    }

    path getcwd()
    {
        char cwd[PATH_MAX];
        if (::getcwd(cwd, sizeof(cwd)) == NULL) {
            return {};
        } else if (cwd[0] != '/') {
            return {};
        }
        return path{cwd};
    }
}
