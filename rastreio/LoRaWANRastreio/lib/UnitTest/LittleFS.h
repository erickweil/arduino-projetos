// Minimal LittleFS mock compatible with g++11
#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

#ifndef FILE_READ
#define FILE_READ 0x01
#endif
#ifndef FILE_WRITE
#define FILE_WRITE 0x02
#endif
#ifndef FILE_APPEND
#define FILE_APPEND 0x04
#endif

enum SeekMode : uint8_t
{
    SeekSet = 0,
    SeekCur = 1,
    SeekEnd = 2
};

class File
{
public:
    File(FILE *p = nullptr) : fp(p), _path("") {}
    ~File()
    {
        close();
    }

    size_t read(uint8_t *buffer, size_t length)
    {
        if (!fp)
            return 0;
        size_t n = fread(buffer, 1, length, fp);
        return n;
    }

    size_t write(const uint8_t *buffer, size_t length)
    {
        if (!fp)
            return 0;
        size_t n = fwrite(buffer, 1, length, fp);
        fflush(fp);
        return n;
    }

    bool seek(uint32_t position, SeekMode mode = SeekSet)
    {
        if (!fp)
            return false;
        int whence = SEEK_SET;
        if (mode == SeekCur)
            whence = SEEK_CUR;
        else if (mode == SeekEnd)
            whence = SEEK_END;
        return fseek(fp, static_cast<long>(position), whence) == 0;
    }

    size_t size() const
    {
        if (!fp)
            return 0;
        long cur = ftell(const_cast<FILE *>(fp));
        if (cur < 0)
            return 0;
        if (fseek(const_cast<FILE *>(fp), 0, SEEK_END) != 0)
            return 0;
        long end = ftell(const_cast<FILE *>(fp));
        if (end < 0)
            return 0;
        // restore
        fseek(const_cast<FILE *>(fp), cur, SEEK_SET);
        return static_cast<size_t>(end);
    }

    void close()
    {
        if (fp)
        {
            fclose(fp);
            fp = nullptr;
        }
    }

    explicit operator bool() const
    {
        return fp != nullptr;
    }

    const char* path() const
    {
        return _path.c_str();
    }

private:
    File(FILE *f, const std::string &p) : fp(f), _path(p) {}

    FILE *fp;
    std::string _path;

    friend class LittleFSMockClass;
};

// Helper: create directories recursively
static bool mkdirs(const std::string &path)
{
    if (path.empty())
        return false;
    size_t pos = 0;
    std::string current;
    if (path[0] == '/')
        pos = 1;
    while (true)
    {
        size_t next = path.find('/', pos);
        current = path.substr(0, next == std::string::npos ? path.size() : next);
        if (current.size() > 0)
        {
            struct stat st;
            if (stat(current.c_str(), &st) != 0)
            {
                if (mkdir(current.c_str(), 0755) != 0 && errno != EEXIST)
                    return false;
            }
            else if (!S_ISDIR(st.st_mode))
            {
                return false;
            }
        }
        if (next == std::string::npos)
            break;
        pos = next + 1;
    }
    return true;

}

class LittleFSMockClass
{
public:
    LittleFSMockClass() : mounted(false)
    {
        // Default root path is a hidden directory inside the project working dir.
        // Using a relative path reduces risk of touching arbitrary system files.
        rootPath = std::string("./test/.littlefs_mock");
    }

    bool begin(bool /*formatOnFail*/ = false)
    {
        if (mounted)
            return true;
        // create root path if needed
        struct stat st;
        if (stat(rootPath.c_str(), &st) != 0)
        {
            if (!mkdirs(rootPath))
                return false;
        }
        mounted = true;
        return true;
    }

    void end()
    {
        mounted = false;
    }

    File open(const char *path, uint8_t mode)
    {
        if (!mounted || !path)
            return File();

        std::string rel(path);
        // strip leading '/'
        while (!rel.empty() && rel[0] == '/')
            rel.erase(0, 1);
        std::string full = rootPath + "/" + rel;

        // ensure parent
        size_t p = full.find_last_of('/');
        if (p != std::string::npos)
        {
            std::string parent = full.substr(0, p);
            mkdirs(parent);
        }

        const char *modestr = "rb";
        if (mode & FILE_APPEND)
            modestr = "ab+"; // append and allow read
        else if (mode & FILE_WRITE)
            modestr = "wb+"; // truncate and allow read

        FILE *f = fopen(full.c_str(), modestr);
        if (!f)
        {
            // if opened for read but file missing, return empty
            if ((mode & FILE_READ) && !(mode & (FILE_WRITE | FILE_APPEND)))
                return File();
            // try to create
            f = fopen(full.c_str(), "wb+");
            if (!f)
                return File();
            if (mode & FILE_APPEND)
                fseek(f, 0, SEEK_END);
        }

        if (mode & FILE_APPEND)
            fseek(f, 0, SEEK_END);

        return File(f, full);
    }

    bool exists(const char *path) const
    {
        if (!mounted || !path)
            return false;
        std::string rel(path);
        while (!rel.empty() && rel[0] == '/')
            rel.erase(0, 1);
        std::string full = rootPath + "/" + rel;
        struct stat st;
        return stat(full.c_str(), &st) == 0;
    }

    bool remove(const char *path)
    {
        if (!mounted || !path)
            return false;
        std::string rel(path);
        while (!rel.empty() && rel[0] == '/')
            rel.erase(0, 1);
        std::string full = rootPath + "/" + rel;
        return unlink(full.c_str()) == 0;
    }

    bool rename(const char *from, const char *to)
    {
        if (!mounted || !from || !to)
            return false;
        std::string relFrom(from);
        while (!relFrom.empty() && relFrom[0] == '/')
            relFrom.erase(0, 1);
        std::string relTo(to);
        while (!relTo.empty() && relTo[0] == '/')
            relTo.erase(0, 1);
        std::string fullFrom = rootPath + "/" + relFrom;
        std::string fullTo = rootPath + "/" + relTo;

        // ensure parent for destination
        size_t p = fullTo.find_last_of('/');
        if (p != std::string::npos)
        {
            std::string parent = fullTo.substr(0, p);
            mkdirs(parent);
        }

        return ::rename(fullFrom.c_str(), fullTo.c_str()) == 0;
    }

    size_t totalBytes() const
    {
        // For mock, return a large fixed size
        return 16 * 1024 * 1024; // 16 MB
    }

    size_t usedBytes() const
    {
        // For mock, return 0
        return 0;
    }
private:
    std::string rootPath;
    bool mounted;
};

LittleFSMockClass LittleFS;