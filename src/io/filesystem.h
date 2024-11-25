#pragma once

#if _MSVC_LANG >= 201703L || __cplusplus >= 201703L

// Use C++ filesystem
#include <filesystem>
namespace fs = std::filesystem;

#elif defined(__cpp_lib_experimental_filesystem)

// Some versions of C++ 14 allow you to use the experimental version of the filesystem
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#else

// Use boost file system for instances of C++ without built in C++ 17 filesystem
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#endif

#include <fstream>
#include <sstream>

namespace FileSystem
{
    class Path : public fs::path
    {
    public:
        using fs::path::path;

        Path(const fs::path& p) : fs::path(p) {}

        void rename(const fs::path& newPath)
        {
            fs::rename(*this, newPath);
        }
    };

    using RecursiveDirectoryIterator = fs::recursive_directory_iterator;
    using DirectoryIterator = fs::directory_iterator;
    using FileTimeType = fs::file_time_type;
    using DirectoryEntry = fs::directory_entry;

    inline bool exists(const Path& p)
    {
        return fs::exists(p);
    }

    inline bool isDirectory(const Path& p)
    {
        return fs::is_directory(p);
    }

    inline bool remove(const Path& p)
    {
        return fs::remove(p);
    }

    inline FileTimeType lastWriteTime(const Path& p)
    {
        return fs::last_write_time(p);
    }

    inline bool createDirectory(const Path& p)
    {
        return fs::create_directory(p);
    }

    inline bool createFullDirectory(const Path& p)
    {
        return fs::create_directories(p);
    }

    inline void removeAll(const Path& p)
    {
        fs::remove_all(p);
    }

    inline void rename(const Path& oldPath, const Path& newPath)
    {
        fs::rename(oldPath, newPath);
    }

    inline bool isRegularFile(const Path& p)
    {
        return fs::is_regular_file(p);
    }

    inline uintmax_t size(const Path& p)
    {
        return fs::file_size(p);
    }

    inline Path parent(const RecursiveDirectoryIterator& it)
    {
        return it->path().parent_path();
    }

    inline std::string filename(const RecursiveDirectoryIterator& it)
    {
        return it->path().filename().string();
    }

    inline Path getCurrentPath()
    {
        return fs::current_path();
    }

    inline std::string extension(const RecursiveDirectoryIterator& it)
    {
        return it->path().extension().string();
    }

    inline std::string read(const Path& path)
    {
        std::ifstream file(path);

        std::stringstream buf;
        buf << file.rdbuf();

        return buf.str();
    }
}