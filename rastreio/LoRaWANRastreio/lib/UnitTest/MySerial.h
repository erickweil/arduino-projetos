#ifndef SERIAL_FAKE_H
#define SERIAL_FAKE_H

#include <vector>
#include <string>

class MySerial {
public:
    MySerial();
    void begin(int baud, int config, int rx, int tx) {}
    int available();
    int read();
    void print(const char *) {}
    void println(const char *) {}
    // Feed data into the serial buffer
    void feed(const char *s);
    void clear();
private:
    std::string buf;
    size_t idx;
    u_int32_t delay;
};

extern MySerial Serial1;

#endif
