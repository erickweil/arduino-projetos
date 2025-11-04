// Minimal LittleFS mock compatible with g++11
#pragma once

#include "FS.h"

extern "C" {
    #include "lfs.h"
    #include "bd/lfs_filebd.h"
}

namespace littlefs {

constexpr lfs_size_t READ_SIZE = 16;
constexpr lfs_size_t PROG_SIZE = 16;
constexpr lfs_size_t BLOCK_SIZE = 4096;
constexpr lfs_size_t BLOCK_COUNT = 128; // 0.5 MB total
constexpr int32_t    BLOCK_CYCLES = 1024;
constexpr lfs_size_t CACHE_SIZE = 64;
constexpr lfs_size_t LOOKAHEAD_SIZE = 32;

constexpr lfs_size_t ERASE_SIZE = 4096;
constexpr lfs_size_t ERASE_COUNT = 1024;

lfs_filebd_t bd;

// https://github.com/littlefs-project/littlefs/blob/master/runners/test_runner.c
// https://github.com/joltwallet/esp_littlefs
struct lfs_config cfg = {
    .context = &bd,
    .read = lfs_filebd_read,
    .prog = lfs_filebd_prog,
    .erase = lfs_filebd_erase,
    .sync = lfs_filebd_sync,
    .read_size = READ_SIZE,
    .prog_size = PROG_SIZE,
    .block_size = BLOCK_SIZE,
    .block_count = BLOCK_COUNT,
    .block_cycles = BLOCK_CYCLES,
    .cache_size = CACHE_SIZE,
    .lookahead_size = LOOKAHEAD_SIZE
};

struct lfs_filebd_config bdcfg = {
    .read_size          = READ_SIZE,
    .prog_size          = PROG_SIZE,
    .erase_size         = ERASE_SIZE,
    .erase_count        = ERASE_COUNT
};

const char *path = "./littlefs_mock.img";

lfs_t lfs;

} // namespace littlefs

class File {
public:
    File(std::shared_ptr<lfs_file_t> p = std::shared_ptr<lfs_file_t>()) : lfs_file(p) {}

    size_t read(uint8_t *buffer, size_t length) {
        if (!lfs_file) return 0;

        lfs_ssize_t n = lfs_file_read(&littlefs::lfs, lfs_file.get(), buffer, length);
        if (n < 0) return 0;
        return static_cast<size_t>(n);
    }

    size_t write(const uint8_t *buffer, size_t length) {
        if (!lfs_file) return 0;
        
        lfs_ssize_t n = lfs_file_write(&littlefs::lfs, lfs_file.get(), buffer, length);
        if (n < 0) return 0;
        return static_cast<size_t>(n);
    }

    bool seek(uint32_t position, SeekMode mode = SeekSet) {
        if (!lfs_file) return false;

        int whence = LFS_SEEK_SET;
        if (mode == SeekCur)
            whence = LFS_SEEK_CUR;
        else if (mode == SeekEnd)
            whence = LFS_SEEK_END;
        lfs_off_t res = lfs_file_seek(&littlefs::lfs, lfs_file.get(), position, whence);
        return res >= 0;
    }

    size_t size() const {
        if (!lfs_file) return 0;

        lfs_off_t sz = lfs_file_size(&littlefs::lfs, lfs_file.get());
        if (sz < 0) return 0;
        return static_cast<size_t>(sz);
    }

    void close() {
        if (!lfs_file) return;

        lfs_file_close(&littlefs::lfs, lfs_file.get());
        lfs_file = nullptr;
    }

    explicit operator bool() const {
        return lfs_file != nullptr;
    }
private:
    std::shared_ptr<lfs_file_t> lfs_file;
};

class LittleFSMockClass {
public:
    bool begin(bool) {
        if (mounted) return true;

        int err = lfs_filebd_create(&littlefs::cfg, littlefs::path, &littlefs::bdcfg);
        if (err < 0) {
            printf("lfs_filebd_create failed: %d\n", err);
            return false;
        }

        err = lfs_format(&littlefs::lfs, &littlefs::cfg);
        if (err < 0) {
            printf("lfs_format failed: %d\n", err);
            return false;
        }

        err = lfs_mount(&littlefs::lfs, &littlefs::cfg);
        if (err < 0) {
            printf("lfs_mount failed: %d\n", err);
            return false;
        }

        mounted = true;
        return true;
    }

    void end() {
        if (!mounted) return;

        int err = lfs_unmount(&littlefs::lfs);
        if (err < 0) {
            printf("lfs_unmount failed: %d\n", err);
            return;
        }

        err = lfs_filebd_destroy(&littlefs::cfg);
        if (err < 0) {
            printf("lfs_filebd_destroy failed: %d\n", err);
            return;
        }

        mounted = false;
    }

    File open(const char *path, const char *mode = FILE_READ) {
        if (!mounted || !path)
            return File();

        int flags = 0;
        if (strcmp(mode, FILE_READ) == 0) {
            flags = LFS_O_RDONLY;
        } else if (strcmp(mode, FILE_WRITE) == 0) {
            flags = LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC;
        } else if (strcmp(mode, FILE_APPEND) == 0) {
            flags = LFS_O_WRONLY | LFS_O_CREAT | LFS_O_APPEND;
        } else {
            // Unsupported mode
            return File();
        }

        auto file = std::make_shared<lfs_file_t>();
        int err = lfs_file_open(&littlefs::lfs, file.get(), path, flags);
        if (err < 0) {
            return File();
        }

        return File(file);
    }

    bool exists(const char *path) {
        if (!mounted)
            return false;

        File file = open(path, FILE_READ);
        if (!file) {
            return false;
        }
        file.close();
        return true;
    }

    bool remove(const char *path) {
        if (!mounted || !path)
            return false;

        int err = lfs_remove(&littlefs::lfs, path);
        return err >= 0;
    }

    bool rename(const char *from, const char *to) {
        if (!mounted || !from || !to)
            return false;

        int err = lfs_rename(&littlefs::lfs, from, to);
        return err >= 0;
    }

    size_t totalBytes() const {
        return littlefs::cfg.block_size * littlefs::cfg.block_count;
    }

    size_t usedBytes() const {
        size_t used_bytes = std::min(totalBytes(), (size_t)(littlefs::cfg.block_size * lfs_fs_size(&littlefs::lfs)));
        return used_bytes;
    }
private:
    bool mounted = false;
};

LittleFSMockClass LittleFS;
