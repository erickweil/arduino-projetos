#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

// Minimal Preferences mock for native unit tests (single-threaded).
// Stores all types as int64_t internally for simplicity
static std::unordered_map<std::string, int64_t> store;

class Preferences {
public:
    Preferences() = default;

    bool begin(const char *ns, bool readOnly)
    {
        (void)ns; (void)readOnly;
        return true;
    }

    uint32_t getULong(const char *key, uint32_t def)
    {
        auto it = store.find(std::string(key));
        if (it == store.end())
            return def;
        return static_cast<uint32_t>(it->second);
    }

    void putULong(const char *key, uint32_t value)
    {
        store[std::string(key)] = static_cast<int64_t>(value);
    }

    uint8_t getUChar(const char *key, uint8_t def)
    {
        auto it = store.find(std::string(key));
        if (it == store.end())
            return def;
        return static_cast<uint8_t>(it->second);
    }

    void putUChar(const char *key, uint8_t value)
    {
        store[std::string(key)] = static_cast<int64_t>(value);
    }

    int32_t getInt(const char *key, int32_t def)
    {
        auto it = store.find(std::string(key));
        if (it == store.end())
            return def;
        return static_cast<int32_t>(it->second);
    }

    void putInt(const char *key, int32_t value)
    {
        store[std::string(key)] = static_cast<int64_t>(value);
    }

    void end() {}

    // Test helper: reset all stored values
    static void reset()
    {
        store.clear();
    }
};