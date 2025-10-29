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
    // Methods for unit testing
    void __feed(const char *s);
    void __delay();
    void __clear();
private:
    constexpr static char DELAY_SEP = (char)127; // DEL as separator for delays
    std::string buf;
    size_t idx;
    u_int32_t delay;
};

extern MySerial Serial1;

#endif
